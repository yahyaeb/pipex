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
        printf("Path[j] = %s\n", pipex->cmd_paths[j]);
        j++;
    }
}


char	*find_command(char *cmd, t_pipex *pipex)
{
	int		i;
	char	*full_cmd;

	i = 0;
	if (!cmd || !pipex->envp)
		return (NULL);
	while (pipex->envp[i])
	{
		full_cmd = ft_strjoin(pipex->envp[i], cmd);
		if (access(full_cmd, X_OK) == 0)
			return (full_cmd);
		free(full_cmd);
		i++;
	}
	return (NULL);
}

void	handle_single_command(t_pipex *pipex)
{
	pid_t	pid;

	pid = fork();
	if (pid == 0)
	{
		printf("I am here\n");
		process_files(pipex);
		printf("I am hereXX\n");
		execute_command(pipex, pipex->argv[2]);
	}
	waitpid(pid, NULL, 0);
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
