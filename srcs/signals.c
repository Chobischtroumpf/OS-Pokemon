#include "img_search.h"


/*
** sets the flag according to the signal
*/
void        sig_handler(int signum){
    if(signum == SIGTERM) {
        flag |= FLAG_TERM;
        // printf("SIGTERM\n");
    } else if(signum == SIGPIPE) {
        flag |= FLAG_PIPE;
        // printf("SIGPIPE\n");
    } else if(signum == SIGINT) {
        flag |= FLAG_INT;
        // printf("SIGINT\n");
    }
}


/*
** Decrements the variable showing how many images left to handle
*/
void sig_handle_usr(int signum){
    if (signum == SIGUSR1){
        to_handle--;
    }
    if (signum == SIGUSR2){
        to_handle--;
    }
    
}


/*
** sets signal handlers of:
** - all processes
** - parent
** - child
*/
void        set_sighandler(bool child)
{
    sigaction(SIGTERM, &(struct sigaction){.sa_handler = sig_handler, .sa_flags = 0}, NULL);
    sigaction(SIGPIPE, &(struct sigaction){.sa_handler = sig_handler, .sa_flags = 0}, NULL);
    if (!child) {
        sigaction(SIGINT, &(struct sigaction){.sa_handler = sig_handler, .sa_flags = 0}, NULL);
        sigaction(SIGUSR1, &(struct sigaction){.sa_handler = sig_handle_usr, .sa_flags = SA_RESTART | SA_NODEFER}, NULL);
        sigaction(SIGUSR2, &(struct sigaction){.sa_handler = sig_handle_usr, .sa_flags = SA_RESTART | SA_NODEFER}, NULL);
    } else 
        sigaction(SIGINT, &(struct sigaction){.sa_handler = SIG_IGN, .sa_flags = 0}, NULL);
}

/*
** shows error message
*/
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
