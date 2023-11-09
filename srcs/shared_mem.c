#include "img_search.h"

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
    fprintf(stderr, "path : %s : dist : %d, shared_mem->dist : %d \n", path, dist, shared_mem->dist);
    if (dist < shared_mem->dist) {
        shared_mem->dist = dist;
        bzero(shared_mem->path, 1000);
        memmove(shared_mem->path, path, strlen(path));
    }
    if (sem_post(&sem_memoire_partagee) == -1)
        return (-1);
    return 0;
}