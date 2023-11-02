#include "utils.h"

void sig_handler_child(int signum)
{
    if (signum == SIGTERM)
        sigterm = true;
}

// void sig_handler_child_2(int signum)
// {
//     if (signum == SIGTERM)
//         sigterm = true;
//     else if (signum == SIGUSR2)
//         sigusr2 = true; 
// }

void sig_handler_parent(int signum)
{
    if (signum == SIGINT)
        sigint = true;
    if (signum == SIGTERM)
        sigterm = true;
    if (signum == SIGUSR1)
        sigusr1 = true;
    if (signum == SIGUSR2)
        sigusr2 = true;
    if (signum == SIGPIPE)
        sigpipe = true;
}
