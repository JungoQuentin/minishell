#ifndef EXECUTOR_H
# define EXECUTOR_H

typedef enum e_cmd_type {
	PIPELINE,		/* cmd | cmd | cmd | ... */
	COMMAND,		/* cmd */
	LOGIC_AND,		/* cmd && cmd */
	LOGIC_OR,		/* cmd || cmd */
	REDIR_IN,		/* < file cmd */
	REDIR_OUT,		/* cmd > file */
	REDIR_APPEND,	/* cmd >> file */
} t_cmd_type;

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
			struct s_cmd *next; // only for pipeline
		} cmd;
		struct {
			int	*pipes;
			int *pids;
			int	pipe_count; // we could just count the count next commands
			struct s_cmd *first_cmd;
		} pipeline;
		struct {
			char *filename;
			int fd;
			struct s_cmd *cmd;
		} redir;
	};
} t_cmd;

// utils
int			fork1();
int			pipe_index(int i, int read);
void		close_all_pipes(int *fds, int pipe_count);

// execute.c
int	execute(t_cmd *cmd);

#endif /* EXECUTOR_H */
