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

void	handle_here_doc(t_pipex *pipex, int argc, char **argv)
{
	char	*limiter;

	check_here_doc_args(pipex, argc);
	pipex->here_doc = true;
	limiter = argv[2];
	read_heredoc_input(pipex, limiter);
	setup_here_doc_fds(pipex, argv[argc - 1]);
	parse_here_doc_commands(pipex, argc, argv);
	ft_parse_paths(pipex);
	execute_multiple_cmds(pipex);
	unlink(".heredoc_tmp");
	free_pipex(pipex);
}

void	handle_bonus(t_pipex *pipex, int argc, char **argv)
{
	int	i;

	i = 0;
	ft_init_pipex(pipex, argv[1], argv[argc - 1]);
	pipex->cmd_count = argc - 3;
	pipex->cmd_args = malloc(sizeof(char **) * pipex->cmd_count);
	if (!pipex->cmd_args)
		ft_exit_error(pipex, "malloc failed");
	while (i < pipex->cmd_count)
	{
		pipex->cmd_args[i] = ft_split(argv[i + 2], ' ');
		if (!pipex->cmd_args[i] || !pipex->cmd_args[i][0])
			ft_exit_error(pipex, "command not found");
		i++;
	}
	ft_parse_paths(pipex);
	execute_multiple_cmds(pipex);
	free_pipex(pipex);
}

static void	fork_and_run(t_pipex *pipex, int i, int prev_fd, int *pipefd)
{
	pid_t	pid;

	pid = fork();
	if (pid == -1)
		ft_exit_error(pipex, "fork error");
	if (pid == 0)
	{
		if (i == pipex->cmd_count - 1)
			execute_child(pipex, prev_fd, pipex->out_fd, pipex->cmd_args[i]);
		else
			execute_child(pipex, prev_fd, pipefd[1], pipex->cmd_args[i]);
	}
}

void	execute_all_children(t_pipex *pipex)
{
	int		i;
	int		pipefd[2];
	int		prev_fd;

	i = 0;
	prev_fd = pipex->in_fd;
	while (i < pipex->cmd_count)
	{
		if (i < pipex->cmd_count - 1 && pipe(pipefd) == -1)
			ft_exit_error(pipex, "pipe error");
		fork_and_run(pipex, i, prev_fd, pipefd);
		if (prev_fd != pipex->in_fd)
			close(prev_fd);
		if (i < pipex->cmd_count - 1)
		{
			close(pipefd[1]);
			prev_fd = pipefd[0];
		}
		i++;
	}
}
