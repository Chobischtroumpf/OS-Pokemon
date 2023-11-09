    /*
    **
    ** Projet 1 du cours de systèmes d'exploitation informatiques
    ** Auteurs : Alessandro Dorigo, Nha Truong, Danae Veyret
    ** fichier : main.c
    **
    */

#include "img_search.h"

sem_t sem_memoire_partagee;
int flag = 0;

int         send_path(char *otherimg, int pipe[2])
{
    if (write(pipe[1], otherimg, strlen(otherimg)) == -1)
    {
        perror(ERR_WRITE);
        if (errno == EINTR)
            return (-2);
        else
            return (-1);
    }
    if (write(pipe[1], "\n", 1) == -1)
    {
        perror(ERR_WRITE);
        if (errno == EINTR)
            return (-2);
        else
            return (-1);
    }
    return 0;
}

/*
** Program's main loop
** Read the path of an image from stdin and send it to the child process's pipe
** Return -1 if an error occured
** Return -2 if a signal was received
*/
int         loop(int pipe1[2], int pipe2[2])
{
    char *otherimg = NULL;
    bool first;
    bool cont;
    int ret; 

    set_sighandler(false);

    first = true;
    cont = true;

    while(cont)
    {
        if (flag & FLAG_INT || flag & FLAG_TERM || flag & FLAG_PIPE) {
            fprintf(stderr, "Signal received, exiting...\n");
            return (-2);
        }
        ret = get_next_line(0, &otherimg);
        printf("otherimg: %s\n", otherimg);
        if (ret == -2) {
            perror(ERR_GNL);
            free(otherimg);
            return (-2);
        } else if (ret == -1) {
            perror(ERR_GNL);
            free(otherimg);
            return (-1);
        } else if (ret == 0) {
            cont = false;
        }

        if (first) {
            ret = send_path(otherimg, pipe1);
            first = false;
        } else {
            ret = send_path(otherimg, pipe2);
            first = true;
        }

        free(otherimg);

        if (ret < 0)
            return (ret);
    }
    return 0;
}

int         main(int argc, char* argv[])
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
    pid2 = create_child(pipe1, pipe2, baseimg, shared_mem);
    if (pid2 == -1) {
        perror(ERR_FORK);
        handle_error(pid1, -1, pipe1[0], pipe1[1], pipe2[0], pipe2[1], baseimg, shared_mem);
        return (-1);
    }

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

    close(pipe1[0]);
    close(pipe1[1]);
    close(pipe2[0]);
    close(pipe2[1]);

    if (waitpid(pid1, NULL, 0) == -1){
        handle_error(pid1, pid2, -1, -1, -1, -1, baseimg, shared_mem);
        return -1;
    }
    if (waitpid(pid2, NULL, 0) == -1) {
        handle_error(-1, pid2, -1, -1, -1, -1, baseimg, shared_mem);
        return -1;
    }

    if (shared_mem->dist == 65)
        printf("No similar image found (no comparison could be performed successfully).\n");
    else
        printf("Most similar image found: '%s' with a distance of %d.\n", shared_mem->path, shared_mem->dist);


    free(baseimg);
    munmap(shared_mem, sizeof(t_img_dist));
    sem_destroy(&sem_memoire_partagee);

    return 0;
}
