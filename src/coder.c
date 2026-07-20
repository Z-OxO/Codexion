/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbenhass <jbenhass@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/03 19:41:22 by jbenhass          #+#    #+#             */
/*   Updated: 2026/07/20 10:00:12 by jbenhass         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	release_dongles(t_sim *sim, t_coder *coder)
{
	int					l_dongle;
	int					r_dongle;
	unsigned long long	avail;

	l_dongle = coder->id;
	r_dongle = (coder->id + 1) % sim->args->nb_coders;
	pthread_mutex_lock(&sim->lock);
	avail = get_ms(sim->sim_start) + sim->args->dongle_cooldown;
	sim->dongles[l_dongle].available = avail;
	sim->dongles[r_dongle].available = avail;
	pthread_cond_broadcast(&sim->sched_wake);
	pthread_mutex_unlock(&sim->lock);
}

static void	do_compile(t_sim *sim, t_coder *coder)
{
	log_state(sim, coder->id + 1, "has taken a dongle");
	log_state(sim, coder->id + 1, "has taken a dongle");
	log_state(sim, coder->id + 1, "is compiling");
	usleep(sim->args->time_to_compile * 1000);
	pthread_mutex_lock(&sim->lock);
	coder->compile_count++;
	pthread_cond_broadcast(&sim->mon_cond);
	pthread_mutex_unlock(&sim->lock);
}

void	wait_for_dongles(t_sim *sim, t_coder *coder)
{
	pthread_mutex_lock(&sim->lock);
	if (sim->args->scheduler_type == FIFO)
		pq_push(&sim->pq, coder->id, get_ms(sim->sim_start));
	else
		pq_push(&sim->pq, coder->id, coder->last_compile_start
			+ sim->args->time_to_burnout);
	pthread_cond_broadcast(&sim->sched_wake);
	while (!sim->granted[coder->id] && !sim->stop)
		pthread_cond_wait(&sim->coder_wake[coder->id], &sim->lock);
	if (sim->stop)
	{
		pthread_mutex_unlock(&sim->lock);
		return ;
	}
	sim->granted[coder->id] = 0;
	pthread_mutex_unlock(&sim->lock);
	do_compile(sim, coder);
}

static void	*single_coder(t_sim *sim, t_coder *coder)
{
	log_state(sim, coder->id + 1, "has taken a dongle");
	pthread_mutex_lock(&sim->lock);
	while (!sim->stop)
		pthread_cond_wait(&sim->coder_wake[coder->id], &sim->lock);
	pthread_mutex_unlock(&sim->lock);
	return (NULL);
}

void	*coder_routine(void *args)
{
	t_coder	*coder;
	t_sim	*sim;
	bool	stop_coder;

	coder = (t_coder *)args;
	sim = coder->sim;
	wait_start(sim);
	if (sim->args->nb_coders == 1)
		return (single_coder(sim, coder));
	while (1)
	{
		wait_for_dongles(sim, coder);
		pthread_mutex_lock(&sim->lock);
		stop_coder = sim->stop;
		pthread_mutex_unlock(&sim->lock);
		if (stop_coder)
			break ;
		release_dongles(sim, coder);
		log_state(sim, coder->id + 1, "is debugging");
		usleep(sim->args->time_to_debug * 1000);
		log_state(sim, coder->id + 1, "is refactoring");
		usleep(sim->args->time_to_refactor * 1000);
	}
	return (NULL);
}
