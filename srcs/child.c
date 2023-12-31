#include "img_search.h"


/* 
** creates another process to execute img-dist to compare images
** waits for the end of the execution of its child
** returns the value returned by img-dist
** grandchild returns -1 if an error occured 
** retuns -2 if an interruption occurs
** returns -1 if an error occured in waitpid()
** returns 65 if another error occurs
*/

int     exec_img_dist(char *baseimg, char *otherimg)
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


/*
** main function of child process
** in a loop, gets the next path through the pipe and runs exec_img_dist()  
** interracts with shared memory 
** return -2 if an interruption occured in get_next_line() or exec_img_dist()
** returns -1 in case of any other error 
** decrements images left to handle
*/

void    fils(char *baseimg, int pipe, t_img_dist *shared_mem, bool first)
{
    char *otherimg;
    int tempval;
    int ret = 0;
    bool cont = true;

    set_sighandler(true);

    while(cont)
    {
        if (flag & FLAG_TERM || flag & FLAG_PIPE || flag & FLAG_INT)
            break;

        tempval = get_next_line(pipe, &otherimg);
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
            break;
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
        } else {
            if (first)
                kill(getppid(), SIGUSR1);
            else
                kill(getppid(), SIGUSR2);
        }

        if (set_shared_memory(shared_mem, tempval, otherimg) == -1) {
            sig_err_msg(ERR_SSM);
            ret = -1;
            break;
        }

        free(otherimg);
    }
    free(otherimg);
    free(baseimg);
    close(pipe);
    exit(ret);
    // return ;
}


/*
** Create a child process and close the unused pipe ends
** Return the pid of the child process
** Return -1 if an error occured
** Does not return in the child process
*/
pid_t   create_child(int *pipe1, int *pipe2, char *baseimg, t_img_dist *shared_mem, bool first)
{
    pid_t pid;
    
    pid = fork();
    if (pid == 0) {
        close(pipe2[0]);
        close(pipe2[1]);
        close(pipe1[1]);
        fils(baseimg, pipe1[0], shared_mem, first);
    } else if (pid == -1) {
        return ((pid_t)-1);
    }
    return pid;
}
