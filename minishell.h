#ifndef MINISHELL_H
# define MINISHELL_H
# define MAX_PATH_LEN 4096

typedef struct s_minishell {
	//char	*prompt;  // avoir un truc constant + le path qui change
	//char	*username;

	char	current_working_directory[MAX_PATH_LEN];

	char	**env_paths; // deja parsee
	char	**env_copy; // malloc / pour export et unset

	// last ast ?
}	t_minishell;

typedef struct s_position
{
	int start;	// inclusif
	int end;	// exclusif
}	t_position;

#endif /* MINISHELL_H */
