/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbenhass <jbenhass@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/03 19:41:22 by jbenhass          #+#    #+#             */
/*   Updated: 2026/06/04 14:49:42 by jbenhass         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	*coder_routine(void *args)
{
	t_coder	*coder;
	t_sim	*sim;

	coder = (t_coder *)args;
	sim = coder->sim;
	pthread_mutex_lock(&sim->lock);
	while (!sim->granted[coder->id] && !sim->stop)
		pthread_cond_wait(&sim->coder_wake[coder->id], &sim->lock);
	log_state(sim, coder->id + 1, "is compiling");
	usleep(sim->args->time_to_compile);
	log_state(sim, coder->id + 1, " is refactoring");
	usleep(sim->args->time_to_refactor);
	log_state(sim, coder->id + 1, " is debugging");
	usleep(sim->args->dongle_cooldown);
	pthread_mutex_unlock(&sim->lock);
	return (NULL);
}
