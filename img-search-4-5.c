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

/*void fils(int fd_read) {
    prend pipe (+img à comparer?), appelle img-dist, récupère val dist, si < qu'avant, sauvegarde @ mémoire partagée
    => les 3 processus pourraie,t acceder à même zone, du coup fils comparent w/ val en mémoirepuis quand fini, père récup valeur?



}*/


/*void handler_sigint(int signal){ //handler SIGINT
    if (signal == SIGINT){
        printf("SIGINT - Ctrl+c - géré par père - fils doivent se terminer de façon propre");
        exit(0);
    }
}*/

/*void handler_sigpipe(int signal){ //handler SIGINT
    if (signal == SIGPIPE){
        printf("SIGPIPE - processus doivent se terminer de façon propre ");
        exit(0);
    }
}*/

int main(){
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
    exit(1);}
return 0;
}
