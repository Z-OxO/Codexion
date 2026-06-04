/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   scheduler.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbenhass <jbenhass@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/02 16:40:15 by jbenhass          #+#    #+#             */
/*   Updated: 2026/06/03 21:38:37 by jbenhass         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	*scheduler_routine(void *args)
{
	t_sim	*sim;

	sim = (t_sim *)args;
	if (!sim)
		return (NULL);
	printf("Scheduler wake coder 0\n");
	pthread_mutex_lock(&sim->lock);
	sim->granted[0] = 1;
	pthread_cond_signal(&sim->coder_wake[0]);
	pthread_mutex_unlock(&sim->lock);
	return (NULL);
}
