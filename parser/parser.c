#include <stdio.h>
#include "parser.h"
#include <unistd.h>
#include "../libft/libft.h"

int	logic(t_list *cursor, t_cmd *cmd)
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
			set_command(start_left, cmd->logic.left);
			set_command(start_right, cmd->logic.right);
			return (USED);
		}
		cursor = cursor->next;
	}
	return (FALSE);
}

t_bool redir(t_list *tokens, t_cmd *cmd)
{
	t_list	*cursor;
	int		tok_type;

	cursor = tokens;
	while (cursor->next != NULL)
	{
		tok_type = get_token_type((char *)cursor->next->content);
		// TODO dabors le out ? (ca devrait etre egal...)
		if (tok_type == REDIR_IN || tok_type == REDIR_OUT)
		{
			cmd->type = (t_cmd_type)tok_type;
			cmd->redir.filename = (char *)cursor->next->next->content;
			/* saute le `> file`, donc 2 tokens */
			cursor->next = cursor->next->next->next;
			cmd->redir.cmd = (t_cmd *)malloc(sizeof(t_cmd));
			set_command(tokens, cmd->redir.cmd);
			return (TRUE);
		}
		cursor = cursor->next;
	}
	return (FALSE);
}

void	print_rest(t_list *cursor)
{
	while (cursor != NULL)
	{
		printf("%s ", (char *)cursor->content);
		cursor = cursor->next;
	}
	printf("\n");
}

int	set_command(t_list *tokens, t_cmd *cmd)
{
	int		exit_status;
	//print_rest(tokens);

	if (are_we_in_parentheses(tokens))
	{
		//printf("parentheses -> ");
		tokens = lst_cut_first_and_last(tokens);
		//print_rest(tokens);
	}
	exit_status = logic(tokens, cmd);
	if (exit_status == ERROR || exit_status == USED)
		return (exit_status);
	exit_status = redir(tokens, cmd);
	if (exit_status == ERROR || exit_status == USED)
		return (exit_status);
	exit_status = pipeline(tokens, cmd);
	if (exit_status == ERROR || exit_status == USED)
		return (exit_status);
	exit_status = parse_command(tokens, cmd);
	return (exit_status);
}

t_cmd *parser(t_list *tokens) {
	t_cmd	*cmd = (t_cmd*)malloc(sizeof(t_cmd));
	if (set_command(tokens, cmd) == ERROR)
	{
		// TODO
		free(cmd);
		return (NULL);
	}
	return (cmd);
}
