/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbenhass <jbenhass@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/02 19:09:38 by jbenhass          #+#    #+#             */
/*   Updated: 2026/06/04 14:51:50 by jbenhass         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

unsigned long long	get_ms(unsigned long long sim_start)
{
	unsigned long long	t;
	struct timeval		time;

	gettimeofday(&time, NULL);
	t = time.tv_sec * 1000 + time.tv_usec / 1000;
	return (t - sim_start);
}

void	log_state(t_sim *sim, int coder_id, const char *msg)
{
	unsigned long long	curr_time;

	pthread_mutex_lock(&sim->log_mutex);
	curr_time = get_ms(sim->sim_start);
	printf("%lld %d %s\n", curr_time, coder_id, msg);
	pthread_mutex_unlock(&sim->log_mutex);
}
