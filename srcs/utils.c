#include "utils.h"

void handle_error()
{
   perror(strerror(errno));
   exit(-1);
}

char		*ft_strtrim(char const *s1, char const *set)
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

char	*ft_strdup(const char *src)
{
	char	*new;
	int		i;
	int		size;

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