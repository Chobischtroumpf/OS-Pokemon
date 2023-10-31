#include "utils.h"

void	*resize(char **arr, size_t old_len, size_t new_len)
{
	char	*new_arr;
	size_t	i;

	new_arr = malloc(sizeof(char) *  new_len + 1);
	if (new_arr == NULL)
		return (NULL);
	i = 0;
	while (i < old_len)
	{
		new_arr[i] = (*arr)[i];
		i++;
	}
	free(*arr);
	(*arr) = new_arr;
	return (new_arr);
}

char	*get_next_line(int fd)
{
	char	ch;
	char	*buff;
	size_t	buff_size;
	size_t	chars_read;
	int		read_return;

	if (fd < 0)
		return (NULL);
	buff_size = 1000;
	buff = malloc(sizeof(char) *  buff_size + 1);
	if (buff == NULL)
		return (NULL);
	chars_read = 0;
	ch = 0;
	read_return = read(fd, &ch, 1);
	if (read_return <= 0)
	{
		free(buff);
		return (NULL);
	}
	while (read_return > 0 && ch != '\n')
	{
		buff[chars_read] = ch;
		chars_read++;
		if (chars_read >= buff_size)
		{
			resize(&buff, buff_size, buff_size * 2);
			buff_size *= 2;
		}
		read_return = read(fd, &ch, 1);
	}
	if (read_return > 0 && ch == '\n')
		buff[chars_read] = ch;
	return (buff);
}