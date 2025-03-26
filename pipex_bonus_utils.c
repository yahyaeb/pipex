/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yel-bouk <yel-bouk@student.42nice.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/16 18:55:38 by yel-bouk          #+#    #+#             */
/*   Updated: 2025/02/21 10:01:37 by yel-bouk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex.h"

void	check_here_doc_args(t_pipex *pipex, int argc)
{
	if (argc < 6)
		ft_exit_error(pipex,
			"Usage: ./pipex here_doc LIMITER cmd1 ... outfile");
}

void	read_heredoc_input(t_pipex *pipex, char *limiter)
{
	char	*line;
	int		temp_fd;

	temp_fd = open(".heredoc_tmp", O_CREAT | O_WRONLY | O_TRUNC, 0644);
	if (temp_fd < 0)
		ft_exit_error(pipex, "here_doc temp file");
	while (1)
	{
		write(1, "> ", 2);
		line = get_next_line(STDIN_FILENO);
		if (!line)
			break ;
		if (ft_strncmp(line, limiter, ft_strlen(limiter)) == 0
			&& line[ft_strlen(limiter)] == '\n')
		{
			free(line);
			break ;
		}
		write(temp_fd, line, ft_strlen(line));
		free(line);
	}
	close(temp_fd);
}

void	setup_here_doc_fds(t_pipex *pipex, char *outfile)
{
	pipex->in_fd = open(".heredoc_tmp", O_RDONLY);
	pipex->out_fd = open(outfile, O_WRONLY | O_CREAT | O_APPEND, 0644);
	if (pipex->in_fd == -1 || pipex->out_fd == -1)
		ft_exit_error(pipex, "here_doc open");
}

void	parse_here_doc_commands(t_pipex *pipex, int argc, char **argv)
{
	int	i;

	pipex->cmd_count = argc - 4;
	pipex->cmd_args = malloc(sizeof(char **) * pipex->cmd_count);
	if (!pipex->cmd_args)
		ft_exit_error(pipex, "malloc failed");
	i = 0;
	while (i < pipex->cmd_count)
	{
		pipex->cmd_args[i] = ft_split(argv[i + 3], ' ');
		if (!pipex->cmd_args[i] || !pipex->cmd_args[i][0])
			ft_exit_error(pipex, "command not found");
		i++;
	}
}
