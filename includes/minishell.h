/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: meunostu <meunostu@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/02 05:36:17 by meunostu          #+#    #+#             */
/*   Updated: 2021/05/22 17:54:05 by meunostu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H

# include <stdlib.h>
# include <unistd.h>
# include <stdio.h>
# include "libft.h"

# define NO_VALID_ENV_VAR ".,-'?/$"
# define NO_VALID_COMMAND_SIMBOLS ":\"'$"

/*
** Errors
*/
# define ERROR_MALLOC "malloc"
# define ERROR_COMMAND "command not found"

typedef struct s_parser
{
	char			*line;
	int 			cur_c;
	int 			pars_command;
	int 			pars_var;
	int 			pars_args;
	int 			pars_flags;
}					t_parser;

typedef struct s_redir
{
	char			*command;
	char			*flags;
	char			**args;
	int 			redir_to;
	char			*error;
}					t_redir;

typedef struct s_pipe
{
	t_redir			*redir;
	t_redir			*redir_next;
}					t_pipe;

typedef struct s_job
{
	t_pipe			*pipe;
	t_pipe			*pipe_next;
	struct t_job	*job_next;
}					t_job;

typedef struct s_main
{
	t_job			*job;
	char			**my_env;
	int				exit;
}					t_main;

/*
** MAIN
*/

/*
** PARSING
*/
void	parser(t_main *main);


/*
** UTILS
*/
void	exit_with_error(t_main *main, char *massage);
int		add_char(char **str, int c);
int		get_next_char(int *c);

/*
** TESTS
*/
void	tests(void);
char	*pars_env_variables(t_main *main, t_parser *parser);

#endif //MINISHELL_H
