/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbenhass <jbenhass@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/31 19:43:45 by jbenhass          #+#    #+#             */
/*   Updated: 2026/06/22 20:54:34 by jbenhass         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	clean_sim(t_sim *sim)
{
	unsigned int	i;

	if (!sim)
		return (1);
	i = 0;
	while (i < sim->args->nb_coders)
	{
		if (sim->coder_wake)
			pthread_cond_destroy(&sim->coder_wake[i]);
		i++;
	}
	pthread_mutex_destroy(&sim->lock);
	pthread_mutex_destroy(&sim->log_mutex);
	pthread_cond_destroy(&sim->sched_wake);
	pthread_cond_destroy(&sim->start_cond);
	pthread_cond_destroy(&sim->mon_cond);
	free(sim->coders);
	free(sim->coder_wake);
	free(sim->dongles);
	free(sim->granted);
	free(sim->pq.data);
	free(sim->backfill);
	return (1);
}

// Simplify init_sim condition | 0 error 1 succes
static int	allocate_arrays(t_sim *sim)
{
	sim->coders = ft_calloc(sim->args->nb_coders, sizeof(t_coder));
	sim->coder_wake = ft_calloc(sim->args->nb_coders, sizeof(pthread_cond_t));
	sim->dongles = ft_calloc(sim->args->nb_coders, sizeof(t_dongle));
	sim->granted = ft_calloc(sim->args->nb_coders, sizeof(bool));
	sim->pq.data = ft_calloc(sim->args->nb_coders, sizeof(t_node));
	sim->backfill = ft_calloc(sim->args->nb_coders, sizeof(t_node));
	if (!sim->coders || !sim->coder_wake || !sim->dongles || !sim->granted
		|| !sim->pq.data || !sim->backfill)
		return (0);
	sim->pq.size = 0;
	return (1);
}

int	init_sim(t_sim *sim, t_args *args)
{
	unsigned int	i;

	if (!sim || !args)
		return (1);
	memset(sim, 0, sizeof(t_sim));
	sim->args = args;
	if (!allocate_arrays(sim))
		return (clean_sim(sim));
	pthread_mutex_init(&sim->lock, NULL);
	pthread_mutex_init(&sim->log_mutex, NULL);
	pthread_cond_init(&sim->sched_wake, NULL);
	pthread_cond_init(&sim->start_cond, NULL);
	pthread_cond_init(&sim->mon_cond, NULL);
	i = 0;
	while (i < sim->args->nb_coders)
	{
		pthread_cond_init(&sim->coder_wake[i], NULL);
		sim->coders[i].id = i;
		sim->coders[i].sim = sim;
		i++;
	}
	return (0);
}
