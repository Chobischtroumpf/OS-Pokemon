/*
**
** Projet 1 du cours de systèmes d'exploitation informatiques
** Auteurs : Alessandro Dorigo, Nha Thy Truong, Danae Veyret
** fichier : main.c
**
*/

#include "utils.h"

sem_t sem_memoire_partagee; // pour gérer var partagée, voir si pipes = vides ou non, doit avoir meilleure façon de determiner valeurs
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
}// end create_shared_memory

char *get_input_file(FILE *inputdes, int pipe[2]){
    char *otherimg = NULL;
    static char tmp[1000];
    bzero(tmp, 1000);

    if (fgets(tmp, 1000, inputdes) == -1){handle_error(pipe, inputdes, NULL, NULL);}

    otherimg = (char *)malloc(sizeof(char) * (strlen(tmp)-1));
    if (otherimg == NULL){handle_error(pipe, inputdes, NULL, NULL);}

    memcpy(otherimg, tmp, strlen(tmp)-1);
    otherimg[strlen(tmp)-1] = '\0';
    return otherimg;
}//  end get_input_file

int exec_img_dist(char *baseimg, char *otherimg, int pipes[2], FILE *inputdes){
    pid_t pid;
    int status;

    pid = fork();
    if (pid == 0){
        if (execlp("img-dist", "-v", baseimg, otherimg, NULL) == -1)
            handle_error(pipes,inputdes,baseimg,otherimg);
    }
    else{
        if (wait(&status) == -1){handle_error(pipes,inputdes,baseimg,otherimg);}
            
        if (WIFEXITED(status)){return WEXITSTATUS(status);}
            
    }
    return 65;
}// end exec_img_dist

void fils(int fd_pipe[2], char *baseimg, t_img_dist *shared_memory){ //regarder si bien passé shared_memory

    //prend pipe (+img à comparer?), appelle img-dist, récupère val dist, si < qu'avant, sauvegarde @ mémoire partagée
    //=> les 3 processus pourraient acceder à même zone, du coup fils comparent w/ val en mémoire puis quand fini, père récup valeur?
 
    int dist;
    FILE *fd_read = fdopen(fd_pipe[0], "r");

    while(true){
        char *otherimg = get_input_file(fd_read);
        if (otherimg == NULL){break;}
            
        // compare images
        dist = exec_img_dist(baseimg, otherimg);

        // update closest image
        sem_wait(&sem_memoire_partagee);  //add at père aussi durant lecture @fin
        if (dist < shared_memory->dist){
            shared_memory->dist = dist;
            bzero(shared_memory->path, 1000);
            strcpy(shared_memory->path, otherimg);
        }
        sem_post(&sem_memoire_partagee);

        if (otherimg != NULL){free(otherimg);}

    }
    fclose(fd_read);
    close(fd_pipe[0]);
    close(fd_pipe[1]);
    exit(0);
}
    //sem_post(tant que c'est pas plein)?!!!!!!!!!
    //end fils

// void handler_sigint(int signal){ //handler SIGINT
//     if (signal == SIGINT){
//         int status;
//         //for(int i = 0; i<2;i++){wait(NULL)} //??? - repredre code main?
//         if (wait(&status)>0){ //en théorie ça fonctionne non...
//             printf("SIGINT - Ctrl+c - géré par père - fils doivent se terminer de façon propre");
//         }
//         else{
//             perror("wait");
//         }//end wait
//         exit(0);
//     }
// }// end handler_sigint

// void handler_sigint_fils(int signal){ //handler SIGINT
//     if (signal == SIGINT){
//         printf("ignore");   
//     }
// }// end handler_sigint_fils

// void handler_sigpipe(int signal){ //handler SIGPIPE
//     if (signal == SIGPIPE){
//         //kill(getpid()) //-> géré par tous les processus?
//         printf("SIGPIPE - processus doivent se terminer de façon propre ");
//         exit(0);
//     }
// }// end handler_sigpipe

int main(int argc, char* argv[]){ //pour récupèrer le path

    if (argc != 2){
        printf("Usage: %s <path to file>\n", argv[0]);
        return 1;
    }

    t_img_dist *shared_memory = (t_img_dist *)create_shared_memory(sizeof(t_img_dist)); //voir comment retourner path    
    shared_memory->dist=65;

    char *baseimg = ft_strdup(argv[1]);

    //pipes: transmettent chemins imgs
    int pipe_fils1[2], pipe_fils2[2]; // creation pipes 

    if (pipe(pipe_fils1) < 0){perror("pipe1 fail"); exit(1);}
    if (pipe(pipe_fils2) < 0){perror("pipe2 fail"); exit(1);}

    pid_t pid1,pid2;
    pid1 = fork(); //creation fils 1
    if (pid1>0){ //code fils 1
        //signal(SIGINT,handler_sigint_fils);
        printf("ok1");

        fils(pipe_fils1, baseimg, shared_memory); //si init 0, peut-être passer par ref si veux décrementer? (voir fils) SI OUI: UTILISER SEMAPHORE PLUTOT
    }
    else if (pid1==0){
        // signal(SIGINT, handler_sigint);
        // signal(SIGPIPE, handler_sigpipe); //pas écrit encore, si soucis: commentaire
        printf("processus père");
        
        pid2 = fork(); //creation fils 2

        if (pid2>0){ //code fils 2
            //signal(SIGINT,handler_sigint_fils);
            printf("ok2");
            fils(pipe_fils2, baseimg, shared_memory);
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
            close(pipe_fils1[0]);
            close(pipe_fils2[0]);
            close(pipe_fils2[1]);
            write(pipe_fils1[1], &recup_fgets, sizeof(recup_fgets));
            close(pipe_fils1[1]);
            }
            else{
            close(pipe_fils1[0]);
            close(pipe_fils2[0]);
            close(pipe_fils1[1]);
            write(pipe_fils2[1], &recup_fgets, sizeof(recup_fgets));
            close(pipe_fils2[1]);
            }
            counter++;
        }//end while
        
        int status1;
        int status2;

        int ret_val;
        char ret_path;

        wait(&status1);
        wait(&status2);
        if (WIFEXITED(status1) && WIFEXITED(status2)){
            if (WEXITSTATUS(status1)==0 && WEXITSTATUS(status2)==0){
                printf("wait ok");
                sem_wait(&sem_memoire_partagee);  //lire var en mémoire partagée
                ret_val = shared_memory->dist;    
                ret_path = shared_memory->path;
                sem_post(&sem_memoire_partagee);
            }
        }
        else if (WIFSIGNALED(status1) || WIFSIGNALED(status2))
        {
            printf("No similar image found (no comparison could be performed successfully).");
        }
        else
        {
            perror(errno);
        }//end wait
        


        if (ret_val < 65){
            printf("Most similar image found: %s with a distance of %d.", ret_path, ret_val);
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