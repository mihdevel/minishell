/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: meunostu <meunostu@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/02 05:40:14 by meunostu          #+#    #+#             */
/*   Updated: 2021/07/01 11:41:55 by meunostu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

char 	**ft_arrdup(char **src, int len)
{
	int i;
	char **dst;

	dst = ft_calloc(len + 2, sizeof(char *));
	i = 0;
	while (src && *src)
		dst[i++] = *src++;
	return (dst);
}

void	append_arg_to_main(t_job *job, t_parser *parser)
{
	char **src;
	char **tmp;

	if (parser->pipe_exist != 1)
		src = job->pipe->redir->args;
	else
		src = job->pipe_next->redir->args;
	tmp = ft_arrdup(src, parser->args_len);
	tmp[parser->args_len++] = parser->line;
	tmp[parser->args_len] = NULL;
	free(src);
	if (parser->pipe_exist != 1)
		job->pipe->redir->args = tmp;
	else
		job->pipe_next->redir->args = tmp;
	parser->line = NULL;
}

void	append_command_to_main(t_job *job, t_parser *parser)
{
	if (!job->pipe->redir->command)
		job->pipe->redir->command = parser->line;
	else
		job->pipe_next->redir->command = parser->line;
	if (parser->line)
		parser->pars_command = 1;
	job->num_commands++;
	parser->line = NULL;
}

void	write_pars_line(t_job *job, t_parser *parser)
{
	if (!parser->pars_command && parser->line && *parser->line)
		append_command_to_main(job, parser);
	else if (parser->line && *parser->line)
		append_arg_to_main(job, parser);
}

void	init_struct_job_next(t_job *job)
{
	t_job	*job_next;
	t_pipe	*pipe;
	t_redir	*redir;

	job_next = (t_job *)malloc(sizeof(t_job));
	pipe = (t_pipe *)malloc(sizeof(t_pipe));
	redir = (t_redir *)malloc(sizeof(t_redir));

	job_next->job_next = NULL;
	redir->redir_file = NULL;
	redir->redir_type = 0;
	job->job_next = job_next;
	job->job_next->pipe = pipe;
	job->job_next->pipe->redir = redir;
	job->job_next->pipe->redir->command = NULL;
	job->job_next->pipe->redir->args = NULL;
}

void	init_struct_pipe_next(t_job *job, int job_next)
{
	t_pipe	*pipe;
	t_redir	*redir;

	pipe = (t_pipe *)malloc(sizeof(t_pipe));
	redir = (t_redir *)malloc(sizeof(t_redir));

	if (job_next)
	{
		redir->redir_file = NULL;
		redir->redir_type = 0;
		job->job_next->pipe_next = pipe;
		job->job_next->pipe_next->redir = redir;
		job->job_next->pipe_next->redir->command = NULL;
		job->job_next->pipe_next->redir->args = NULL;
	}
	else
	{
		redir->redir_file = NULL;
		redir->redir_type = 0;
		job->pipe_next = pipe;
		job->pipe_next->redir = redir;
		job->pipe_next->redir->command = NULL;
		job->pipe_next->redir->args = NULL;
	}
}

void	zero_parser(t_parser *parser)
{
	parser->pipe_exist = 1;
	parser->pars_command = 0;
	parser->pars_args = 0;
	parser->args_len = 0;
}

t_job	*get_next_pipe_addr(t_job *job, t_main *main, t_parser *parser)
{
	write_pars_line(job, parser);
	zero_parser(parser);
	if (!job->pipe_next)
	{
		init_struct_pipe_next(job, 0);
		main->job->num_pipes++;
		return (job);
	}
	else
	{
		parser->pipe_exist = 0;
		init_struct_job_next(job);
		main->job->num_pipes++;
		return (job->job_next);
	}
}

char 	*pars_env_variable(t_parser *parser)
{
	int		c;

	parser->pars_var = 1;
	if (ft_isalnum(parser->cur_c) || parser->cur_c == '_')
		add_char(&parser->variable, parser->cur_c);
	else
		return (parser->variable);
	while (get_next_char(parser, &c) && (ft_isalnum(c) || c == '_') &&
	c != ' ' && c != '\n')
		add_char(&parser->variable, c);
	return (parser->variable);
}

char	*get_env_value(char **my_env, char	*name_variable)
{
	int		i;
	char	*env;
	char	*env_value;

	i = 0;
	env_value = NULL;
	while (my_env[i])
	{
		if (ft_strnstr(my_env[i], name_variable, ft_strlen(name_variable)))
		{
			env = my_env[i];
			while (*env && *env != '=')
				env++;
			env++;
			env_value = ft_strdup(env);
			break ;
		}
		i++;
	}
	return (env_value);
}

void	add_value_in_line(t_parser *parser)
{
	if (parser->variable_value)
		parser->line = ft_strjoin(parser->line, parser->variable_value);
	mem_free(&parser->variable_value);
	mem_free(&parser->variable);
}

t_job	*env_digit(t_main *main, t_job *job, t_parser *parser)
{
	int c;

	if (parser->cur_c == '0')
		parser->line = ft_strjoin(parser->line, "bash");
	else
	{
		while (get_next_char(parser, &c) && strchr(SPEC_SYMBOLS, c))
			add_char(&parser->line, c);
		job = distribution_parser(main , job, parser);
	}
	return (job);
}

t_job	*pars_env_and_append_line(t_parser *parser, t_main *main, t_job *job)
{
	int		c;

	get_next_char(parser, &c);
	if (c == '?')
		parser->line = ft_strjoin(parser->line, ft_itoa(main->exit));
	else if (c == '\n' || c == ' ')
		parser->line = ft_strjoin(parser->line, "$");
	else if (c == '=')
		parser->line = ft_strjoin(parser->line, "$=");
	else if (c == '$')
		parser->line = ft_strjoin(parser->line, "$=");
	else if (c == '|')
		job = get_next_pipe_addr(job, main, parser);
	else if (c == '>' || c == '<')
		job = redirects(job, parser);
	else if (ft_isdigit(c))
		job = env_digit(main, job, parser);
	else
	{
		pars_env_variable(parser);
		parser->variable_value = get_env_value(main->my_env, parser->variable);
		add_value_in_line(parser);
		if (parser->double_quote != 1)
			job = distribution_parser(main, job, parser);
	}
	if (parser->cur_c == ' ')
		job = distribution_parser(main, job, parser);
	return (job);
}

void	print_params(t_main *main)
{
	int i;

	i = -1;
	write(1, "command: ", ft_strlen("command: "));
	write(1, main->job->pipe->redir->command, ft_strlen(main->job->pipe->redir->command));
	while (main->job->pipe->redir->args && main->job->pipe->redir->args[++i])
	{
		write(1, "\nargv[" , ft_strlen("\nargv["));
		ft_putnbr_fd(i, 1);
		write(1, "]: ", ft_strlen("]: "));
		write(1, main->job->pipe->redir->args[i], ft_strlen(main->job->pipe->redir->args[i]));
		write(1, "\n", 1);
	}
}

void	check_symbols_and_append_line(t_job *job, t_parser *parser)
{
	int c;

	c = parser->cur_c;
	if ((!parser->pars_command && !ft_strchr(NO_VALID_COMMAND_SYMBOLS, c)) ||
		(parser->pars_command && !ft_strchr(NO_VALID_SYMBOLS, c)))
			add_char(&parser->line, c);
	else
	{
		if (!parser->pars_command)
		{
			parser->pars_command = 1;
			append_command_to_main(job, parser);
			add_char(&parser->line, c);
		}
		else
		{
			append_arg_to_main(job, parser);
			add_char(&parser->line, c);
		}
	}
}

void	pars_double_quote(t_parser *parser, t_main *main, t_job *job)
{
	int		c;

	parser->double_quote = 1;
	while (parser->cur_c != '\n' && get_next_char(parser, &c) && c != '"' && c != '\n')
	{
		if (c == '$')
		{
			pars_env_and_append_line(parser, main, job);
			if (parser->cur_c == '"')
				break ;
		}
		else
			add_char(&parser->line, c);
	}
	if (c != '"')
		exit_with_error(main, "No two quote");
	parser->double_quote = 0;
}

void	pars_quote(t_parser *parser, t_main *main)
{
	int		c;

	if (parser->double_quote == 1)
	{
		add_char(&parser->line, parser->cur_c);
		return ;
	}
	parser->quote = 1;
	while (get_next_char(parser, &c) && c != '\'' && c != '\n')
		add_char(&parser->line, c);
	if (c != '\'')
		exit_with_error(main, "No two quote");
	parser->quote = 0;
}

t_pipe	*get_current_pipe(t_job *job)
{
	if (job->pipe_next)
		return (job->pipe_next);
	else
		return (job->pipe);
}

char *get_redir_file(t_parser *parser)
{
	int		c;

	while (get_next_char(parser, &c) && ft_strchr(VALID_SYMBOLS_FILES, c) &&
	ft_isalnum(c) && c != '\n')
		add_char(&parser->line, c);
	return (parser->line);
}

t_job	*redirects(t_job *job, t_parser *parser)
{
	int		c;
	int		redir_type;
	char	*redir_file;
	t_pipe	*pipe;

	write_pars_line(job, parser);
	redir_type = 0;
	if (parser->cur_c == '>')
		redir_type = 1;
	else if (parser->cur_c == '<')
		redir_type = 2;
	else
	{
		get_next_char(parser, &c);
		if (c == '>')
			redir_type = 3;
		else if (c == '<')
			redir_type = 4;
		else if (c != ' ')
			add_char(&parser->line, c);
	}
	pipe = get_current_pipe(job);
	redir_file = get_redir_file(parser);
	pipe->redir->redir_type = redir_type;
	pipe->redir->redir_file = redir_file;
	return (job);
}

t_job	*distribution_parser(t_main *main, t_job *job, t_parser *parser)
{
	int	c;

	c = parser->cur_c;
	if (!ft_isprint(c) || c == ';')
		return (job);
	else if (c == '"')
		pars_double_quote(parser, main, job);
	else if (c == '\'')
		pars_quote(parser, main);
	else if (c == '$')
		pars_env_and_append_line(parser, main, job);
	else if (c == '|')
		job = get_next_pipe_addr(job, main, parser);
	else if (c == '>' || c == '<')
		job = redirects(job, parser);
	else if (c == ' ')
		write_pars_line(job, parser);
	else
		check_symbols_and_append_line(job, parser);
	return (job);
}

void	print_error_message(t_main *main, char *line, int len)
{
	char	*error;
	set_error(main->job->pipe->redir, 1);
	error = "bash: syntax error near unexpected token `";
	write(1, error, strlen(error));
	write(1, line, len);
	write(1, "'\n", 2);
}

void	parser_go(t_main *main, t_parser *parser)
{
	int		c;
	int		i;
	t_job	*job;

	i = 1;
	job = main->job;
	while (parser->cur_c != '\n' && get_next_char(parser, &c) && c != '\n')
	{
		job = distribution_parser(main, job, parser);
		if (i == 2 && parser->line && ft_strchr(SPECIFICATORS, *parser->line) &&
		ft_strchr(SPECIFICATORS, c))
			print_error_message(main, parser->line,  i);
		i++;
	}
	if (i == 2 && parser->line && ft_strchr(SPECIFICATORS, *parser->line) &&
	c == '\n')
		print_error_message(main, parser->line,  i - 1);
	write_pars_line(job, parser);
	// 	print_params(main);
}

void	init_parser(t_parser *parser)
{
	parser->line = NULL;
	parser->pars_command = 0;
	parser->pars_args = 0;
	parser->pars_var = 0;
	parser->double_quote = 0;
	parser->args_len = 0;
	parser->pipe_exist = 0;
	parser->variable = NULL;
	parser->variable_value = NULL;
}

void	end_session_pars(t_parser *parser)
{
	parser->line = NULL;
}

void	parser(t_main *main, char *string)
{
	t_parser	parser;

	init_parser(&parser);
	parser.string = string;
	parser_go(main, &parser);
	end_session_pars(&parser);
//	TODO add static to functions
}
