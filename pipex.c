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

void ft_exit_error(t_pipex *pipex, const char *msg)
{
    perror(msg);
    free_pipex(pipex);
    exit(EXIT_FAILURE);
}

void ft_free_array(char **arr, int count)
{
    int i = 0;
    if (!arr)
        return;
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

void ft_free_2d_array(char ***arr, int count)
{
    int i = 0;
    if (!arr)
        return;
    while (i < count)
    {
        if (arr[i])
            ft_free_array(arr[i], -1);
        i++;
    }
    free(arr);
}

void free_pipex(t_pipex *pipex)
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

void ft_init_pipex(t_pipex *pipex, char *infile, char *outfile)
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

char *find_cmd_path(char *cmd, char **paths)
{
    int i = 0;
    char *temp;

    while (paths[i])
    {
        temp = ft_strjoin(paths[i], cmd);
        if (!temp)
            return (NULL);
        if (access(temp, X_OK) == 0)
            return (temp);
        free(temp);
        i++;
    }
    return (NULL);
}

char *get_path_from_env(char *envp[])
{
    int i = 0;
    while (envp[i])
    {
        if (ft_strncmp(envp[i], "PATH=", 5) == 0)
            return (envp[i] + 5);
        i++;
    }
    return (NULL);
}

void ft_parse_cmds(t_pipex *pipex, char **argv)
{
    int i = 0;

    pipex->cmd_count = 2;
    pipex->cmd_args = malloc(sizeof(char **) * pipex->cmd_count);
    if (!pipex->cmd_args)
        ft_exit_error(pipex, "malloc failed");

    while (i < pipex->cmd_count)
    {
        pipex->cmd_args[i] = ft_split(argv[i + 2], ' ');
        if (!pipex->cmd_args[i] || !pipex->cmd_args[i][0])
        {
            if (pipex->cmd_args[i])
                ft_free_array(pipex->cmd_args[i], -1); // 💡 Free current entry before exit
            fprintf(stderr, "pipex: command not found\n");
            while (--i >= 0)
                ft_free_array(pipex->cmd_args[i], -1);
            free(pipex->cmd_args);
            exit(127);
        }


        i++;
    }
}

void ft_parse_paths(t_pipex *pipex)
{
    int j = 0;
    char *path_value = get_path_from_env(pipex->envp);

    if (!path_value)
        ft_exit_error(pipex, "PATH not found");

    pipex->cmd_paths = ft_split(path_value, ':');
    if (!pipex->cmd_paths)
        ft_exit_error(pipex, "Failed to split PATH");

    while (pipex->cmd_paths[j])
    {
        char *temp = ft_strjoin(pipex->cmd_paths[j], "/");
        free(pipex->cmd_paths[j]);
        pipex->cmd_paths[j] = temp;
        j++;
    }
}

void ft_execute_pipex(t_pipex *pipex)
{
    int pipefd[2];
    pid_t pid1, pid2;
    int status;

    if (pipe(pipefd) == -1)
        ft_exit_error(pipex, "pipe failed");

    pid1 = fork();
    if (pid1 == -1)
        ft_exit_error(pipex, "fork failed");

    if (pid1 == 0)
    {
        char *cmd_path = find_cmd_path(pipex->cmd_args[0][0], pipex->cmd_paths);
        if (!cmd_path)
        {
            fprintf(stderr, "%s: command not found\n", pipex->cmd_args[0][0]);
            free_pipex(pipex);
            exit(127);
        }
        if (pipex->in_fd == -1 || dup2(pipex->in_fd, STDIN_FILENO) == -1 || dup2(pipefd[1], STDOUT_FILENO) == -1)
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

    pid2 = fork();
    if (pid2 == -1)
        ft_exit_error(pipex, "fork failed");

    if (pid2 == 0)
    {
        char *cmd_path = find_cmd_path(pipex->cmd_args[1][0], pipex->cmd_paths);
        if (!cmd_path)
        {
            fprintf(stderr, "%s: command not found\n", pipex->cmd_args[1][0]);
            free_pipex(pipex);
            exit(127);
        }
        if (dup2(pipefd[0], STDIN_FILENO) == -1 || pipex->out_fd == -1 || dup2(pipex->out_fd, STDOUT_FILENO) == -1)
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

    close(pipefd[0]);
    close(pipefd[1]);
    waitpid(pid1, NULL, 0);
    waitpid(pid2, &status, 0);
    pipex->exit_status = WEXITSTATUS(status);
}

int main(int argc, char **argv, char **envp)
{
    t_pipex pipex = {0};

    if (argc != 5)
    {
        fprintf(stderr, "Usage: ./pipex infile \"cmd1\" \"cmd2\" outfile\n");
        return (1);
    }

    pipex.envp = envp;
    ft_init_pipex(&pipex, argv[1], argv[argc - 1]);
    ft_parse_cmds(&pipex, argv);
    ft_parse_paths(&pipex);
    ft_execute_pipex(&pipex);
    free_pipex(&pipex);
    return (pipex.exit_status);
}