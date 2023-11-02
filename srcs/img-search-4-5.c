/*
**
** Projet 1 du cours de systèmes d'exploitation informatiques
** Auteurs : Alessandro Dorigo, Nha Thy Truong, Danae Veyret
** fichier : main.c
**
*/

#include "utils.h"

t_img_dist *create_shared_memory(size_t size){

    t_img_dist *shared_memory;
    
    //create shared memory
    const int protection = PROT_READ | PROT_WRITE;
    const int visibility = MAP_SHARED | MAP_ANONYMOUS;
    shared_memory = (t_img_dist*)mmap(NULL, size, protection, visibility, -1, 0);
    if ((void *)shared_memory == MAP_FAILED){
        handle_error(NULL, NULL, NULL);
    }

    //init shared memory
    shared_memory->dist=65;
    bzero(shared_memory->path, 1000);

    return shared_memory;

}// end create_shared_memory

int exec_img_dist(char *baseimg, char *otherimg, int pipes[2]){
    pid_t pid;
    int status;
    int ret_val = 65;

    pid = fork();
    if (pid == 0){
        if (execlp("img-dist", "-v", baseimg, otherimg, NULL) == -1)
            handle_error(pipes,baseimg,otherimg);
    }
    else
    {
        if (wait(&status) == -1)
            handle_error(pipes,baseimg,otherimg);
        if (WIFEXITED(status))
            ret_val = WEXITSTATUS(status);
    }
    return 65;
}// end exec_img_dist

void fils(int fd_pipe[2], char *baseimg, t_img_dist *shared_memory)
{
    int dist;

    while(true){
        char *otherimg = get_next_line(fd_pipe[0]);
        if (otherimg == NULL){
            handle_error(fd_pipe, baseimg, NULL);
        }
            
        // compare images
        dist = exec_img_dist(baseimg, otherimg, fd_pipe);

        // update closest image
        sem_wait(&sem_memoire_partagee);  //add at père aussi durant lecture @fin
        if (dist < shared_memory->dist){
            shared_memory->dist = dist;
            bzero(shared_memory->path, 1000);
            strcpy(shared_memory->path, otherimg);
        }
        sem_post(&sem_memoire_partagee);

        if (otherimg != NULL)
        {
            free(otherimg);
        }

    }
    close(fd_pipe[0]);
    close(fd_pipe[1]);
    exit(0);
}

void pipe_create(int pipe1[2], int pipe2[2]){
    int ret1;
    int ret2;

    ret1 = pipe(pipe1);
    ret2 = pipe(pipe2);

    if (ret1 == -1 || ret2 == -1){
        perror(strerror(errno));
        exit(1);
    }
}

pid_t start_son(int pipe[2], char *baseimg, t_img_dist  *shared_memory)
{
    pid_t   pid;

    pid = fork();
    if (pid == 0)
        fils(pipe, baseimg, shared_memory);
    else if (pid < 0)
        return ((pid_t)-1);
    return pid;
}

int main(int argc, char* argv[])
{
    t_img_dist  *shared_memory;
    char        *baseimg;
    int         pipe_fils1[2];
    int         pipe_fils2[2]; // creation pipes 
    pid_t       pid1;
    pid_t       pid2;

    if (argc != 2){
        printf("Usage: %s <path to file>\n", argv[0]);
        return 1;
    }

    start_parent_signal_handler();
    shared_memory = create_shared_memory(sizeof(t_img_dist));
    baseimg = ft_strdup(argv[1]);
    pipe_create(pipe_fils1, pipe_fils2);
    printf("pipes : %d %d %d %d\n", pipe_fils1[0], pipe_fils1[1], pipe_fils2[0], pipe_fils2[1]);

    if (((pid1 = start_son(pipe_fils1, baseimg, shared_memory)) != -1) &&
        ((pid2 = start_son(pipe_fils2, baseimg, shared_memory)) != -1))
    {
        close(pipe_fils1[0]);
        close(pipe_fils2[0]);
        close(pipe_fils1[1]);
        close(pipe_fils2[1]);
        if (pid1 > 0)
            kill(pid1, SIGTERM);
        if (pid2 > 0)
            kill(pid2, SIGTERM);
        //desalouer la memoire paratagée
        handle_error(NULL, baseimg, NULL);
    }
    else
    {
        char *input;
        bool wrote = false;

        input = get_next_line(STDIN_FILENO);
        
        while(input)
        {
            if (sigusr1)
            {
                write(pipe_fils1[1], &input, sizeof(input));
                sigusr1 = false;
                wrote = true;
            }
            else if (sigusr2)
            {
                write(pipe_fils2[1], &input, sizeof(input));
                sigusr2 = false;
                wrote = true;
            }
            if (wrote)
            {
                free(input);
                input = get_next_line(STDIN_FILENO);
                wrote = false;
            }
        }
    }

    if (pid1 == -1 || pid2 == -1)
    {
        close(pipe_fils1[0]);
        close(pipe_fils1[1]);
        close(pipe_fils2[0]);
        close(pipe_fils2[1]);
        handle_error(NULL, baseimg, NULL);
    }
    else
    {
        char *input;
        input = get_next_line(STDIN_FILENO);
        while(input)
        {
            
        }//finish later
    }

}






//         int counter =0;

//         while (true){    //à voir 256
//             char *input;
//             if (sigint == true)
//             {
//                 kill(pid1, SIGUSR1); // le code pour envoyer un signal au process enfant doit etre ici
//             }
//             input = get_next_line(STDIN_FILENO);
//             if (input == NULL){
//                 perror(strerror(errno));
//                 exit(1);
//             }
//             if(counter%2==0){
//                 write(pipe_fils1[1], &input, sizeof(input));
//             }
//             else{
//                 write(pipe_fils2[1], &input, sizeof(input));
//             }
//             free(input);
//             counter++;
//         }//end while
        
//         int status1;
//         int status2;

//         int ret_val;
//         char ret_path;

//         wait(&status1);
//         wait(&status2);
//         if (WIFEXITED(status1) && WIFEXITED(status2)){
//             if (WEXITSTATUS(status1)==0 && WEXITSTATUS(status2)==0){
//                 printf("wait ok");
//                 sem_wait(&sem_memoire_partagee);  //lire var en mémoire partagée
//                 ret_val = shared_memory->dist;    
//                 ret_path = shared_memory->path;
//                 sem_post(&sem_memoire_partagee);
//             }
//         }
//         else if (WIFSIGNALED(status1) || WIFSIGNALED(status2))
//         {
//             printf("No similar image found (no comparison could be performed successfully).");
//         }
//         else
//         {
//             perror(strerror(errno));
//         }//end wait
        


//         if (ret_val < 65){
//             printf("Most similar image found: %s with a distance of %d.", ret_path, ret_val);
//         }
//         else{
//             printf("No similar image found (no comparison could be performed successfully).");
//         }
        
//     }//end pid==0 (père)

//     else{                  //erreur creation fils 1
//         perror("fork 1");
//         exit(1);
//     }
//     return 0;
// }//end main




// /*QUESTIONS POUR ALESSANDRO :
// - les sémaphores pour les fils : le fils ne commence pas à exécuter le code tant que le pipe n'est pas plein
// - comment récupérer la distance ligne 53
// - comment récupérer le path vers l'image qui a la distance la plus courte
// - signal(sigint, handler2) chez les fils pour qu'ils l'ignorent?
// */