/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute_command.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: qjungo <qjungo@student.42lausanne.ch>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/23 20:20:30 by qjungo            #+#    #+#             */
/*   Updated: 2023/02/23 20:23:33 by qjungo           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/wait.h>
#include "executor.h"
#include "../libft/libft.h"
#include "../builtins/builtins.h"
#include "../env/env.h"

void	child(t_minishell *minishell, t_cmd *cmd, int pipes[2])
{
	// TODO refactoriser pour que pipeline ai la meme chose !
	replace_argv0_with_full_path(cmd, minishell);
	if (cmd->s_command.heredoc != NULL)
	{
		dup2(pipes[0], STDIN_FILENO);
		close(pipes[0]);
		close(pipes[1]);
	}
	if (access(cmd->s_command.argv[0], F_OK) == -1)
	{
		ft_putstr_fd((char *)"minishell: ", STDERR_FILENO);
		ft_putstr_fd(cmd->s_command.argv[0], STDERR_FILENO);
		ft_putstr_fd((char *)": command not found\n", STDERR_FILENO);
		exit(127);
	}
	execve(cmd->s_command.argv[0], cmd->s_command.argv, minishell->env_copy);
	// TODO
	perror("execv");
	exit(errno);
}

int	execute_command(t_cmd *cmd, t_minishell *minishell)
{
	int		exit_status;
	int		pipes[2];

	if (cmd->s_command.heredoc != NULL)
	{
		if (pipe(pipes) == -1)
			return (ERROR);
	}
	if (execute_builtin(cmd, minishell, &exit_status))
		return (exit_status);
	exit_status = 0;
	if (fork1() == 0)
		child(minishell, cmd, pipes);
	if (cmd->s_command.heredoc != NULL)
	{
		write(pipes[1], cmd->s_command.heredoc,
			ft_strlen(cmd->s_command.heredoc));
		close(pipes[0]);
		close(pipes[1]);
	}
	wait(&exit_status);
	return (WEXITSTATUS(exit_status));
}