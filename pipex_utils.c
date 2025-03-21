/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yel-bouk <yel-bouk@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/20 07:03:44 by yel-bouk          #+#    #+#             */
/*   Updated: 2025/03/18 14:57:45 by yel-bouk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex.h"

void create_pipes(int *pipes, int cmd_count)
{
    int i = 0;
    
    if (!pipes)  // ✅ Ensure pipes[] is allocated before using it!
    {
        fprintf(stderr, "Error: pipes array is NULL!\n");
        exit(1);
    }

    while (i < cmd_count - 1)  // ✅ cmd_count - 1, because you need (cmd_count - 1) pipes
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
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

void execute_child(t_pipex *pipex, int i, int pipes[], char *argv[])
{


    if (i == 0)
    {
        dup2(pipex->infile, STDIN_FILENO);
        dup2(pipes[1], STDOUT_FILENO);
    }
    else if (i == pipex->cmd_count - 1)
    {
        pipex->outfile = open(argv[pipex->argc - 1], O_WRONLY | O_CREAT | O_APPEND, 0644);
        if (pipex->outfile == -1)
        {
            perror("Error opening outfile");
            free_str_array(pipex->cmd_paths);
            // free_str_array(pipex->cmd_args);
            free(pipex->pids);
            free(pipex->pipes);
            exit(1);
        }

        dup2(pipes[(i - 1) * 2], STDIN_FILENO);
        dup2(pipex->outfile, STDOUT_FILENO);
        close(pipex->outfile);
    }
    else
    {
        dup2(pipes[(i - 1) * 2], STDIN_FILENO);
        dup2(pipes[i * 2 + 1], STDOUT_FILENO);
    }

    close_pipes(pipes, 2 * (pipex->cmd_count - 1));
    execute_command(pipex, argv[i + 2]);
    exit(EXIT_FAILURE);
}


void fork_and_execute(t_pipex *pipex, char *argv[])
{
    int i = 0;
    int status;

    pipex->pids = malloc(sizeof(pid_t) * pipex->cmd_count);
    if (!pipex->pids)
        exit(1);

    // Fork all processes
    while (i < pipex->cmd_count)
    {
        pipex->pids[i] = fork();
        if (pipex->pids[i] == -1)
        {
            perror("Fork failed");
            free(pipex->pids);
            exit(1);
        }
        else if (pipex->pids[i] == 0)
        {
            // Child process
            execute_child(pipex, i, pipex->pipes, argv);
        }
        i++;
    }

    // Close all pipe ends in parent
    close_pipes(pipex->pipes, 2 * (pipex->cmd_count - 1));

    // Wait for all children and track last exit status
    i = 0;
    while (i < pipex->cmd_count)
    {
        waitpid(pipex->pids[i], &status, 0);

        // Save exit status of the last command only
        if (i == pipex->cmd_count - 1)
            pipex->exit_status = status >> 8;

        i++;
    }

    free(pipex->pids);
}




void	process_files(t_pipex *pipex)
{
	// if (pipex->infile == -1)
	// {
	// 	perror("Error opening infile");
	// 	exit(1);
	// }
	dup2(pipex->infile, STDIN_FILENO);
	close(pipex->infile);
	if (pipex->outfile == -1)
	{
		perror("Error opening outfile");
		exit(1);
	}
    if (fcntl(pipex->outfile, F_GETFD) == -1)
        perror("outfile FD is already closed");
if (dup2(pipex->outfile, STDOUT_FILENO) == -1)
    {
        perror("dup2 outfile failed");
        exit(1);
    }
	close(pipex->outfile);
}
