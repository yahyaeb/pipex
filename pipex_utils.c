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

void	create_pipes(int pipes[], int cmd_count)
{
	int	i;

	i = 0;
	while (i < cmd_count)
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
    int infile, outfile;

    if (i == 0)
    {
        // Open infile
        infile = open(argv[1], O_RDONLY);
        if (infile == -1)
        {
            perror("Error opening infile");
            fprintf(stderr, "Failed to open infile: %s\n", strerror(errno));
            exit(EXIT_FAILURE);
        }


        // Redirect stdin to infile
        if (dup2(infile, STDIN_FILENO) == -1)
        {
            perror("dup2 infile error");
            exit(EXIT_FAILURE);
        }
        close(infile);

        // Redirect stdout to pipe
        if (dup2(pipes[1], STDOUT_FILENO) == -1)
        {
            perror("dup2 pipe error");
            exit(EXIT_FAILURE);
        }
    }
    else if (i == pipex->cmd_count - 1)
    {
        // Open outfile
        outfile = open(argv[pipex->argc - 1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (outfile == -1)
        {
            perror("Error opening outfile");
            fprintf(stderr, "Failed to open outfile: %s\n", strerror(errno));
            exit(EXIT_FAILURE);
        }

        // Redirect stdin to previous pipe
        if (dup2(pipes[(i - 1) * 2], STDIN_FILENO) == -1)
        {
            perror("dup2 infile from pipe error");
            exit(EXIT_FAILURE);
        }

        // Redirect stdout to outfile
        if (dup2(outfile, STDOUT_FILENO) == -1)
        {
            perror("dup2 outfile error");
            exit(EXIT_FAILURE);
        }
        close(outfile);
    }
    else
    {
        // Redirect stdin from previous pipe
        if (dup2(pipes[(i - 1) * 2], STDIN_FILENO) == -1)
        {
            perror("dup2 previous pipe error");
            exit(EXIT_FAILURE);
        }

        // Redirect stdout to next pipe
        if (dup2(pipes[i * 2 + 1], STDOUT_FILENO) == -1)
        {
            perror("dup2 next pipe error");
            exit(EXIT_FAILURE);
        }
    }


    // Close all pipes after redirection
    close_pipes(pipes, 2 * (pipex->cmd_count - 1));

    // Execute the command
    execute_command(pipex, argv[i + 2]);

    // If exec fails, print error
    exit(EXIT_FAILURE);
}

// void execute_child(t_pipex *pipex, int i, int pipes[], char *argv[])
// {
//     int infile, outfile;

//     if (i == 0)
//     {
//         infile = open(argv[1], O_RDONLY);
//         if (infile == -1)
//             perror("Error opening infile");
//         dup2(infile, STDIN_FILENO);
//         close(infile);
//         dup2(pipes[1], STDOUT_FILENO);
//     }
//     else if (i == pipex->cmd_count - 1)
//     {
//         outfile = open(argv[pipex->argc - 1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
//         if (outfile == -1)
//             perror("Error opening outfile");
//         dup2(pipes[(i - 1) * 2], STDIN_FILENO);
//         dup2(outfile, STDOUT_FILENO);
//         close(outfile);
//     }
//     else
//     {
//         dup2(pipes[(i - 1) * 2], STDIN_FILENO);
//         dup2(pipes[i * 2 + 1], STDOUT_FILENO);
//     }

//     // Close all pipes
//     close_pipes(pipes, 2 * (pipex->cmd_count - 1));

//     execute_command(pipex, argv[i + 2]);
// }


void fork_and_execute(t_pipex *pipex, char *argv[])
{
    pid_t *pids;
    int i;
    i = 0;
    pids = malloc(sizeof(pid_t) * pipex->cmd_count);
    if (!pids)
        exit(1);
    
    while( i < pipex->cmd_count)
    {
        pids[i] = fork();
        if (pids[i] == -1)
        {
            perror("Fork failed");
            free(pids);
            exit(1);
        }
        else if (pids[i] == 0) // Child process
            execute_child(pipex, i, pipex->pipes, argv);
        i++;
    }
    close_pipes(pipex->pipes, 2 * (pipex->cmd_count - 1));
    i = 0;
    while ( i < pipex->cmd_count)
    {
        waitpid(pids[i], NULL, 0);
        i++;
    }

    free(pids);
}



void	process_files(t_pipex *pipex)
{
	if (pipex->infile == -1)
	{
		perror("Error opening infile");
		exit(1);
	}
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
