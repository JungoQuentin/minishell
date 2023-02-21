#ifndef PARSER_H
# define PARSER_H
# include "../minishell.h"
# include "../libft/libft.h"

/* TODO une idee pour pouvoir tout free a la fin : faire une list de t_list, 
 *		bien garder la taille, et free tout les elements qui ne sont pas NULL
 *		- en cas d'erreur : tout free
 *		- reussite : ne free que les t_list et pas les strings 
 */

typedef enum e_cmd_type {
	PIPELINE,		/* cmd | cmd | cmd | ... */
	COMMAND,		/* cmd */
	LOGIC_AND,		/* cmd && cmd */
	LOGIC_OR,		/* cmd || cmd */
	REDIR_IN,		/* < file cmd */
	REDIR_OUT,		/* cmd > file */
	REDIR_APPEND,	/* cmd >> file */
	HEREDOC,		/* cmd << EOF */
} t_cmd_type;

// la suite, juste pour le parser
enum e_token_type {
  OPEN_PARENTHESES = 10,
  CLOSE_PARENTHESES,
};

enum e_result_more {
	USED = 2,
};

typedef struct s_cmd {
	t_cmd_type	type;
	union {
		// TODO simplify this
		struct {
			struct s_cmd *left;
			struct s_cmd *right;
		} logic;
		struct {
			struct s_cmd *left;
			struct s_cmd *right;
		} pipe;
		struct {
			char **argv;
			char *heredoc; // NULL if none
			struct s_cmd *next; // only for pipeline
		} cmd;
		struct {
			int	*pipes;
			int *pids;
			int	pipe_count; // n of command (and not of the pipe op)
			struct s_cmd *first_cmd;
		} pipeline;
		struct {
			char *filename;
			int fd;
			struct s_cmd *cmd;
		} redir;
	};
} t_cmd;

t_list		*skip_parentheses(t_list *cursor);
int			get_token_type(char *token);
t_bool		are_we_in_parentheses(t_list *tokens);
t_list		*lst_cut_first_and_last(t_list *tokens);

int			parse_command(t_list *tokens, t_cmd *cmd, t_minishell *minishell);
int			pipeline(t_list *tokens, t_cmd *cmd, t_minishell *minishell);
int			set_command(t_list *tokens, t_cmd *cmd, t_minishell *minishell);
t_cmd		*parser(t_list *tokens, t_minishell *minishell);

#endif /* PARSER_H */
