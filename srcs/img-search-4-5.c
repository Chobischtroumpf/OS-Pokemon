    /*
    **
    ** Projet 1 du cours de systèmes d'exploitation informatiques
    ** Auteurs : Alessandro Dorigo, Nha Truong, Danae Veyret
    ** fichier : main.c
    **
    */

#include "utils.h"

sem_t sem_memoire_partagee;
int flag = 0;


t_img_dist  *create_shared_memory(size_t size)
{
    t_img_dist *shared_memory;

    const int protection = PROT_READ | PROT_WRITE;
    const int visibility = MAP_SHARED | MAP_ANONYMOUS;
    shared_memory = (t_img_dist*)mmap(NULL, size, protection, visibility, -1, 0);
    if ((void *)shared_memory == MAP_FAILED)
        return NULL;
    shared_memory->dist=65;
    bzero(shared_memory->path, 1000);

    return shared_memory;
}

int         set_shared_memory(t_img_dist *shared_mem, int dist, char *path)
{
    if (sem_wait(&sem_memoire_partagee) == -1)
        return (-1);
    if (dist < shared_mem->dist)
    {
        shared_mem->dist = dist;
        bzero(shared_mem->path, 1000);
        memmove(shared_mem->path, path, strlen(path));
    }
    if (sem_post(&sem_memoire_partagee) == -1)
        return (-1);
    return 0;
}

void        sig_handler(int signum)
{
    if(signum == SIGTERM)
    {
        flag |= FLAG_TERM;
        printf("SIGTERM\n");
    }
    else if(signum == SIGPIPE)
    {
        flag |= FLAG_PIPE;
        printf("SIGPIPE\n");
    }
    else if(signum == SIGINT)
    {
        flag |= FLAG_INT;
        printf("SIGINT\n");
    }
}

void        sig_err_msg(char *msg)
{
    if (msg != NULL)
        perror(msg);
    if (flag & FLAG_PIPE){
        perror(ERR_PIPE);
    } else if (flag & FLAG_TERM) {
        perror(ERR_TERM);
    } else if (flag & FLAG_INT) {
        perror(ERR_INT);
    }
}

int         exec_img_dist(char *baseimg, char *otherimg)
{
    pid_t pid;
    int status;
    int ret = 65; 

    pid = fork();
    if (pid == 0)
    {
        if (execlp("img-dist", "img-dist", baseimg, otherimg, NULL) == -1){
            perror(strerror(errno));
            return (-1);
        }
    }
    else
    {
        if (waitpid(pid, &status, 0) == -1){
            kill(pid, SIGTERM);
            waitpid(pid, NULL, 0);
            if (errno == EINTR && (flag & FLAG_TERM || flag & FLAG_PIPE || flag & FLAG_INT))
                return (-2);
            else
                return (-1);
        }
        if (WIFEXITED(status))
            ret = WEXITSTATUS(status);
        if (WIFSIGNALED(status))
            ret = 65;
    }
    return ret;
}

void        fils(char *baseimg, int pipe[2], t_img_dist *shared_mem)
{
    char *otherimg;
    int tempval;
    int ret = 0;
    bool cont = true;

    signal(SIGTERM, sig_handler);
    signal(SIGPIPE, sig_handler);
    signal(SIGINT, SIG_IGN);

    close(pipe[1]);
    while(cont)
    {
        if (flag & FLAG_TERM || flag & FLAG_PIPE || flag & FLAG_INT)
            break;
        tempval = get_next_line(pipe[0], &otherimg);    
        if (tempval == -2){
            sig_err_msg(ERR_GNL);
            ret = -2;
            break;
        } else if (tempval == -1){
            perror(ERR_GNL);
            perror(strerror(errno));
            ret = -1;
            break;
        } else if (tempval == 0){
            cont = false;
        }

        tempval = exec_img_dist(baseimg, otherimg);
        if (tempval == -2)
        {
            sig_err_msg(ERR_EID);
            ret = -2;
            break;
        } else if (tempval == -1){
            perror(ERR_EID);
            perror(strerror(errno));
            ret = -1;
            break;
        }

        if (set_shared_memory(shared_mem, tempval, otherimg) == -1)
        {
            sig_err_msg(ERR_SSM);
            ret = -1;
            break;
        }

        free(otherimg);
    }
    if (ret < 0)
        free(otherimg);
    free(baseimg);
    exit(ret);
}

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

int         loop(int pipe1[2], int pipe2[2], pid_t pid1, pid_t pid2)
{
    char *otherimg;
    bool first = true;
    bool cont = true;
    int ret;

    while(cont)
    {
        if (flag & FLAG_INT)
        {
            kill(pid1, SIGTERM);
            kill(pid2, SIGTERM);
            return (-1);
        }
        ret = get_next_line(0, &otherimg);
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
    char *baseimg;
    t_img_dist *shared_mem;

    signal(SIGTERM, sig_handler);
    signal(SIGPIPE, sig_handler);
    signal(SIGINT, sig_handler);

    if (argc != 2)
    {
        printf("Usage: %s <path to file>\n", argv[0]);
        return 1;
    }
    baseimg = ft_strdup(argv[1]);
    if (baseimg == NULL)
        return (-1);

    if (sem_init(&sem_memoire_partagee, 1, 1) == -1)
    {
        free(baseimg);
        perror(strerror(errno));
        return (-1);
    }

    shared_mem = create_shared_memory(sizeof(t_img_dist));

    if (shared_mem == NULL)
    {
        free(baseimg);
        sem_destroy(&sem_memoire_partagee);
        perror(strerror(errno));
        return (-1);
    }

    int pipes1[2];
    int pipes2[2];

    if (pipe(pipes1) == -1){
        perror(ERR_PIPE);
        handle_error(-1, -1, -1, -1, -1, -1, baseimg, shared_mem);
        return (-1);
    }
    if (pipe(pipes2) == -1){
        perror(ERR_PIPE);
        handle_error(-1, -1, pipes1[0], pipes1[1], -1, -1, baseimg, shared_mem);
        return (-1);
    }
    pid_t pid1 = fork();

    if (pid1 == 0)
    {
        close(pipes2[0]);
        close(pipes2[1]);
        fils(baseimg, pipes1, shared_mem);
    }
    else if (pid1 == -1)
    {
        perror(ERR_FORK);
        handle_error(-1, -1, pipes1[0], pipes1[1], pipes2[0], pipes2[1], baseimg, shared_mem);
        return (-1);
    }

    pid_t pid2 = fork();

    if (pid2 == 0) {
        close(pipes1[0]);
        close(pipes1[1]);
        fils(baseimg, pipes2, shared_mem);
    } else if (pid2 == -1) {
        perror(ERR_FORK);
        handle_error(pid1, -1, pipes1[0], pipes1[1], pipes2[0], pipes2[1], baseimg, shared_mem);
        return (-1);
    }

    int ret = loop(pipes1, pipes2, pid1, pid2);

    if (ret == -1){
        handle_error(pid1, pid2, pipes1[0], pipes1[1], pipes2[0], pipes2[1], baseimg, shared_mem);
        return (-1);
    } else if (ret == -2) {
        handle_error(pid1, pid2, pipes1[0], pipes1[1], pipes2[0], pipes2[1], baseimg, shared_mem);
        return (-2);
    }

    close(pipes1[0]);
    close(pipes1[1]);
    close(pipes2[0]);
    close(pipes2[1]);

    waitpid(pid1, NULL, 0);
    waitpid(pid2, NULL, 0);    

    if (shared_mem->dist == 65)
        printf("No similar image found (no comparison could be performed successfully).\n");
    else
        printf("Most similar image found: '%s' with a distance of %d.\n", shared_mem->path, shared_mem->dist);


    free(baseimg);
    munmap(shared_mem, sizeof(t_img_dist));
    sem_destroy(&sem_memoire_partagee);

    return 0;
}
