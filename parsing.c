/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yel-bouk <yel-bouk@student.42nice.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/20 07:15:03 by yel-bouk          #+#    #+#             */
/*   Updated: 2025/02/20 11:40:26 by yel-bouk         ###   ########.fr       */
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

void	filter_command(char **envp, t_pipex *pipex)
{
	int		j;
	char	**path;
	char	*path_value;
	int		count;

	j = 0;
	count = 0;
	path_value = get_path_from_env(envp);
	if (!path_value)
		return ;
	path = ft_split(path_value, ':');
	if (!path)
		return ;
	while (path[count])
		count++;
	pipex->envp = malloc(sizeof(char *) * (count + 1));
	if (!pipex->envp)
		return ;
	while (path[j])
	{
		pipex->envp[j] = ft_strjoin(path[j], "/");
		j++;
	}
	pipex->envp[j] = NULL;
	free_str_array(path);
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

void	handle_single_command(t_pipex *pipex, int argc, char *argv[])
{
	pid_t	pid;
	int		infile;
	int		outfile;

	pid = fork();
	if (pid == 0)
	{
		infile = open(argv[1], O_RDONLY);
		outfile = open(argv[argc - 1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
		process_files(infile, outfile);
		execute_command(pipex, argv[2]);
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
