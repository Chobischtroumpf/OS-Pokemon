#include <stdio.h>      // printf, perror
#include <sys/types.h>  // pid_t
#include <unistd.h>     // fork, exec
#include <signal.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <time.h>

#include "img-dist/img-dist"  // FAUT INCLURE LE FICHIER, path relatif

#include <sys/wait.h>   //wait
//#include <string.h>     //strlen,strcmp (si besoin)
//#include <stdbool.h>
//#include <stdatomic.h>

#include "utils.h"

//rappel:
//read = 0
//write = 1

//int dist = 65; //var partagée


sem_t sem_memoire_partagee, vide1, vide2, plein1, plein2; // pour gérer var partagée, voir si pipes = vides ou non, doit avoir meilleure façon de determiner valeurs
pthread_mutex_t mutex; //ou mutex plutot? (pour var partagée)
const int MAX_PATH = 256;

struct img_dist{
    int dist;
    char path[1000];
} 
typedef t_img_dist;

void *create_shared_memory(size_t size){
    //Vient du cours TP5 ex4
    const int protection = PROT_READ | PROT_WRITE;
    const int visibility = MAP_SHARED | MAP_ANONYMOUS;
    return mmap(NULL, size, protection, visibility, -1, 0); //-> verifier parametres
}

char *get_input_fd(int fd){// adapter pour lire sur file qu'on veut
    char *otherimg;
    static char tmp[1000];
    bzero(tmp, 1000);

    if (read(fd, tmp, 1000) == -1)
        handle_error();

    otherimg = (char *)malloc(sizeof(char) * (strlen(tmp)-1));
    if (otherimg == NULL)
        handle_error();

    memcpy(otherimg, tmp, strlen(tmp)-1);
    otherimg[strlen(tmp)-1] = '\0';
    return otherimg;
}

int exec_img_dist(char *baseimg, char *otherimg){
   pid_t pid;
   int status;
   

   pid = fork();
   if (pid == 0){
      if (execlp("img-dist", "-v", baseimg, otherimg, NULL) == -1)
         handle_error();
   }
   else{
      if (wait(&status) == -1)
         handle_error();
      if (WIFEXITED(status))
         return WEXITSTATUS(status);
   }
   return 65;
}

void fils(int fd_read, char *baseimg, t_img_dist *shared_memory){ //regarder si bien passé shared_memory

    //prend pipe (+img à comparer?), appelle img-dist, récupère val dist, si < qu'avant, sauvegarde @ mémoire partagée
    //=> les 3 processus pourraient acceder à même zone, du coup fils comparent w/ val en mémoire puis quand fini, père récup valeur?
    
    char *closest_img = NULL;
    int closest_dist = 65;
    int dist;

   while(true){
        char *otherimg = get_input();
        if (otherimg == NULL)
            break;
        // compare images
        dist = exec_img_dist(baseimg, otherimg);

        sem_wait(&sem_memoire_partagee);  //add at père aussi durant lecture @fin
        if (dist < shared_memory->dist){
            shared_memory->dist = dist;
            bzero(shared_memory->path, 1000);
            strcpy(shared_memory->path, otherimg);
        }
        sem_post(&sem_memoire_partagee);

      // update closest image
        if (dist < closest_dist){
            if (closest_img != NULL)
                free(closest_img);
            closest_dist = dist;
            closest_img = ft_strdup(otherimg);
            if (closest_img == NULL)
                handle_error();
        
        if (otherimg != NULL)
            free(otherimg);
        }

   }
    //sem_post(tant que c'est pas plein)?!!!!!!!!!
}//end fils



void handler_sigint(int signal){ //handler SIGINT
    if (signal == SIGINT){
        int status;
        if (wait(&status)>0){ //en théorie ça fonctionne non...
            printf("SIGINT - Ctrl+c - géré par père - fils doivent se terminer de façon propre");
        }
        else{
            perror("wait");
        }//end wait
        exit(0);
    }
}// end handler_sigint

void handler_sigint_fils(int signal){ //handler SIGINT
    if (signal == SIGINT){
        printf("ignore");
        
    }//end wait
    exit(0);
 }// end handler_sigint

void handler_sigpipe(int signal){ //handler SIGPIPE
    if (signal == SIGPIPE){
        //kill(getpid()) //-> géré par tous les processus?
        printf("SIGPIPE - processus doivent se terminer de façon propre ");
        exit(0);
    }
}// end handler_sigpipe




int main(int argc, char* argv[]){ //pour récupèrer le path

    //if (argc<?){printf("No similar image found (no comparison could be performed successfully).");exit(0);}

    t_img_dist *shared_memory = (t_img_dist *)create_shared_memory(sizeof(t_img_dist)); //voir comment retourner path    
    shared_memory->dist=65;

    char *baseimg = ft_strdup(argv[1]);

    //VARIABLES PEUT-ÊTRE SUFFISENT??? car pipes pas partagés 
    sem_init(&vide1, 0, 0);
    sem_init(&vide2, 0, 0);
    sem_init(&plein1, 0, 51);
    sem_init(&plein2, 0, 50);


    int img_tot = 101; //PROBABLEMENT PAS COMMENT ON DOIT FAIRE
    /*int plein_var1 =51;
    int plein_var2 =50;*/ 
    //OU
    int plein_var1 = 0;
    int plein_var2 = 0;

    pid_t pid1,pid2;
    
    pid1 = fork(); //creation fils 1

    //pipes: transmettent chemins imgs
    int fd_p_f1[2], fd_p_f2[2]; // creation pipes 
    //int fd_f1_p[2], fd_f2_p[2]; //, probablement pas besoin de f->p

    if (pipe(fd_p_f1)<0){perror("pipe1 fail"); exit(1);}
    if (pipe(fd_p_f2)<0){perror("pipe2 fail"); exit(1);}

    if (pid1>0){ //code fils 1
        //signal(SIGINT,handler_sigint_fils);
        printf("ok1");
        fils(fd_p_f1[0], baseimg, shared_memory); //si init 0, peut-être passer par ref si veux décrementer? (voir fils) SI OUI: UTILISER SEMAPHORE PLUTOT
    }
    else if (pid1==0){
        signal(SIGINT, handler_sigint);
        signal(SIGPIPE, handler_sigpipe); //pas écrit encore, si soucis: commentaire
        printf("processus père");
        
        pid2 = fork(); //creation fils 2

        if (pid2>0){ //code fils 2
            //signal(SIGINT,handler_sigint_fils);
            printf("ok2");
            fils(fd_p_f2[0], baseimg, shared_memory);
        }

        else if (pid2<0){ //erreur creation fils 2
            perror("fork 2");
            exit(1);

        }//voir si faut mettre code du père dans else de pid2 

        //collectione imgs stdin , envoie première @ fils1, deuxiè:e @f2, 3ème @f1 etc

        int counter =0;
        char recup_fgets;

        while (fgets(&recup_fgets, 256, stdin)){    //à voir 256
            if (counter%2==0){
            close(fd_p_f1[0]);
            close(fd_p_f2[0]);
            close(fd_p_f2[1]);
            write(fd_p_f1[1], &recup_fgets, sizeof(recup_fgets));
            close(fd_p_f1[1]);
            plein_var1 ++;
            }
    
            else{
            close(fd_p_f1[0]);
            close(fd_p_f2[0]);
            close(fd_p_f1[1]);
            write(fd_p_f2[1], &recup_fgets, sizeof(recup_fgets));
            close(fd_p_f2[1]);
            plein_var2 ++;
            }
            counter++;
        }//end while
        
        int status;

        int ret_val;
        //char ret_path;

        if (wait(&status)>0){
            printf("wait ok");
            sem_wait(&sem_memoire_partagee);  //lire var en mémoire partagée
            ret_val = shared_memory[0] ;    
            sem_post(&sem_memoire_partagee);
        //print dist & img 
        }
        else{
            perror("wait");
        }//end wait
        


        if (ret_val<65){
            //printf("Most similar image found: %s with a distance of %d.",ret_path,ret_val);
        }
        else{
            printf("No similar image found (no comparison could be performed successfully).");
        }
        
    }//end pid==0 (père)

    else{                  //erreur creation fils 1
        perror("fork 1");
        exit(1);
    }
    return 0;
}//end main




/*QUESTIONS POUR ALESSANDRO :
- les sémaphores pour les fils : le fils ne commence pas à exécuter le code tant que le pipe n'est pas plein
- comment récupérer la distance ligne 53
- comment récupérer le path vers l'image qui a la distance la plus courte
- signal(sigint, handler2) chez les fils pour qu'ils l'ignorent?
*/