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

void	wait_for_all_children(t_pipex *pipex, int count)
{
	int	status;

	while (count-- > 0)
		wait(&status);
	if (WIFEXITED(status))
		pipex->exit_status = WEXITSTATUS(status);
	else
		pipex->exit_status = 1;
}

void	execute_multiple_cmds(t_pipex *pipex)
{
	execute_all_children(pipex);
	wait_for_all_children(pipex, pipex->cmd_count);
}

void	execute_first_child(t_pipex *pipex, int *pipefd)
{
	char	*cmd_path;

	cmd_path = find_cmd_path(pipex->cmd_args[0][0], pipex->cmd_paths);
	if (!cmd_path)
	{
		ft_printf("%s: command not found\n", pipex->cmd_args[0][0]);
		free_pipex(pipex);
		exit(127);
	}
	if (pipex->in_fd == -1 || dup2(pipex->in_fd, STDIN_FILENO) == -1
		|| dup2(pipefd[1], STDOUT_FILENO) == -1)
	{
		free(cmd_path);
		free_pipex(pipex);
		perror("pipex: input redirection");
		exit(1);
	}
	close(pipefd[0]);
	close(pipefd[1]);
	execve(cmd_path, pipex->cmd_args[0], pipex->envp);
	perror("execve");
	free(cmd_path);
	exit(1);
}

void	execute_second_child(t_pipex *pipex, int *pipefd)
{
	char	*cmd_path;

	cmd_path = find_cmd_path(pipex->cmd_args[1][0], pipex->cmd_paths);
	if (!cmd_path)
	{
		ft_printf("%s: command not found\n", pipex->cmd_args[1][0]);
		free_pipex(pipex);
		exit(127);
	}
	if (dup2(pipefd[0], STDIN_FILENO) == -1 || pipex->out_fd == -1
		|| dup2(pipex->out_fd, STDOUT_FILENO) == -1)
	{
		free(cmd_path);
		free_pipex(pipex);
		perror("pipex: output redirection");
		exit(1);
	}
	close(pipefd[1]);
	close(pipefd[0]);
	execve(cmd_path, pipex->cmd_args[1], pipex->envp);
	perror("execve");
	free(cmd_path);
	exit(1);
}
