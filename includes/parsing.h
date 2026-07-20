/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbenhass <jbenhass@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/28 21:02:15 by jbenhass          #+#    #+#             */
/*   Updated: 2026/07/20 16:36:21 by jbenhass         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#ifndef PARSING_H
# define PARSING_H

# include <limits.h>
# include <stdbool.h>
# include <stdio.h>
# include <string.h>
# include <unistd.h>

typedef enum e_scheduler_type
{
	FIFO,
	EDF
}						t_scheduler_type;

typedef enum e_parsing_errors
{
	// DEV
	OK,
	NULL_PTR,
	// USER ERROR MESSAGE
	INVALID_ARGUMENTS_NB,
	INVALID_SCHEDULER,
	EMPTY_ARGS_VALUE,
	NOT_NUMERIC,
	NEGATIVE_NB,
	INTEGER_OVERFLOW,
}						t_parsing_errors;

typedef struct s_args
{
	unsigned long		nb_coders;
	unsigned long		time_to_burnout;
	unsigned long		time_to_compile;
	unsigned long		time_to_debug;
	unsigned long		time_to_refactor;
	unsigned long		nb_compiles_required;
	unsigned long		dongle_cooldown;
	t_scheduler_type	scheduler_type;
}						t_args;

t_parsing_errors		parse_args(const int argc, const char **argv,
							t_args *args);
int						print_error(t_parsing_errors err);

#endif
