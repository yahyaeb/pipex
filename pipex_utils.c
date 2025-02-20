/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yel-bouk <yel-bouk@student.42nice.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/20 07:03:44 by yel-bouk          #+#    #+#             */
/*   Updated: 2025/02/20 11:36:00 by yel-bouk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex.h"

void	create_pipes(int pipes[], int cmd_count)
{
	int	i;

	i = 0;
	while (i < cmd_count - 1)
	{
		if (pipe(pipes + (i * 2)) == -1)
		{
			perror("Pipe creation failed");
			exit(1);
		}
		i++;
	}
}

void	close_pipes(int pipes[], int pipe_count)
{
	int	i;

	i = 0;
	while (i < pipe_count)
		close(pipes[i++]);
}

void	execute_child(t_pipex *pipex, int i, int pipes[], int cmd_count, char *argv[], int argc)
{
	int	infile;

	if (i == 0)
	{
		infile = open(argv[1], O_RDONLY);
		dup2(infile, STDIN_FILENO);
		close(infile);
		dup2(pipes[1], STDOUT_FILENO);
	}
	else if (i == cmd_count - 1)
	{
		int outfile = open(argv[argc - 1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
		dup2(pipes[(i - 1) * 2], STDIN_FILENO);
		dup2(outfile, STDOUT_FILENO);
		close(outfile);
	}
	else
	{
		dup2(pipes[(i - 1) * 2], STDIN_FILENO);
		dup2(pipes[i * 2 + 1], STDOUT_FILENO);
	}

	close_pipes(pipes, 2 * (cmd_count - 1));
	execute_command(pipex, argv[i + 2]);
}

void	fork_and_execute(t_pipex *pipex, int cmd_count, char *argv[], int pipes[], int argc)
{
	pid_t	pids[cmd_count];
	int		i;
 
	i = 0;
	while (i < cmd_count)
	{
		pids[i] = fork();
		if (pids[i] == 0)
			execute_child(pipex, i, pipes, cmd_count, argv, argc);
		i++;
	}
	close_pipes(pipes, 2 * (cmd_count - 1));
	i = 0;
	while (i < cmd_count)
		waitpid(pids[i++], NULL, 0);
}

void	process_files(int infile, int outfile)
{
	if (infile == -1)
	{
		perror("Error opening infile");
		exit(1);
	}
	dup2(infile, STDIN_FILENO);
	close(infile);
	if (outfile == -1)
	{
		perror("Error opening outfile");
		exit(1);
	}
	dup2(outfile, STDOUT_FILENO);
	close(outfile);
}
