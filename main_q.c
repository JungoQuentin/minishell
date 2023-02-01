#include "libft/libft.h"       // ...
#include <stdio.h>             // ...
#include <stdlib.h>            // ...
#include <unistd.h>            // .
#include <signal.h>
#include <readline/history.h>  // readline history
#include <readline/readline.h> // readline lib
#include "fcntl.h"
#include "sys/wait.h"
// nous
#include "tokenizer/tokenizer.h"
#include "env/env.h"
#include "builtins/builtins.h"
#include "tests/debug_helper.h"
#include "minishell.h"
#include "parser/parser.h"
#include "executor/executor.h"

static int	check_input(char *input)
{
	if (input == NULL)
		return ('0');
	else if (ft_strlen(input) == 0)
		return ('0');
	else if (ft_strncmp(input, "exit", 5) == 0)
	{
		rl_clear_history();
		return ('e');
	}
	else
	{
		add_history(input);
		return (1);
	}
}

static int	init_minishell(t_minishell *minishell, char **envp)
{
	minishell->cmd_input = NULL;
	minishell->prompt_msg = ft_strdup("minishell $>");
	if (!minishell->prompt_msg)
		return (ERROR);
	if (getcwd(minishell->current_working_directory, MAX_PATH_LEN + 1) == NULL)
		return (ERROR);
	minishell->env_copy = cpy_envp(envp);
	if (minishell->env_copy == NULL)
		return (ERROR);
	minishell->env_paths = get_paths_in_env(envp);
	if (minishell->env_paths == NULL)
	{
		split_free(minishell->env_paths);
		return (ERROR);
	}
	return (SUCCESS);
}

int	count_seperateur_in_tooken(t_list *lst, char *sep)
{
	int	count;

	count = 0;
	while (lst)
	{
		if (ft_strncmp(lst->content, sep, ft_strlen(sep)))
			count++;
		lst = lst->next;
	}
	return (count);
}

static int	main_minishell(t_minishell *minishell, char *valid_input)
{
	(void)minishell;
	t_list	*tokens;
	t_cmd	*cmd;

	tokens = tokenizer(valid_input);
	cmd = (t_cmd *)malloc(sizeof(t_cmd));
	set_command(tokens, cmd);
	execute(cmd);
	return (SUCCESS);
}

static int	main_loop(t_minishell *minishell)
{
	int		cmd_code;

	while (1)
	{
		minishell->cmd_input = readline(minishell->prompt_msg);
		cmd_code = check_input(minishell->cmd_input);
		if (cmd_code != 'e')
		{
			if (cmd_code != '0')
			{
				if (main_minishell(minishell, minishell->cmd_input) == ERROR)
					printf("LOL\n");
			}
			free(minishell->cmd_input);
		}
		else
		{
			free(minishell->cmd_input);
			free(minishell->prompt_msg);
			return (0);
		}
	}
}

int	main(int argc, char **argv, char **envp)
{
	t_minishell	minishell;

	(void) argc;
	(void) argv;
	if (init_minishell(&minishell, envp) == ERROR)
		return (EXIT_FAILURE);
	main_loop(&minishell);
	/*
	//char **test = ft_split("export CACA=0", ' ');
	//char **cd_test = ft_split("cd /", ' ');
	//char **untest = ft_split("unset LS_COLORS", ' ');
	//export_(&minishell, test);
	//unset(&minishell, untest);
	env(minishell);
	cd(&minishell, cd_test);
	env(minishell);
	*/
	return (0);
}
