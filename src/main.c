/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbenhass <jbenhass@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/29 02:20:56 by jbenhass          #+#    #+#             */
/*   Updated: 2026/06/02 15:39:24 by jbenhass         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	main(int argc, const char **argv)
{
	t_args				args;
	t_parsing_errors	err;
	t_sim				sim;

	err = parse_args(argc, argv, &args);
	if (err != OK)
		return (print_error(err));
	printf("nb coders: %lu\n", args.nb_coders);
	init_sim(&sim, &args);
	printf("%d\n", sim.coders[0].id);
	return (OK);
}
