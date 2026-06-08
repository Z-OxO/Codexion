/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   scheduler.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbenhass <jbenhass@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/02 16:40:15 by jbenhass          #+#    #+#             */
/*   Updated: 2026/06/05 18:55:41 by jbenhass         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	*scheduler_routine(void *args)
{
	t_sim	*sim;

	sim = (t_sim *)args;
	if (!sim)
		return (NULL);
	while (!sim->stop)
	{
		if (sim->coders[0].compile_count > 1)
		{
			pthread_mutex_lock(&sim->lock);
			sim->stop = 1;
			pthread_mutex_unlock(&sim->lock);
			break ;
		}
		pthread_mutex_lock(&sim->lock);
		if (!sim->sim_start)
			sim->sim_start = get_ms(0);
		sim->granted[0] = 1;
		pthread_cond_signal(&sim->coder_wake[0]);
		pthread_cond_wait(&sim->sched_wake, &sim->lock);
		pthread_mutex_unlock(&sim->lock);
	}
	return (NULL);
}
