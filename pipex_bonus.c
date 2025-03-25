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

void	ft_init_pipex(t_pipex *pipex, char *infile, char *outfile)
{
	pipex->in_fd = open(infile, O_RDONLY);
	if (pipex->in_fd == -1)
		perror(infile);
	pipex->out_fd = open(outfile, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (pipex->out_fd == -1)
		perror(outfile);
	pipex->cmd_args = NULL;
	pipex->cmd_paths = NULL;
	pipex->cmd_count = 0;
	pipex->here_doc = false;
}

void	execute_first_child(t_pipex *pipex, int *pipefd)
{
	char	*cmd_path;

	cmd_path = find_cmd_path(pipex->cmd_args[0][0], pipex->cmd_paths);
	if (!cmd_path)
	{
		fprintf(stderr, "%s: command not found\n", pipex->cmd_args[0][0]);
		free_pipex(pipex);
		exit(127);
	}
	if (pipex->in_fd == -1 || dup2(pipex->in_fd, STDIN_FILENO) == -1
		|| dup2(pipefd[1], STDOUT_FILENO) == -1)
	{
		free(cmd_path);
		free_pipex(pipex);
		perror("pipex: input redirection");
		exit(1);
	}
	close(pipefd[0]);
	close(pipefd[1]);
	execve(cmd_path, pipex->cmd_args[0], pipex->envp);
	perror("execve");
	free(cmd_path);
	exit(1);
}

void	execute_second_child(t_pipex *pipex, int *pipefd)
{
	char	*cmd_path;

	cmd_path = find_cmd_path(pipex->cmd_args[1][0], pipex->cmd_paths);
	if (!cmd_path)
	{
		fprintf(stderr, "%s: command not found\n", pipex->cmd_args[1][0]);
		free_pipex(pipex);
		exit(127);
	}
	if (dup2(pipefd[0], STDIN_FILENO) == -1 || pipex->out_fd == -1
		|| dup2(pipex->out_fd, STDOUT_FILENO) == -1)
	{
		free(cmd_path);
		free_pipex(pipex);
		perror("pipex: output redirection");
		exit(1);
	}
	close(pipefd[1]);
	close(pipefd[0]);
	execve(cmd_path, pipex->cmd_args[1], pipex->envp);
	perror("execve");
	free(cmd_path);
	exit(1);
}

void	ft_execute_pipex(t_pipex *pipex)
{
	int		pipefd[2];
	pid_t	pid1;
	pid_t	pid2;
	int		status;

	if (pipe(pipefd) == -1)
		ft_exit_error(pipex, "pipe failed");
	pid1 = fork();
	if (pid1 == -1)
		ft_exit_error(pipex, "fork failed");
	if (pid1 == 0)
		execute_first_child(pipex, pipefd);
	pid2 = fork();
	if (pid2 == -1)
		ft_exit_error(pipex, "fork failed");
	if (pid2 == 0)
		execute_second_child(pipex, pipefd);
	close(pipefd[0]);
	close(pipefd[1]);
	waitpid(pid1, NULL, 0);
	waitpid(pid2, &status, 0);
	pipex->exit_status = WEXITSTATUS(status);
}

static void	execute_child(t_pipex *pipex, int in_fd, int out_fd, char **cmd)
{
	char	*cmd_path;

	if (dup2(in_fd, STDIN_FILENO) == -1 || dup2(out_fd, STDOUT_FILENO) == -1)
		ft_exit_error(pipex, "dup2 error");
	cmd_path = find_cmd_path(cmd[0], pipex->cmd_paths);
	if (!cmd_path)
	{
		fprintf(stderr, "%s: command not found\n", cmd[0]);
		free_pipex(pipex);
		exit(127);
	}
	execve(cmd_path, cmd, pipex->envp);
	perror("execve");
	free(cmd_path);
	exit(1);
}

void	execute_multiple_cmds(t_pipex *pipex)
{
	int		i;
	int		pipefd[2];
	int		prev_fd;
	pid_t	pid;
	int		status;

	i = 0;
	prev_fd = pipex->in_fd;
	while (i < pipex->cmd_count)
	{
		if (i < pipex->cmd_count - 1 && pipe(pipefd) == -1)
			ft_exit_error(pipex, "pipe error");
		pid = fork();
		if (pid == -1)
			ft_exit_error(pipex, "fork error");
		if (pid == 0)
		{
			if (i == pipex->cmd_count - 1)
				execute_child(pipex, prev_fd, pipex->out_fd, pipex->cmd_args[i]);
			else
				execute_child(pipex, prev_fd, pipefd[1], pipex->cmd_args[i]);
		}
		if (prev_fd != pipex->in_fd)
			close(prev_fd);
		if (i < pipex->cmd_count - 1)
		{
			close(pipefd[1]);
			prev_fd = pipefd[0];
		}
		i++;
	}
	while (i-- > 0)
		wait(&status);
	if (WIFEXITED(status))
		pipex->exit_status = WEXITSTATUS(status);
	else
		pipex->exit_status = 1;

}
void handle_here_doc(t_pipex *pipex, int argc, char **argv)
{
	int temp_fd;
	char *line;
	char *limiter;
	int i = 0;

	if (argc < 6)
		ft_exit_error(pipex, "Usage: ./pipex here_doc LIMITER cmd1 ... outfile");

	pipex->here_doc = true;
	limiter = argv[2];
	temp_fd = open(".heredoc_tmp", O_CREAT | O_WRONLY | O_TRUNC, 0644);
	if (temp_fd < 0)
		ft_exit_error(pipex, "here_doc temp file");

	while (1)
	{
		write(1, "> ", 2);
		line = get_next_line(STDIN_FILENO);
		if (!line)
			break;
		if (ft_strncmp(line, limiter, ft_strlen(limiter)) == 0
			&& line[ft_strlen(limiter)] == '\n')
		{
			free(line);
			break;
		}
		write(temp_fd, line, ft_strlen(line));
		free(line);
	}
	close(temp_fd);

	pipex->in_fd = open(".heredoc_tmp", O_RDONLY);
	pipex->out_fd = open(argv[argc - 1], O_WRONLY | O_CREAT | O_APPEND, 0644);
	if (pipex->in_fd == -1 || pipex->out_fd == -1)
		ft_exit_error(pipex, "here_doc open");

	pipex->cmd_count = argc - 4;
	pipex->cmd_args = malloc(sizeof(char **) * pipex->cmd_count);
	if (!pipex->cmd_args)
		ft_exit_error(pipex, "malloc failed");

	i = 0;
	while (i < pipex->cmd_count)
	{
		pipex->cmd_args[i] = ft_split(argv[i + 3], ' ');
		if (!pipex->cmd_args[i] || !pipex->cmd_args[i][0])
			ft_exit_error(pipex, "command not found");
		i++;
	}
	ft_parse_paths(pipex);
	execute_multiple_cmds(pipex);
	unlink(".heredoc_tmp");
	free_pipex(pipex);
}
void	handle_mandatory(t_pipex *pipex, char **argv, int argc)
{
	ft_init_pipex(pipex, argv[1], argv[argc - 1]);
	ft_parse_cmds(pipex, argv);
	ft_parse_paths(pipex);
	ft_execute_pipex(pipex);
	free_pipex(pipex);
}

void	handle_bonus(t_pipex *pipex, int argc, char **argv)
{
	int	i = 0;

	ft_init_pipex(pipex, argv[1], argv[argc - 1]);
	pipex->cmd_count = argc - 3;
	pipex->cmd_args = malloc(sizeof(char **) * pipex->cmd_count);
	if (!pipex->cmd_args)
		ft_exit_error(pipex, "malloc failed");
	while (i < pipex->cmd_count)
	{
		pipex->cmd_args[i] = ft_split(argv[i + 2], ' ');
		if (!pipex->cmd_args[i] || !pipex->cmd_args[i][0])
			ft_exit_error(pipex, "command not found");
		i++;
	}
	ft_parse_paths(pipex);
	execute_multiple_cmds(pipex);
	free_pipex(pipex);
}

int	main(int argc, char **argv, char **envp)
{
	t_pipex	pipex;

	pipex = (t_pipex){0};
	pipex.envp = envp;
	if (argc >= 6 && strcmp(argv[1], "here_doc") == 0)
		handle_here_doc(&pipex, argc, argv);
	else if (argc == 5)
		handle_mandatory(&pipex, argv, argc);
	else if (argc > 5)
		handle_bonus(&pipex, argc, argv);
	else
	{
		printf("Usage:\n");
		printf("./pipex infile cmd1 cmd2 outfile\n");
		printf("./pipex here_doc LIMITER cmd1 ... outfile\n");
		return (1);
	}
	return (pipex.exit_status);
}
