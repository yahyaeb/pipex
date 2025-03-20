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

void handle_here_doc(char *limiter)
{
    int fd = open(".here_doc_tmp", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    char *line;

    if (fd == -1)
    {
        perror("Error creating temporary here_doc file");
        exit(1);
    }

    while (1)
    {
        write(STDOUT_FILENO, "> ", 2);  // Prompt
        line = get_next_line(STDIN_FILENO);
        if (!line || ft_strcmp(line, limiter) == 0)
            break;

        write(fd, line, ft_strlen(line));
        free(line);
    }
    close(fd);
}

int	update_count(char **path, int count)
{
	count = 0;
	while (path[count])
		count++;
	return (count);
}

void init_pipex(t_pipex *pipex, char *infile, char *outfile, char **envp, int argc)
{
    if (ft_strcmp(pipex->argv[1], "here_doc") == 0)
    {
        pipex->here_doc = 1;  // Mark as here_doc mode
        handle_here_doc(pipex->argv[2]);  // Read input from stdin
    }

    pipex->infile = open(infile, O_RDONLY);
    if (pipex->infile == -1)
    {
        perror("Error opening infile");
        exit(0);
    }

    pipex->outfile = open(outfile, O_WRONLY | O_CREAT | O_APPEND, 0644);
    if (pipex->outfile == -1)
    {
        perror("Error opening outfile");
        exit(0);
    }

    pipex->cmd_count = argc - 3;
    pipex->envp = envp;
    pipex->argc = argc;  // ✅ Store argc for later use
}



void execute_command(t_pipex *pipex, char *cmd)
{
    pipex->cmd_args = ft_split(cmd, ' ');
    pipex->valid_cmd = find_command(pipex->cmd_args[0], pipex);

    if (!pipex->valid_cmd)
    {
        perror("Command not found");
        free_str_array(pipex->cmd_paths);
        free_str_array(pipex->cmd_args);
        free(pipex->pids);
        free(pipex->pipes);
        exit(127);
    }
    execve(pipex->valid_cmd, pipex->cmd_args, pipex->envp);
    
    perror("Execve failed");
    exit(1);
}

void fork_processes(t_pipex *pipex)
{

    pipex->pipes = malloc(sizeof(int) * 2 * (pipex->cmd_count - 1));
    if (!pipex->pipes)
    {
        perror("malloc failed");
        exit(1);
    }

    if (pipex->cmd_count == 1)
    {
        handle_single_command(pipex, pipex->argv[2]); // ✅ Pass correct command
        free(pipex->pipes);
        return;
    }
    create_pipes(pipex->pipes, pipex->cmd_count); // ✅ Pass correct number of pipes
    fork_and_execute(pipex, pipex->argv);
    free(pipex->pipes);
}

int main(int argc, char *argv[], char *envp[])
{
    if(argc < 4)
        return 0;
    t_pipex pipex;



    init_pipex(&pipex, argv[1], argv[argc - 1], envp, argc);
    pipex.argv = argv;
    filter_command(&pipex);
    fork_processes(&pipex);
    free_str_array(pipex.cmd_paths);
    return (0);
}

