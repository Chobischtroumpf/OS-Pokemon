#pragma once
# include <stdio.h>
# include <errno.h>
# include <stdlib.h>
# include <unistd.h>
# include <string.h>
# include <signal.h>
# include <stdbool.h> 
# include <strings.h>
# include <sys/stat.h>
# include <sys/wait.h>
# include <sys/mman.h>
# include <sys/types.h>
# include <semaphore.h>

#ifndef BUFFER_SIZE
# define BUFFER_SIZE 1000
#endif

#define FLAG_TERM 0x01
#define FLAG_PIPE 0x02
#define FLAG_INT 0x04
#define ERR_PIPE "Error: got SIGPIPE"
#define ERR_TERM "Error: got SIGTERM"
#define ERR_INT "Error: got SIGINT"
#define ERR_GNL "Error: GNL :"
#define ERR_EID "Error: exec_img_dist :"
#define ERR_SSM "Error: set_shared_memory :"
#define ERR_FORK "Error: fork :"
#define ERR_WRITE "Error: write :"

typedef struct img_dist{
    int dist;
    char path[1000];
} t_img_dist;

extern int flag;
extern sem_t sem_memoire_partagee;


char    *ft_strdup(const char *src);
char    *ft_strtrim(char const *s1, char const *set);
void    handle_error(pid_t child1, pid_t child2, int read_child1, int write_child1, int read_child2, int write_child2, char *baseimg, t_img_dist *shared_mem);
int     get_next_line(int fd, char **line);

// #endif /* !UTILS_H_ */