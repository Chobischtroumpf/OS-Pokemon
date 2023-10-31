#ifndef UTILS_H_
# define UTILS_H_

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
// # define MAP_ANONYMOUS 0x20 


void    handle_error(int pipe[2], char *baseimg, char *otherimg);
char    *ft_strdup(const char *src);
char    *strjoin(char const *s1, char const *s2);
char    *ft_strtrim(char const *s1, char const *set);
char	*get_next_line(int fd);

#endif /* !UTILS_H_ */