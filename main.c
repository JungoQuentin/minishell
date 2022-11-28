#include "libft/libft.h"		// ...
#include <stdio.h>				// ...
#include <stdlib.h>				// ...
#include <unistd.h>				// ...

#include <readline/readline.h>	// readline lib
#include <readline/history.h>	// readline history

// test la fonction readline
int main() {

	char *line;
	char *prompt_msg;

	char current_dir[2048];
	getcwd(current_dir, 2048);
	printf("dir : %s \n", current_dir);
	prompt_msg = ft_sprintf("%s %s %s", "hostname", current_dir, " > ");

	while (1)
	{
		line = readline(prompt_msg);
		if (line == NULL)
		{
			// Ca arrive quand on lache un Ctrl-D
			rl_clear_history();
			printf("EOF rl_clear_history\n");
			continue;
		}
		if (ft_strlen(line) == 0)
		{
			// Ca arrive quand on enter sans rien...
			//printf("ligne vide\n");
			rl_replace_line("ligne videe...", 10);
			rl_redisplay();
			continue;
		}
		add_history(line);
		int c = rl_on_new_line();
		printf("rl_on_new_line : %d\n", c);
		free(line);
	}
	printf("line: %s", line);
}
