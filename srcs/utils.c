#include "utils.h"

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

char *ft_strtrim(char const *s1, char const *set)
{
    size_t	beg;
    size_t	end;
    char	*new;

    if (!s1 || !set)
        return (NULL);
    beg = 0;
    while (s1[beg] && strchr(set, s1[beg]))
        beg++;
    end = strlen(s1 + beg);
    if (end)
        while (s1[end + beg - 1] != 0 && strchr(set, s1[end + beg - 1]) != 0)
            end--;
    if (!(new = malloc(sizeof(char) * end + 1)))
        return (NULL);
    strncpy(new, s1 + beg, end);
    new[end] = '\0';
    return (new);
}

char *ft_strdup(const char *src)
{
    char    *new;
    int     i;
    int     size;

    size = 0;
    if (!src)
        return (NULL);
    while (src[size])
        ++size;
    if (!(new = malloc(sizeof(char) * (size + 1))))
    {
        return (NULL);
    }
    i = 0;
    while (src[i])
    {
        new[i] = src[i];
        i++;
    }
    new[i] = '\0';
    return (new);
}
