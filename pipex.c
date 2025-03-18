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

int	update_count(char **path, int count)
{
	count = 0;
	while (path[count])
		count++;
	return (count);
}

void init_pipex(t_pipex *pipex, char *infile, char *outfile, char **envp, int argc)
{
    pipex->infile = open(infile, O_RDONLY);
    if (pipex->infile == -1)
        perror("Error opening infile");

    pipex->outfile = open(outfile, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (pipex->outfile == -1)
        perror("Error opening outfile");

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
        exit(127);
    }

    execve(pipex->valid_cmd, pipex->cmd_args, pipex->envp);
    
    perror("Execve failed");
    exit(1);
}


void fork_processes(t_pipex *pipex)
{
    int *pipes;

    printf("cmd->count %d\n", pipex->cmd_count);

    pipes = malloc(sizeof(int) * 2 * (pipex->cmd_count - 1));
    if (!pipes)
    {
        perror("malloc failed");
        exit(1);
    }

    pipex->pipes = pipes; // ✅ Store pipes in pipex to avoid memory issues

    if (pipex->cmd_count == 1)
    {
        handle_single_command(pipex, pipex->argv[2]); // ✅ Pass correct command
        free(pipes);
        return;
    }

    create_pipes(pipes, pipex->cmd_count - 1); // ✅ Pass correct number of pipes
    fork_and_execute(pipex, pipex->argv);
    free(pipes);
}


int main(int argc, char *argv[], char *envp[])
{
    t_pipex pipex;



    init_pipex(&pipex, argv[1], argv[argc - 1], envp, argc);
    pipex.argv = argv;
    filter_command(&pipex);
    fork_processes(&pipex);
    free_str_array(pipex.envp);

    return (0);
}

