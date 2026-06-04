/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbenhass <jbenhass@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/31 19:43:45 by jbenhass          #+#    #+#             */
/*   Updated: 2026/06/04 19:34:12 by jbenhass         ###   ########lyon.fr   */
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
	free(sim->coders);
	free(sim->coder_wake);
	free(sim->dongles);
	free(sim->granted);
	free(sim->pq.data);
	return (1);
}

// Simplify init_sim condition | 0 error 1 succes
static int	allocate_arrays(t_sim *sim)
{
	sim->coders = malloc(sizeof(t_coder) * sim->args->nb_coders);
	sim->coder_wake = malloc(sizeof(pthread_cond_t) * sim->args->nb_coders);
	sim->dongles = malloc(sizeof(t_dongle) * sim->args->nb_coders);
	sim->granted = malloc(sizeof(int) * sim->args->nb_coders);
	sim->pq.data = malloc(sizeof(t_node) * sim->args->nb_coders);
	if (!sim->coders || !sim->coder_wake || !sim->dongles || !sim->granted
		|| !sim->pq.data)
		return (0);
	memset(sim->coders, 0, sizeof(t_coder) * sim->args->nb_coders);
	memset(sim->dongles, 0, sizeof(t_dongle) * sim->args->nb_coders);
	memset(sim->granted, 0, sizeof(int) * sim->args->nb_coders);
	memset(sim->pq.data, 0, sizeof(t_node) * sim->args->nb_coders);
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
