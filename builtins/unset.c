#include "../libft/libft.h"
#include "../minishell.h"
#include "../env/env.h"

static int	remove_env_var(t_minishell *mini, int index_var)
{
	int		last;

	last = 0;
	while (mini->env_copy[last] != NULL)
		last++;
	free(mini->env_copy[index_var]);
	mini->env_copy[index_var] = mini->env_copy[last - 1];
	mini->env_copy[last - 1] = NULL;
	return (SUCCESS);
}

int	unset(t_minishell *minishell, char **args)
{
	// TODO path
	char	*var;
	int		var_index;

	var = ft_strjoin(args[1], "=");
	if (var == NULL)
		return (ERROR);
	var_index = get_env_var_index((const char **)minishell->env_copy, var);
	if (var_index != -1)
		remove_env_var(minishell, var_index);
	if (ft_strncmp(var, "PATH=", 5))
	{
		split_free(minishell->env_paths);
		minishell->env_paths = NULL;
	}
	free(var);
	return (SUCCESS);
}
