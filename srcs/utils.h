#ifndef UTILS_H_
# define UTILS_H_
# include <stdio.h>
# include <errno.h>
# include <stdlib.h>
# include <unistd.h>
# include <string.h>
# include <strings.h>
# include <sys/stat.h>
# include <sys/wait.h>
# include <sys/mman.h>
# include <sys/types.h>
# include <semaphore.h>

# define true 1

void    handle_error(int pipe[2], FILE *fstream, char *baseimg, char *otherimg);
char	*ft_strdup(const char *src);
char	*strjoin(char const *s1, char const *s2);
char		*ft_strtrim(char const *s1, char const *set);

#endif /* !UTILS_H_ */