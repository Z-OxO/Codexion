/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbenhass <jbenhass@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/02 19:09:38 by jbenhass          #+#    #+#             */
/*   Updated: 2026/06/08 15:04:11 by jbenhass         ###   ########lyon.fr   */
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

void	log_state(t_sim *sim, unsigned long long sim_start, int coder_id,
		const char *msg)
{
	unsigned long long	curr_time;

	curr_time = get_ms(sim_start);
	pthread_mutex_lock(&sim->log_mutex);
	printf("%lld %d %s\n", curr_time, coder_id, msg);
	pthread_mutex_unlock(&sim->log_mutex);
}
