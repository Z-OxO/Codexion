/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbenhass <jbenhass@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/03 19:41:22 by jbenhass          #+#    #+#             */
/*   Updated: 2026/06/08 15:02:42 by jbenhass         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	release_dongles(t_sim *sim, t_coder *coder)
{
	int					l_dongle;
	int					r_dongle;
	unsigned long long	now;
	unsigned long long	avail;

	l_dongle = coder->id;
	r_dongle = (coder->id + 1) % sim->args->nb_coders;
	now = get_ms(sim->sim_start);
	avail = now + sim->args->dongle_cooldown;
	pthread_mutex_lock(&sim->lock);
	sim->dongles[l_dongle].held = 0;
	sim->dongles[r_dongle].held = 0;
	sim->dongles[l_dongle].available = avail;
	sim->dongles[r_dongle].available = avail;
	coder->need_lease = 1;
	coder->debug_start = get_ms(sim->sim_start);
	pthread_cond_signal(&sim->sched_wake);
	pthread_mutex_unlock(&sim->lock);
}

void	wait_for_dongles(t_sim *sim, t_coder *coder)
{
	pthread_mutex_lock(&sim->lock);
	while (!sim->granted[coder->id] && !sim->stop)
		pthread_cond_wait(&sim->coder_wake[coder->id], &sim->lock);
	if (sim->stop)
	{
		pthread_mutex_unlock(&sim->lock);
		return ;
	}
	sim->granted[coder->id] = 0;
	pthread_mutex_unlock(&sim->lock);
	log_state(sim, sim->sim_start, coder->id + 1, " is compiling");
	coder->last_compile_start = get_ms(sim->sim_start);
	usleep(sim->args->time_to_compile * 1000);
	coder->compile_count++;
}

void	*coder_routine(void *args)
{
	t_coder	*coder;
	t_sim	*sim;

	coder = (t_coder *)args;
	sim = coder->sim;
	while (1)
	{
		wait_for_dongles(sim, coder);
		if (sim->stop)
			break ;
		release_dongles(sim, coder);
		log_state(sim, sim->sim_start, coder->id + 1, " is refactoring");
		usleep(sim->args->time_to_refactor * 1000);
		log_state(sim, sim->sim_start, coder->id + 1, " is debugging");
		usleep(sim->args->dongle_cooldown * 1000);
	}
	pthread_mutex_lock(&sim->lock);
	pthread_cond_signal(&sim->sched_wake);
	pthread_mutex_unlock(&sim->lock);
	return (NULL);
}
