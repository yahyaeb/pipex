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

// void handle_here_doc(char *limiter)
// {
//     int fd = open(".here_doc_tmp", O_WRONLY | O_CREAT | O_TRUNC, 0644);
//     char *line;

//     if (fd == -1)
//     {
//         perror("Error creating temporary here_doc file");
//         exit(1);
//     }

//     while (1)
//     {
//         write(STDOUT_FILENO, "> ", 2);
//         line = get_next_line(STDIN_FILENO);
//         if (!line)
//             break;

//         if (ft_strncmp(line, limiter, ft_strlen(limiter)) == 0 &&
//             line[ft_strlen(limiter)] == '\n')
//         {
//             free(line);
//             break;
//         }
//         write(fd, line, ft_strlen(line));
//         free(line);
//     }
//     close(fd);
// }

void free_pipex(t_pipex *pipex)
{
    if (pipex->cmd_paths)
    {
        free_str_array(pipex->cmd_paths);
        pipex->cmd_paths = NULL;
    }
    if (pipex->cmd_args)
    {
        free_str_array(pipex->cmd_args);
        pipex->cmd_args = NULL;
    }
    if (pipex->pipes)
    {
        free(pipex->pipes);
        pipex->pipes = NULL;
    }
    if (pipex->pids)
    {
        free(pipex->pids);
        pipex->pids = NULL;
    }
}

void	exit_with_cleanup(t_pipex *pipex, int status)
{
	free_pipex(pipex);
	exit(status);
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
    pipex->here_doc = 0;

    if (ft_strncmp(infile, "here_doc", 8) == 0)
    {
        pipex->here_doc = 1;

        // Prepare .here_doc_tmp from STDIN
        int fd = open(".here_doc_tmp", O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (fd == -1)
            perror("here_doc tmp file creation failed");

        char buf[1024];
        ssize_t n;
        while ((n = read(0, buf, 1024)) > 0)
        {
            // Check if limiter was entered
            if (ft_strncmp(buf, outfile, ft_strlen(outfile)) == 0)
                break;
            write(fd, buf, n);
        }
        close(fd);

        pipex->infile = open(".here_doc_tmp", O_RDONLY);
        pipex->outfile = open(outfile, O_WRONLY | O_CREAT | O_APPEND, 0644);
    }
    else
    {
        pipex->infile = open(infile, O_RDONLY);
        if (pipex->infile == -1)
        {   
            perror("Error opening infile");
        }

        pipex->outfile = open(outfile, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    }

    if (pipex->outfile == -1)
        perror("Error opening outfile");

    pipex->cmd_count = argc - 3;
    pipex->envp = envp;
    pipex->argc = argc;
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
        pipex->pipes = NULL;
        return;
    }
    create_pipes(pipex->pipes, pipex->cmd_count); // ✅ Pass correct number of pipes
    fork_and_execute(pipex, pipex->argv);
    free(pipex->pipes);
    pipex->pipes = NULL;
}


int main(int argc, char *argv[], char *envp[])
{
    if(argc < 5)
    {
        write(2, "Usage: ./pipex infile cmd1 cmd2 outfile\n", 41);
        return (1);
    }
    else
    {
        t_pipex pipex;

        pipex.argv = argv;
        
        init_pipex(&pipex, argv[1], argv[argc - 1], envp, argc);

        filter_command(&pipex);
        fork_processes(&pipex);
        free_str_array(pipex.cmd_paths);
        if (pipex.here_doc)
            unlink(".here_doc_tmp");

        return (pipex.exit_status); 

    }
};


