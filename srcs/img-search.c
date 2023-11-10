    /*
    **
    ** Projet 1 du cours de systèmes d'exploitation informatiques
    ** Auteurs : Alessandro Dorigo, Nha Truong, Danae Veyret
    ** fichier : main.c
    **
    */

#include "img_search.h"
#include <time.h>

sem_t sem_memoire_partagee;
short flag = 0;
int to_handle = 0;

int send_path(char *otherimg, int pipe[2])
{
    if (write(pipe[1], otherimg, strlen(otherimg)) == -1){
        perror(ERR_WRITE);
        if (errno==EINTR && flag)
            return -2;
        return -1;
    }
    if (write(pipe[1], "\n", 1) == -1){
        perror(ERR_WRITE);
        if (errno==EINTR && flag)
            return -2;
        return -1;
    }
    // fprintf(stderr, "data being sent to child: %s", otherimg);
    return 0;
}

/*
** Program's main loop
** Read the path of an image from stdin and send it to the child process's pipe
** Return -1 if an error occured
** Return -2 if a signal was received
*/
int loop(int pipe1[2], int pipe2[2])
{
    char *otherimg = NULL;
    // bool first;
    bool cont;
    int ret; 

    set_sighandler(false);

    // first = true;
    cont = true;

    while(cont)
    {
        if (flag & FLAG_INT || flag & FLAG_TERM || flag & FLAG_PIPE) {
            fprintf(stderr, "Signal received, exiting...\n");
            return (-2);
        }
        ret = get_next_line(0, &otherimg);
        // printf("otherimg: %s\n", otherimg);
        if (ret == -1 && errno == EINTR) {
            perror(ERR_GNL);
        } else if (ret == -1) {
            perror(ERR_GNL);
            free(otherimg);
            return (-1);
        } else if (ret == 0) {
            cont = false;
        }
        to_handle++;
        if (first) {
            // fprintf(stderr, "otherimg to child1 : %s\n", otherimg);
            ret = send_path(otherimg, pipe1);
            first = false;
        } else {
            // fprintf(stderr, "otherimg to child2 : %s \n", otherimg);
            ret = send_path(otherimg, pipe2);
            first = true;
        }

        free(otherimg);

        if (ret < 0)
            return (ret);
    }
    return 0;
}

int main(int argc, char* argv[])
{
    t_img_dist *shared_mem;
    char *baseimg;
    pid_t pid1;
    pid_t pid2;
    int pipe1[2];
    int pipe2[2];
    int loop_ret;

    if (argc != 2){
        printf("Usage: %s <path to file>\n", argv[0]);
        return 1;
    }

    baseimg = strdup(argv[1]);
    if (baseimg == NULL)
        return (-1);

    if (sem_init(&sem_memoire_partagee, 1, 1) == -1){
        free(baseimg);
        perror(strerror(errno));
        return (-1);
    }

    shared_mem = create_shared_memory(sizeof(t_img_dist));
    if (shared_mem == NULL){
        free(baseimg);
        sem_destroy(&sem_memoire_partagee);
        perror(strerror(errno));
        return (-1);
    }

    if (pipe(pipe1) == -1){
        perror(ERR_PIPE);
        handle_error(-1, -1, -1, -1, -1, -1, baseimg, shared_mem);
        return (-1);
    }
    if (pipe(pipe2) == -1){
        perror(ERR_PIPE);
        handle_error(-1, -1, pipe1[0], pipe1[1], -1, -1, baseimg, shared_mem);
        return (-1);
    }

    pid1 = create_child(pipe1, pipe2, baseimg, shared_mem);
    if (pid1 == -1)
    {
        perror(ERR_FORK);
        handle_error(-1, -1, pipe1[0], pipe1[1], pipe2[0], pipe2[1], baseimg, shared_mem);
        return (-1);
    }
    first=false;
    pid2 = create_child(pipe2, pipe1, baseimg, shared_mem);
    if (pid2 == -1) {
        perror(ERR_FORK);
        handle_error(pid1, -1, pipe1[0], pipe1[1], pipe2[0], pipe2[1], baseimg, shared_mem);
        return (-1);
    }

    close(pipe1[0]);
    close(pipe2[0]);

    loop_ret = loop(pipe1, pipe2);

    if (loop_ret == -1){
        perror("Error while reading from stdin.");
        handle_error(pid1, pid2, pipe1[0], pipe1[1], pipe2[0], pipe2[1], baseimg, shared_mem);
        return (-1);
    } else if (loop_ret == -2) {
        perror("Signal received while reading from stdin.\n");
        handle_error(pid1, pid2, pipe1[0], pipe1[1], pipe2[0], pipe2[1], baseimg, shared_mem);
        return (-2);
    }

    time_t t = time(NULL);
    fprintf(stderr, "before waitpid's %ld \n", t);
    
    while(to_handle > 0) {
        pause();
        if (flag & FLAG_INT || flag & FLAG_TERM || flag & FLAG_PIPE) {
            fprintf(stderr, "Signal received, exiting...\n");
            handle_error(pid1, pid2, pipe1[0], pipe1[1], pipe2[0], pipe2[1], baseimg, shared_mem);
            return (-2);
        }
        // printf("to_handle: %d\n", to_handle);
    }

    close(pipe1[1]);
    close(pipe2[1]);

    int status;
    waitpid(pid1, &status, 0);
    waitpid(pid2, &status, 0);
    
    t = time(NULL);
    fprintf(stderr, "after waitpid's %ld\n", t);

    if (shared_mem->dist == 65)
        printf("No similar image found (no comparison could be performed successfully).\n");
    else
        printf("Most similar image found: '%s' with a distance of %d.\n", shared_mem->path, shared_mem->dist);


    free(baseimg);
    munmap(shared_mem, sizeof(t_img_dist));
    sem_destroy(&sem_memoire_partagee);

    return 0;
}
