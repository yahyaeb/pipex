/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yel-bouk <yel-bouk@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/16 18:54:10 by yel-bouk          #+#    #+#             */
/*   Updated: 2025/03/18 14:56:17 by yel-bouk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


// pipex.h
#ifndef PIPEX_H
# define PIPEX_H

# include <unistd.h>
# include <stdlib.h>
# include <fcntl.h>
# include <sys/wait.h>
# include <errno.h>
# include "ft_libft/libft/libft.h"
# include "ft_libft/ft_printf/includes/ft_printf.h"

typedef enum e_bool { false, true } t_bool;

typedef struct s_pipex {
    int in_fd;
    int out_fd;
    t_bool here_doc;
    char **cmd_paths;
    char ***cmd_args;
    int cmd_count;
	char **envp;
	int exit_status;
} t_pipex;

void ft_exit_error(t_pipex *pipex, const char *msg);
void ft_free_array(char **arr, int count);
void ft_free_2d_array(char ***arr, int count);
void free_pipex(t_pipex *pipex);
void ft_init_pipex(t_pipex *pipex, char *infile, char *outfile);
void ft_parse_cmds(t_pipex *pipex, char **argv);
void ft_parse_paths(t_pipex *pipex);

#endif

