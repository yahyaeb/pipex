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
#include <unistd.h>

void	ft_init_pipex(t_pipex *pipex, char *infile, char *outfile)
{
	pipex->in_fd = open(infile, O_RDONLY);
	if (pipex->in_fd == -1)
		perror(infile);
	pipex->out_fd = open(outfile, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (pipex->out_fd == -1)
		perror(outfile);
	pipex->cmd_args = NULL;
	pipex->cmd_paths = NULL;
	pipex->cmd_count = 0;
	pipex->here_doc = false;
}

void	ft_execute_pipex(t_pipex *pipex)
{
	int		pipefd[2];
	pid_t	pid1;
	pid_t	pid2;
	int		status;

	if (pipe(pipefd) == -1)
		ft_exit_error(pipex, "pipe failed");
	pid1 = fork();
	if (pid1 == -1)
		ft_exit_error(pipex, "fork failed");
	if (pid1 == 0)
		execute_first_child(pipex, pipefd);
	pid2 = fork();
	if (pid2 == -1)
		ft_exit_error(pipex, "fork failed");
	if (pid2 == 0)
		execute_second_child(pipex, pipefd);
	close(pipefd[0]);
	close(pipefd[1]);
	waitpid(pid1, NULL, 0);
	waitpid(pid2, &status, 0);
	pipex->exit_status = WEXITSTATUS(status);
}

void	execute_child(t_pipex *pipex, int in_fd, int out_fd, char **cmd)
{
	char	*cmd_path;

	if (dup2(in_fd, STDIN_FILENO) == -1 || dup2(out_fd, STDOUT_FILENO) == -1)
		ft_exit_error(pipex, "dup2 error");
	cmd_path = find_cmd_path(cmd[0], pipex->cmd_paths);
	if (!cmd_path)
	{
		fprintf(stderr, "%s: command not found\n", cmd[0]);
		free_pipex(pipex);
		exit(127);
	}
	execve(cmd_path, cmd, pipex->envp);
	perror("execve");
	free(cmd_path);
	exit(1);
}

void	handle_mandatory(t_pipex *pipex, char **argv, int argc)
{
	ft_init_pipex(pipex, argv[1], argv[argc - 1]);
	ft_parse_cmds(pipex, argv);
	ft_parse_paths(pipex);
	ft_execute_pipex(pipex);
	free_pipex(pipex);
}

int	main(int argc, char **argv, char **envp)
{
	t_pipex	pipex;

	pipex = (t_pipex){0};
	pipex.envp = envp;
	if (argc >= 6 && ft_strcmp(argv[1], "here_doc") == 0)
		handle_here_doc(&pipex, argc, argv);
	else if (argc == 5)
		handle_mandatory(&pipex, argv, argc);
	else if (argc > 5)
		handle_bonus(&pipex, argc, argv);
	else
	{
		ft_printf("Usage:\n");
		ft_printf("./pipex infile cmd1 cmd2 outfile\n");
		ft_printf("./pipex here_doc LIMITER cmd1 ... outfile\n");
		return (1);
	}
	return (pipex.exit_status);
}
