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

void	ft_exit_error(t_pipex *pipex, const char *msg)
{
	perror(msg);
	free_pipex(pipex);
	exit(EXIT_FAILURE);
}

void	ft_free_array(char **arr, int count)
{
	int	i;

	i = 0;
	if (!arr)
		return ;
	if (count == -1)
	{
		while (arr[i])
			free(arr[i++]);
	}
	else
	{
		while (i < count)
			free(arr[i++]);
	}
	free(arr);
}

void	ft_free_2d_array(char ***arr, int count)
{
	int	i;

	i = 0;
	if (!arr)
		return ;
	while (i < count)
	{
		if (arr[i])
			ft_free_array(arr[i], -1);
		i++;
	}
	free(arr);
}

void	free_pipex(t_pipex *pipex)
{
	if (pipex->in_fd != -1)
		close(pipex->in_fd);
	if (pipex->out_fd != -1)
		close(pipex->out_fd);
	if (pipex->cmd_paths)
		ft_free_array(pipex->cmd_paths, -1);
	if (pipex->cmd_args)
		ft_free_2d_array(pipex->cmd_args, pipex->cmd_count);
}
