#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <readline/history.h>
#include <readline/readline.h>
#include <termios.h>
// nous
#include "libft/libft.h"
#include "tokenizer/tokenizer.h"
#include "minishell.h"
#include "executor/executor.h"
#include "token_checker/token_checker.h"
#include "builtins/builtins.h"


void	signal_handler(int sig)
{
	printf("signal_handler: %d\n", sig);
}

int main(int argc, char **argv, char **envp)
{
	char	*cmd_input;

	struct sigaction	prompt_sa;
	struct sigaction	exec_sa;

	prompt_sa.sa_handler = signal_handler;
	sigaction(SIGINT, &prompt_sa, &exec_sa);
	while (TRUE)
	{
		cmd_input = readline("minishell$ ");
		if (cmd_input == NULL)
		{
			write(1, "exit\n", 5);
			break ;
		}
		if (ft_strlen(cmd_input) == 0)
			continue ;
		add_history(cmd_input);
		free(cmd_input);
		sigaction(SIGINT, &exec_sa, &prompt_sa);
		printf("start\n");
		usleep(100000000); // EXEC
		printf("end\n");
		sigaction(SIGINT, &prompt_sa, &exec_sa);
	}
}
