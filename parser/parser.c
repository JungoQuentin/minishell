#include <stdio.h>
#include "parser.h"
#include <unistd.h>
#include "../libft/libft.h"

int	logic(t_list *cursor, t_cmd *cmd, t_minishell *minishell)
{
	int			tok_type;
	t_list		*start_left = cursor;
	t_list		*start_right = cursor;

	while (cursor->next != NULL)
	{
		tok_type = get_token_type((char *)cursor->next->content);
		if (tok_type == LOGIC_OR || tok_type == LOGIC_AND)
		{
			start_right = cursor->next->next;
			free(cursor->next->content);
			cursor->next = NULL;

			cmd->type = (t_cmd_type)tok_type;
			cmd->logic.left = (t_cmd *)malloc(sizeof(t_cmd));
			cmd->logic.right = (t_cmd *)malloc(sizeof(t_cmd));
			set_command(start_left, cmd->logic.left, minishell);
			set_command(start_right, cmd->logic.right, minishell);
			return (USED);
		}
		cursor = cursor->next;
	}
	return (FALSE);
}

int redir(t_list *tokens, t_cmd *cmd, t_minishell *minishell)
{
	t_list	*cursor;
	int		tok_type;

	cursor = tokens;
	while (cursor->next != NULL)
	{
		tok_type = get_token_type((char *)cursor->next->content);
		if (tok_type == REDIR_IN || tok_type == REDIR_OUT)
		{
			cmd->type = (t_cmd_type)tok_type;
			cmd->redir.filename = (char *)cursor->next->next->content;
			cursor->next = cursor->next->next->next;
			cmd->redir.cmd = (t_cmd *)malloc(sizeof(t_cmd));
			set_command(tokens, cmd->redir.cmd, minishell);
			return (USED);
		}
		cursor = cursor->next;
	}
	return (FALSE);
}

void	print_rest(t_list *cursor)
{
	printf("[");
	while (cursor != NULL)
	{
		printf("%s, ", (char *)cursor->content);
		cursor = cursor->next;
	}
	printf("]\n");
}

int	set_command(t_list *tokens, t_cmd *cmd, t_minishell *minishell)
{
	int		exit_status;
	//print_rest(tokens);

	if (are_we_in_parentheses(tokens))
	{
		if (ft_strncmp((char *)tokens->next->content, ")", 2) == 0)
		{
			// TODO
			write(2, "minishell: syntax error near unexpected token `)'\n", 50);
			return (EXIT_FAILURE);
		}
		tokens = lst_cut_first_and_last(tokens);
	}
	exit_status = logic(tokens, cmd, minishell);
	if (exit_status == ERROR || exit_status == USED)
		return (exit_status);
	exit_status = redir(tokens, cmd, minishell);
	if (exit_status == ERROR || exit_status == USED)
		return (exit_status);
	exit_status = pipeline(tokens, cmd, minishell);
	if (exit_status == ERROR || exit_status == USED)
		return (exit_status);
	exit_status = parse_command(tokens, cmd, minishell);
	return (exit_status);
}

/* TODO libft
t_list	*get_last(t_list *list, t_list *current)
{
	t_list	*ptr;

	ptr = list;
	while (ptr->next != current && ptr->next != NULL)
		ptr = ptr->next;
	return (ptr);
}
*/

t_cmd *parser(t_list *tokens, t_minishell *minishell) 
{
	t_cmd	*cmd = (t_cmd*)malloc(sizeof(t_cmd));

	// TODO dans le main ?
	//tokens = expand_and_retokenize(tokens, minishell);
	(void)minishell;

	if (set_command(tokens, cmd, minishell) == ERROR)
	{
		// TODO
		free(cmd);
		return (NULL);
	}
	return (cmd);
}
