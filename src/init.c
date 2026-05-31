/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbenhass <jbenhass@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/31 19:43:45 by jbenhass          #+#    #+#             */
/*   Updated: 2026/05/31 22:07:08 by jbenhass         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/codexion.h"

void	clean_sim(t_sim *sim)
{
	unsigned int	i;

	if (!sim)
		return ;
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
}
static int	allocate_arrays(t_sim *sim)
{
	sim->coders = malloc(sizeof(t_coder) * sim->args->nb_coders);
	sim->coder_wake = malloc(sizeof(pthread_cond_t) * sim->args->nb_coders);
	if (!sim->coders || !sim->coder_wake)
		return (0); // Simplify init_sim condition | 0 error 1 succes
	return (1);
}

int	init_sim(t_sim *sim, t_args *args)
{
	unsigned int	i;

	if (!sim || !args)
		return (1);
	sim->args = args;
	if (!allocate_arrays(sim))
	{
		clean_sim(sim);
		return (1);
	}
	pthread_mutex_init(&sim->lock, NULL);
	pthread_mutex_init(&sim->log_mutex, NULL);
	pthread_cond_init(&sim->sched_wake, NULL);
	pthread_cond_init(&sim->mon_cond, NULL);
	memset(sim->coders, 0, sizeof(t_coder) * sim->args->nb_coders);
	i = 0;
	while (i < sim->args->nb_coders)
	{
		pthread_cond_init(&sim->coder_wake[i], NULL);
		sim->coders[i].id = i + 1; // Start at 1
		sim->coders[i].sim = sim;
		i++;
	}
	return (0);
}
