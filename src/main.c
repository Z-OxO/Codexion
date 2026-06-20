/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbenhass <jbenhass@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/29 02:20:56 by jbenhass          #+#    #+#             */
/*   Updated: 2026/06/20 18:47:54 by jbenhass         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	launch_threads(t_sim *sim, pthread_t *tids)
{
	unsigned int	i;

	pthread_create(&tids[0], NULL, &scheduler_routine, sim);
	i = 0;
	while (i < sim->args->nb_coders)
	{
		pthread_create(&tids[i + 1], NULL, &coder_routine, &sim->coders[i]);
		i++;
	}
	pthread_create(&tids[sim->args->nb_coders + 1], NULL, &monitor_routine,
		sim);
	pthread_mutex_lock(&sim->lock);
	sim->sim_start = get_ms(0);
	sim->started = 1;
	pthread_cond_broadcast(&sim->start_cond);
	pthread_mutex_unlock(&sim->lock);
	return (0);
}

static void	join_threads(t_sim *sim, pthread_t *tids)
{
	unsigned int	i;

	pthread_join(tids[0], NULL);
	i = 0;
	while (i < sim->args->nb_coders)
	{
		pthread_join(tids[i + 1], NULL);
		i++;
	}
	pthread_join(tids[sim->args->nb_coders + 1], NULL);
}

int	main(int argc, const char **argv)
{
	t_args				args;
	t_parsing_errors	err;
	t_sim				sim;
	pthread_t			*tids;

	err = parse_args(argc, argv, &args);
	if (err != OK)
		return (print_error(err));
	if (init_sim(&sim, &args))
		return (1);
	tids = malloc(sizeof(pthread_t) * (args.nb_coders + 2));
	if (!tids)
		return (clean_sim(&sim));
	launch_threads(&sim, tids);
	join_threads(&sim, tids);
	free(tids);
	clean_sim(&sim);
	return (OK);
}
