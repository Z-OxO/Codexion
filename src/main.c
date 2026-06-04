/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbenhass <jbenhass@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/29 02:20:56 by jbenhass          #+#    #+#             */
/*   Updated: 2026/06/03 21:34:44 by jbenhass         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	main(int argc, const char **argv)
{
	t_args				args;
	t_parsing_errors	err;
	t_sim				sim;
	pthread_t			tid_coder;
	pthread_t			tid_scheduler;

	err = parse_args(argc, argv, &args);
	if (err != OK)
		return (print_error(err));
	init_sim(&sim, &args);
	pthread_create(&tid_scheduler, NULL, &scheduler_routine, (void *)&sim);
	pthread_create(&tid_coder, NULL, &coder_routine, (void *)&sim.coders[0]);
	sim.sim_start = get_ms(0);
	pthread_join(tid_coder, NULL);
	pthread_join(tid_scheduler, NULL);
	return (OK);
}
