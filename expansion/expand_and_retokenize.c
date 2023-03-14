/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand_and_retokenize.c                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: qjungo <qjungo@student.42lausanne.ch>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/14 17:22:11 by qjungo            #+#    #+#             */
/*   Updated: 2023/03/14 17:43:56 by qjungo           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "expansion.h"

void	retokenize(t_list *cursor, char **split, t_minishell *minishell)
{
	int		i;
	t_list	*new;
	t_list	*next;

	next = cursor->next;
	free(cursor->content);
	cursor->content = split[0];
	i = 1;
	while (split[i] != NULL)
	{
		new = ft_lstnew(split[i]);
		if (new == NULL)
			malloc_error(minishell);
		append_to_free_list(minishell, new);
		cursor->next = new;
		cursor = new;
		i++;
	}
	free(split);
	cursor->next = next;
}

void	split_and_retokenize(t_list *cursor, t_minishell *msh)
{
	char	**split;

	split = ft_split((char *)cursor->content, ' ');
	if (split == NULL)
		malloc_error(msh);
	if (split[0] == NULL || split[1] == NULL)
	{
		if (split[0] != NULL)
			free(split[0]);
		free(split);
		return ;
	}
	retokenize(cursor, split, msh);
}

void	expand_and_retokenize(t_list **tokens, t_minishell *minishell)
{
	t_list	*cursor;
	t_list	*last;

	last = NULL;
	cursor = *tokens;
	while (cursor != NULL)
	{
		expand((char **)&cursor->content, minishell);
		if (ft_strlen(cursor->content) == 0)
		{
			if (last == NULL)
				*tokens = cursor->next;
			else
				last->next = cursor->next;
			free(cursor->content);
			cursor = cursor->next;
			continue ;
		}
		split_and_retokenize(cursor, minishell);
		unquote((char *)cursor->content);
		last = cursor;
		cursor = cursor->next;
	}
}
