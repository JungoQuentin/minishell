#ifndef EXPANSION_H
# define EXPANSION_H
# include "../minishell.h"
# include "../libft/libft.h"

char		*expand(const char *token, t_minishell *minishell);
void		unquote(char *str);
int			get_var_position(int begin_from,
				const char *token, int *start_index, int *end_index);
char		*expand_dollar(const char *token,
				t_minishell *minishell, t_position position);
t_list	*expand_and_retokenize(t_list *tokens, t_minishell *minishell);

#endif /* EXPANSION_H */
