#include "img_search.h"

void    handle_error(pid_t child1, pid_t child2, int read_child1, int write_child1, int read_child2, int write_child2, char *baseimg, t_img_dist *shared_mem)
{
    if (child1 != -1)
        kill(child1, SIGTERM);
    if (child2 != -1)
        kill(child2, SIGTERM);
    if (child1 != -1)
        waitpid(child1, NULL, 0);
    if (child2 != -1)
        waitpid(child2, NULL, 0);

    if (read_child1 != -1)
        close(read_child1);
    if (write_child1 != -1)
        close(write_child1);

    if (read_child2 != -1)
        close(read_child2);
    if (write_child2 != -1)
        close(write_child2);
    if (baseimg != NULL)
        free(baseimg);
    if (shared_mem != NULL)
        munmap(shared_mem, sizeof(t_img_dist));

    sem_destroy(&sem_memoire_partagee);
    perror(strerror(errno));
}
