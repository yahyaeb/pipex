/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yel-bouk <yel-bouk@student.42nice.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/16 18:54:10 by yel-bouk          #+#    #+#             */
/*   Updated: 2025/02/20 09:07:43 by yel-bouk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_libft/libft/libft.h"
#include "ft_libft/ft_printf/includes/ft_printf.h"
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>
#include <sys/wait.h>

typedef struct s_pipex
{
	int		infile;
	int		outfile;
	int		pipe_fds[2];
	char	**cmd_paths;
	char	**cmd_args;
	char	**envp;
	char	*valid_cmd;
}	t_pipex;

void	create_pipes(int pipes[], int cmd_count);
void	close_pipes(int pipes[], int pipe_count);
void	execute_child(t_pipex *pipex, int i, int pipes[],
			int cmd_count, char *argv[], int argc);
void	fork_and_execute(t_pipex *pipex, int cmd_count,
			char *argv[], int pipes[], int argc);
void	process_files(int infile, int outfile);
int		update_count(char **path, int count);
void	init_pipex(t_pipex *pipex, char *infile, char *outfile, char **envp);
char	*get_path_from_env(char *envp[]);
void	filter_command(char **envp, t_pipex *pipex);
char	*find_command(char *cmd, t_pipex *pipex);
void	execute_command(t_pipex *pipex, char *cmd);
void	handle_single_command(t_pipex *pipex, int argc, char *argv[]);
void	fork_processes(t_pipex *pipex, int argc, char *argv[]);
void	free_str_array(char **array);