/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbenhass <jbenhass@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/29 02:20:56 by jbenhass          #+#    #+#             */
/*   Updated: 2026/05/31 19:52:11 by jbenhass         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/parsing.h"

int	main(int argc, const char **argv)
{
	t_args				args;
	t_parsing_errors	err;

	err = parse_args(argc, argv, &args);
	if (err != OK)
		return (print_error(err));
	printf("nb coders: %lu\n", args.nb_coders);
	return (OK);
}
