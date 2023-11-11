#include "img_search.h"


/*
** templated function to create the shared memory
** will contain an int and a path
** sets the necessary flags and intialises the two variables
** returns the pointer
** returns NULL if mmap fails
*/
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

/*
** changes the values saved in the shared memory
** semaphore guarantees only one process at the time has access
** if the new distance is smaller than the one saved, changes it and saves the path
** returns 0
** returns -1 if error 
*/
int         set_shared_memory(t_img_dist *shared_mem, int dist, char *path)
{
    if (sem_wait(&sem_memoire_partagee) == -1)
        return (-1);
    if (dist < shared_mem->dist) {
        shared_mem->dist = dist;
        bzero(shared_mem->path, 1000);
        memmove(shared_mem->path, path, strlen(path));
    }
    if (sem_post(&sem_memoire_partagee) == -1)
        return (-1);
    return 0;
}
