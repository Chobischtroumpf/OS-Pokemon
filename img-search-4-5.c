#include <stdio.h>      // printf, perror
#include <sys/types.h>  // pid_t
#include <unistd.h>     // fork, exec
#include <signal.h>
#include <stdlib.h>
#include <pthread.h>

//#include "utils.h"
//#include <sys/wait.h>   //wait
//#include <string.h>     //strlen,strcmp (si besoin)
//#include <stdbool.h>
//#include <stdatomic.h>
//#include <time.h>
//#include <sys/mman.h>

//rappel:
//read = 0
//write = 1

/* Tu penses qu'on en a besoin ?
void *create_shared_memory(size_t size) {

    //Vient du cours TP5 ex4

    const int protection = PROT_READ | PROT_WRITE;
    const int visibility = MAP_SHARED | MAP_ANONYMOUS;
    return mmap(NULL, size, protection, visibility, -1, 0);
}
*/


void fils(int fd_read/*[2]*/) { //Je sais pas s'il faut faire une fonction pour créer une mémoire partagée comme le TP5 ex4 ?

    //prend pipe (+img à comparer?), appelle img-dist, récupère val dist, si < qu'avant, sauvegarde @ mémoire partagée
    //=> les 3 processus pourraient acceder à même zone, du coup fils comparent w/ val en mémoire puis quand fini, père récup valeur?

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
        pkz daeforgbjn Je sais pas comment lire la valeur que renvoie img-dist aled
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

int main(){

    /*
    CREER LA MEMOIRE PARTAGEE
    int *shared_memory = create_shared_memory(sizeof(jsp?));
    */

    pid_t pid1 = fork(); //creation fils 1

    //pipes: transmettent chemins imgs
    int fd_p_f1[2], fd_p_f2[2]; // creation pipes 
    //int fd_f1_p[2], fd_f1_p[2]; //, probablement pas besoin de f->p

    if (pid1>0){ //code fils 1
        pid_t pid2 = fork(); //creation fils 2


        if (pid2>0){ //code fils 2
            printf("ok");
        }

        else if (pid2<0){ //erreur creation fils 2
            perror("fork 2");
            exit(1);

        }
    }
    else if (pid1==0){
        //signal(SIGPIPE,handler_sigpipe);
        printf("processus père");
        //collectione imgs stdin , envoie première @ fils1, deuxiè:e @f2, 3ème @f1 etc
        //fgets
        // var pour  fget?
        //int counter =0;
        // while (pas Ctrl+D){fgets, if (counter%2==0) write @ f1 else write @ f2}

        //if:
        //close(fd_p_f1[0])
        //close(fd_p_f2[0])
        //close(fd_p_f2[1])
        // write(fd_p_f1[1],&var_récup_fgets,sizeof(var_récup_fgets))
        //close(fd_p_f1[1])


        //else:
        //close(fd_p_f1[0])
        //close(fd_p_f2[0])
        //close(fd_p_f1[1])
        //write(fd_p_f2[1],&var_récup_fgets,sizeof(var_récup_fgets))
        //close(fd_p_f2[1])
    }
    else {                  //erreur creation fils 1
        perror("fork 1");
        exit(1);
    }
    return 0;
}
