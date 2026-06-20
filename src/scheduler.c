/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   scheduler.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbenhass <jbenhass@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/03 19:41:22 by jbenhass          #+#    #+#             */
/*   Updated: 2026/06/20 18:47:58 by jbenhass         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	try_grant_head(t_sim *sim)
{
	t_node				*lease;
	int					id;
	int					r_id;
	unsigned long long	now;
	unsigned long long	busy;

	lease = pq_peek(&sim->pq);
	id = lease->id;
	r_id = (id + 1) % sim->args->nb_coders;
	now = get_ms(sim->sim_start);
	if (now < sim->dongles[id].available || now < sim->dongles[r_id].available)
		return (0);
	busy = now + sim->args->time_to_compile + sim->args->dongle_cooldown;
	sim->dongles[id].available = busy;
	sim->dongles[r_id].available = busy;
	sim->granted[id] = 1;
	pq_pop(&sim->pq);
	pthread_cond_signal(&sim->coder_wake[id]);
	return (1);
}

static void	wait_until_ready(t_sim *sim)
{
	t_node				*lease;
	int					r_id;
	unsigned long long	target;
	unsigned long long	abs_ms;
	struct timespec		ts;

	lease = pq_peek(&sim->pq);
	r_id = (lease->id + 1) % sim->args->nb_coders;
	target = sim->dongles[lease->id].available;
	if (sim->dongles[r_id].available > target)
		target = sim->dongles[r_id].available;
	abs_ms = sim->sim_start + target;
	ts = ms_to_ts(abs_ms);
	pthread_cond_timedwait(&sim->sched_wake, &sim->lock, &ts);
}

void	*scheduler_routine(void *args)
{
	t_sim	*sim;

	sim = (t_sim *)args;
	if (!sim)
		return (NULL);
	pthread_mutex_lock(&sim->lock);
	while (!sim->started)
		pthread_cond_wait(&sim->start_cond, &sim->lock);
	while (!sim->stop)
	{
		if (sim->pq.size == 0)
			pthread_cond_wait(&sim->sched_wake, &sim->lock);
		else if (!try_grant_head(sim))
			wait_until_ready(sim);
	}
	pthread_mutex_unlock(&sim->lock);
	return (NULL);
}
