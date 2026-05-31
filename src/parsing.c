/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbenhass <jbenhass@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/28 21:02:04 by jbenhass          #+#    #+#             */
/*   Updated: 2026/05/31 19:49:07 by jbenhass         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "parsing.h"

static bool	is_num(char c)
{
	return (c >= '0' && c <= '9');
}

static bool	is_possible_timings(const t_args *args)
{
	if (args->time_to_compile + args->time_to_debug
		+ args->time_to_refactor >= args->time_to_burnout)
		return (false);
	if (args->dongle_cooldown >= args->time_to_burnout)
		return (false);
	return (true);
}

int	print_error(t_parsing_errors err)
{
	static const char	*msg[] = {"",
			"Error: Internal NULL pointer detected.\n",
			"Parsing Error: Invalid number of arguments.\n",
			"Parsing Error: Scheduler must be 'fifo' or 'edf'.\n",
			"Parsing Error: Empty argument provided.\n",
			"Parsing Error: Arguments must strictly be numeric.\n",
			"Parsing Error: Negative numbers are not allowed.\n",
			"Parsing Error: Integer overflow detected.\n",
			"Parsing Error: Impossible timing parameters provided.\n"};

	if (err > OK && err <= IMPOSSIBLE_TIMING)
		write(2, msg[err], strlen(msg[err]));
	return (1);
}

static t_parsing_errors	string_to_ul(const char *str, unsigned long *ret_ptr)
{
	int					i;
	unsigned long long	nb;

	i = 0;
	nb = 0;
	if (!ret_ptr || !str)
		return (NULL_PTR);
	if (!str[0])
		return (EMPTY_ARGS_VALUE);
	if (str[0] == '-')
		return (NEGATIVE_NB);
	while (str[i])
	{
		if (!is_num(str[i]))
			return (NOT_NUMERIC);
		nb *= 10;
		nb += str[i] - '0';
		i++;
	}
	if (nb > INT_MAX || i > 10)
		return (INTEGER_OVERFLOW);
	*ret_ptr = nb;
	return (OK);
}

t_parsing_errors	parse_args(const int argc, const char **argv, t_args *args)
{
	unsigned long		*converted_args;
	t_parsing_errors	ret;
	int					i;

	if (argc != 9)
		return (INVALID_ARGUMENTS_NB);
	converted_args = (unsigned long *)args; // Cast to use continuous memory
	i = 0;
	argv++; // Skip program name
	while (i < 7)
	{
		ret = string_to_ul(argv[i], &converted_args[i]);
		if (ret != OK)
			return (ret);
		i++;
	}
	if (strcmp(argv[7], "fifo") == 0)
		args->scheduler_type = FIFO;
	else if (strcmp(argv[7], "edf") == 0)
		args->scheduler_type = EDF;
	else
		return (INVALID_SCHEDULER);
	if (!is_possible_timings(args))
		return (IMPOSSIBLE_TIMING);
	return (OK);
}
