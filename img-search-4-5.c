#include <stdio.h>      // printf, perror
#include <sys/types.h>  // pid_t
#include <unistd.h>     // fork, exec
#include <signal.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/mman.h>

#include "img-dist/img-dist"  // FAUT INCLURE LE FICHIER, path relatif

//#include <sys/wait.h>   //wait
//#include <string.h>     //strlen,strcmp (si besoin)
//#include <stdbool.h>
//#include <stdatomic.h>
//#include <time.h>

//rappel:
//read = 0
//write = 1

int dist = 65; //var partagée
sem_t sem_memoire_partagee, vide1,vide2,plein1,plein2; // pour gérer var partagée, voir si pipes = vides ou non, doit avoir meilleure façon de determiner valeurs
pthread_mutex_t mutex; //ou mutex plutot? (pour var partagée)




//Tu penses qu'on en a besoin ? - oui car on a besoin de la mémoire partagée
void *create_shared_memory(size_t size) {

    //Vient du cours TP5 ex4

    const int protection = PROT_READ | PROT_WRITE;
    const int visibility = MAP_SHARED | MAP_ANONYMOUS;
    return mmap(NULL, size, protection, visibility, -1, 0); //-> verifier parametres
}



void fils(int fd_read/*[2]*/, int num_img) { //Je sais pas s'il faut faire une fonction pour créer une mémoire partagée comme le TP5 ex4 ?

    //prend pipe (+img à comparer?), appelle img-dist, récupère val dist, si < qu'avant, sauvegarde @ mémoire partagée
    //=> les 3 processus pourraient acceder à même zone, du coup fils comparent w/ val en mémoire puis quand fini, père récup valeur?


    // pas besoin de creer de pipe ou processus dans fils! géré dans main
    /* ESSAI
    pipe(fd_read);
    pid_t pid = fork();
    if (pid < 0){
        perror("Pipe failed");
    }
    else if (pid == 0){
        int val_dist;
        int max_path = 256;
        int *shared_memory = create_shared_memory(sizeof(jsp?));    Je sais pas quoi mettre pour la taille ? max_path ?
        close(fd_read[1]);
        pkz daeforgbjn Je sais pas comment lire la valeur que renvoie img-dist aled - val_dist = "./img-dist first second" //je crois

        safe_read(fd_read[0], &val_dist, sizeof(max_path));
        
        
        *shared_memory = val_dist;
        close(fd_read[0]);
    }
    else if (pid > 0){
        close(fd_read[1]);
        waitpid(pid, NULL????, 0);      Apparemment je dois mettre le *status mais je sais pas?
        int final_dist = *shared_memory     Normalement ça fait que le père récupère la valeur ?
        close(fd_read[0]);
    }
    */

   /*
    //récuperer img à comparer via read?
    char to_cmp = "";                       //char? ""?


   char img_path="";    
    int max_path = 256;
    int val_dist;
   for(int i=0;i<num_img;i++){      //comparaisons                // au lieu : while(num_img>0)
    read(fd_read[0],&val_dist,sizeof(max_path));
    val_dist = "./img-dist to_cmp img_path" //adjacent... 
    if (val_dist<mémoire partagée){
        mémoire partagée = val dist;
    }

   }
   
   */
}


void handler_sigint(int signal){ //handler SIGINT
    if (signal == SIGINT){
        //Je sais pas s'il faut écrire quelque chose?
        printf("SIGINT - Ctrl+c - géré par père - fils doivent se terminer de façon propre");
        exit(0);
    }
}

void handler_sigpipe(int signal){ //handler SIGPIPE
    if (signal == SIGPIPE){
        //Je sais pas s'il faut écrire quelque chose?
        printf("SIGPIPE - processus doivent se terminer de façon propre ");
        exit(0);
    }
}

int main(int argc, char* argv[]){ //pour récupèrer le path

    /*
    CREER LA MEMOIRE PARTAGEE
    int *shared_memory = create_shared_memory(sizeof(jsp?));
    */

    //VARIABLES PEUT-ÊTRE SUFFICENT??? car pipes pas partagés
    sem_init(&vide1,0,0);
    sem_init(&vide2,0,0);
    sem_init(&plein1,0,51);
    sem_init(&plein2,0,50);


    int img_tot = 101; //PROBABLEMENT PAS COMMENT ON DOIT FAIRE
    /*int plein_var1 =51;
    int plein_var2 =50;*/ 
    //OU
    int plein_var1 =0;
    int plein_var2=0 ;


    pid_t pid1,pid2;
    
    pid1 = fork(); //creation fils 1

    //pipes: transmettent chemins imgs
    int fd_p_f1[2], fd_p_f2[2]; // creation pipes 
    //int fd_f1_p[2], fd_f1_p[2]; //, probablement pas besoin de f->p

    if(pipe(fd_p_f1)<0){perror("pipe1 fail"); exit(1)}
    if(pipe(fd_p_f2)<0){perror("pipe2 fail"); exit(1)}

    if (pid1>0){ //code fils 1
        printf("ok1");
        fils(fd_p_f1,plein_var1); //si init 0, peut-être passer par ref si veux décrementer? (voir fils) SI OUI: UTILISER SEMAPHORE PLUTOT
    }
    else if (pid1==0){
        signal(SIGPIPE,handler_sigpipe); //pas écrit encore, si soucis: commentaire
        printf("processus père");
        
        pid2 = fork(); //creation fils 2

        if (pid2>0){ //code fils 2
            printf("ok2");
            fils(fd_p_f2,plein_var2);
        }

        else if (pid2<0){ //erreur creation fils 2
            perror("fork 2");
            exit(1);

        }

        //collectione imgs stdin , envoie première @ fils1, deuxiè:e @f2, 3ème @f1 etc

        // var pour  fget?
        int counter =0;

        //inserer img à comparer dans chaque pipe en premier? 





        // while (pas Ctrl+D){fgets, if (counter%2==0) write @ f1 else write @ f2}

        /*while(counter<img_tot) {//seule idée pour l'instant...
        if (counter%2==0){
        close(fd_p_f1[0]);
        close(fd_p_f2[0]);
        close(fd_p_f2[1]);
        write(fd_p_f1[1],&var_récup_fgets,sizeof(var_récup_fgets));
        close(fd_p_f1[1]);
        plein_var1 ++;
        }
        


        else {
        close(fd_p_f1[0]);
        close(fd_p_f2[0]);
        close(fd_p_f1[1]);
        write(fd_p_f2[1],&var_récup_fgets,sizeof(var_récup_fgets));
        close(fd_p_f2[1]);
        plein_var2 ++;
        }
        counter++;
        }*/
       
    }
    else {                  //erreur creation fils 1
        perror("fork 1");
        exit(1);
    }
    return 0;
}
