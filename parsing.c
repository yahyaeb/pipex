/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yel-bouk <yel-bouk@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/20 07:15:03 by yel-bouk          #+#    #+#             */
/*   Updated: 2025/03/18 14:59:46 by yel-bouk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex.h"

char	*get_path_from_env(char *envp[])
{
	int	i;

	i = 0;
	while (envp[i])
	{
		if (ft_strncmp(envp[i], "PATH=", 5) == 0)
			return (envp[i] + 5);
		i++;
	}
	return (NULL);
}

void filter_command(t_pipex *pipex)
{
    int j = 0;
    char *path_value;

    path_value = get_path_from_env(pipex->envp);
    if (!path_value)
    {
        perror("Error: PATH variable not found");
        exit(1);
	}
    pipex->cmd_paths = ft_split(path_value, ':');
    if (!pipex->cmd_paths)
    {
        perror("Error: Failed to split PATH");
        exit(1);
    }
    while (pipex->cmd_paths[j])
    {
        char *temp = ft_strjoin(pipex->cmd_paths[j], "/");
        free(pipex->cmd_paths[j]); 
        pipex->cmd_paths[j] = temp;
        j++;
    }
}


char *find_command(char *cmd, t_pipex *pipex)
{
    int i;
    char *full_cmd;

    i = 0;
    if (!cmd || !pipex->cmd_paths) // ✅ Check correct variable
        return (NULL);

    while (pipex->cmd_paths[i])
    {
        full_cmd = ft_strjoin(pipex->cmd_paths[i], cmd); // ✅ Use cmd_paths, not envp
        if (access(full_cmd, X_OK) == 0) // ✅ Check if command is executable
            return (full_cmd);

        free(full_cmd); // ✅ Prevent memory leak
        i++;
    }
    return (NULL); // ❌ Command not found
}


void handle_single_command(t_pipex *pipex, char *cmd)
{
    pid_t pid = fork();

    if (pid < 0)
    {
        perror("Error: fork() failed");
        exit(1);
    }

    if (pid == 0) // Child process
    {
        printf("Command received: %s\n", cmd);
        fflush(stdout);

        if (!cmd)
        {
            perror("Error: Command is NULL");
            exit(1);
        }

            
        pipex->cmd_args = ft_split(cmd, ' ');
        if (!pipex->cmd_args || !pipex->cmd_args[0])
        {
            perror("Error: Failed to parse command arguments");
            exit(1);
        }

        fflush(stdout);
        pipex->valid_cmd = find_command(pipex->cmd_args[0], pipex);
        if (!pipex->valid_cmd)
        {
            perror("Error: Command not found or not executable");
            exit(127);
        }

        fflush(stdout);

	if (pipex->infile == -1)
	{
		perror("Error: infile FD is invalid before dup2");
		exit(1);
	}
	if (pipex->outfile == -1)
	{
		perror("Error: outfile FD is invalid before dup2");
		exit(1);
	}


	if (dup2(pipex->infile, STDIN_FILENO) == -1)
	{
		perror("dup2 infile failed");
		exit(1);
	}

	if (dup2(pipex->outfile, STDOUT_FILENO) == -1)
	{
		perror("dup2 outfile failed");
		exit(1);
	}
        fflush(stdout);

        close(pipex->infile);
        close(pipex->outfile);

        execve(pipex->valid_cmd, pipex->cmd_args, pipex->envp);

        perror("execve failed");
        exit(1);
    }
    
    wait(NULL);
}

void	free_str_array(char **array)
{
	int	i;

	i = 0;
	if (!array)
		return ;
	while (array[i])
	{
		free(array[i]);
		i++;
	}
	free(array);
}
