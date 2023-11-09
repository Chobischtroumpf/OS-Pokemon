#include "img_search.h"

void        sig_handler(int signum){
    if(signum == SIGTERM) {
        flag |= FLAG_TERM;
        printf("SIGTERM\n");
    } else if(signum == SIGPIPE) {
        flag |= FLAG_PIPE;
        printf("SIGPIPE\n");
    } else if(signum == SIGINT) {
        flag |= FLAG_INT;
        printf("SIGINT\n");
    }
}

void        set_sighandler(bool child)
{
    sigaction(SIGTERM, &(struct sigaction){.sa_handler = sig_handler, .sa_flags = 0}, NULL);
    sigaction(SIGPIPE, &(struct sigaction){.sa_handler = sig_handler, .sa_flags = 0}, NULL);
    if (!child)
        sigaction(SIGINT, &(struct sigaction){.sa_handler = sig_handler, .sa_flags = 0}, NULL);
    else 
        sigaction(SIGINT, &(struct sigaction){.sa_handler = SIG_IGN, .sa_flags = 0}, NULL);
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
