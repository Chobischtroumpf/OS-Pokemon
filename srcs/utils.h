#ifndef UTILS_H_
# define UTILS_H_
# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>
# include <string.h>
# include <strings.h>
# include <sys/stat.h>
# include <sys/types.h>
# include <sys/wait.h>
# include <errno.h>
# define _GNU_SOURCE

# define true 1

void    handle_error();
char	*ft_strdup(const char *src);
char	*strjoin(char const *s1, char const *s2);
char		*ft_strtrim(char const *s1, char const *set);

#endif /* !UTILS_H_ */