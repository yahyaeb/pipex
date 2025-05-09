/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yel-bouk <yel-bouk@student.42nice.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/19 16:27:43 by yel-bouk          #+#    #+#             */
/*   Updated: 2024/11/20 22:40:51 by yel-bouk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line.h"
// Helper function to join two strings and free the first
char	*strjoin_and_free(char *s1, const char *s2)
{
	size_t		len1;
	size_t		len2;
	char		*new_str;

	len1 = 0;
	len2 = 0;
	if (s1)
		len1 = ft_strlen_gnl(s1);
	if (s2)
		len2 = ft_strlen_gnl(s2);
	new_str = malloc((len1 + len2 + 1) * sizeof(char));
	if (!new_str)
		return (NULL);
	if (s1)
	{
		ft_strlcpy_gnl(new_str, s1, len1 + 1);
		free(s1);
	}
	if (s2)
		ft_strlcpy_gnl(new_str + len1, s2, len2 + 1);
	return (new_str);
}
// Function to extract a line from the buffer

char	*extract_line(char *buffer)
{
	char	*newline_pos;
	char	*line;
	int		line_length;
	int		leftover_length;

	newline_pos = ft_strchr(buffer, '\n');
	if (newline_pos)
	{
		line_length = newline_pos - buffer + 1;
		line = malloc((line_length + 1) * sizeof(char));
		if (!line)
			return (NULL);
		ft_strlcpy_gnl(line, buffer, line_length + 1);
		leftover_length = ft_strlen_gnl(buffer) - line_length;
		ft_memmove(buffer, buffer + line_length, leftover_length + 1);
	}
	else
	{
		line = ft_strdup(buffer);
		*buffer = '\0';
	}
	return (line);
}

char	*final_line(char **buffer)
{
	char	*line;

	if (**buffer)
		line = extract_line(*buffer);
	else
	{
		free(*buffer);
		*buffer = NULL;
		line = NULL;
	}
	return (line);
}

// Main function to get the next line
int	read_until_newline(int fd, char **buffer, char *temp)
{
	int		bytes_read;
	char	*newline_pos;

	newline_pos = ft_strchr(*buffer, '\n');
	while (newline_pos == NULL)
	{
		bytes_read = read(fd, temp, BUFFER_SIZE);
		if (bytes_read <= 0)
			return (bytes_read);
		temp[bytes_read] = '\0';
		*buffer = strjoin_and_free(*buffer, temp);
		if (!*buffer)
			return (-1);
		newline_pos = ft_strchr(*buffer, '\n');
	}
	return (1);
}

char	*get_next_line(int fd)
{
	static char	*buffer;
	char		*temp;
	int			status;

	if (fd < 0 || BUFFER_SIZE <= 0)
		return (NULL);
	if (!buffer)
		buffer = ft_strdup("");
	temp = malloc((BUFFER_SIZE + 1) * sizeof(char));
	if (!temp)
		return (NULL);
	status = read_until_newline(fd, &buffer, temp);
	free(temp);
	if (status == -1)
		return (NULL);
	return (final_line(&buffer));
}

/*
 int main() {
    int fd = open("test.txt", O_RDONLY);
    if (fd < 0) {
        perror("Error opening file");
        return 1;
    }

    char *line;
    while ((line = get_next_line(fd)) != NULL) {
        printf("%s", line); // Print one line at a time
        printf("\n");
        free(line);
    }

    close(fd);
    return 0;
}
*/
