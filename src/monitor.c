/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbenhass <jbenhass@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/03 19:41:22 by jbenhass          #+#    #+#             */
/*   Updated: 2026/06/20 18:47:56 by jbenhass         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"
#include <errno.h>

static unsigned long long	next_deadline(t_sim *sim, unsigned int *who)
{
	unsigned int		i;
	unsigned long long	d;
	unsigned long long	best;

	best = (unsigned long long)-1;
	i = 0;
	while (i < sim->args->nb_coders)
	{
		d = sim->coders[i].last_compile_start + sim->args->time_to_burnout;
		if (d < best)
		{
			best = d;
			*who = i;
		}
		i++;
	}
	return (best);
}

static int	all_compiled(t_sim *sim)
{
	unsigned int	i;

	i = 0;
	while (i < sim->args->nb_coders)
	{
		if (sim->coders[i].compile_count < sim->args->nb_compiles_required)
			return (0);
		i++;
	}
	return (1);
}

static void	stop_simulation(t_sim *sim, int coder_id, int burned)
{
	unsigned int	i;

	if (burned)
		log_state(sim, coder_id, "burned out");
	sim->stop = 1;
	pthread_cond_broadcast(&sim->sched_wake);
	i = 0;
	while (i < sim->args->nb_coders)
	{
		pthread_cond_broadcast(&sim->coder_wake[i]);
		i++;
	}
}

void	*monitor_routine(void *arg)
{
	t_sim				*sim;
	unsigned int		who;
	unsigned long long	deadline;
	struct timespec		ts;

	sim = (t_sim *)arg;
	who = 0;
	pthread_mutex_lock(&sim->lock);
	while (!sim->started)
		pthread_cond_wait(&sim->start_cond, &sim->lock);
	while (!sim->stop)
	{
		if (all_compiled(sim))
		{
			stop_simulation(sim, 0, 0);
			break ;
		}
		deadline = next_deadline(sim, &who);
		ts = ms_to_ts(sim->sim_start + deadline);
		if (pthread_cond_timedwait(&sim->mon_cond, &sim->lock, &ts) == ETIMEDOUT
			&& get_ms(sim->sim_start) >= deadline)
			stop_simulation(sim, who + 1, 1);
	}
	pthread_mutex_unlock(&sim->lock);
	return (NULL);
}
