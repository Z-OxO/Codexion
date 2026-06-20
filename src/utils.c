/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbenhass <jbenhass@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/02 19:09:38 by jbenhass          #+#    #+#             */
/*   Updated: 2026/06/20 18:48:01 by jbenhass         ###   ########lyon.fr   */
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

void	*ft_calloc(size_t count, size_t size)
{
	void	*ptr;

	ptr = malloc(count * size);
	if (!ptr)
		return (NULL);
	memset(ptr, 0, count * size);
	return (ptr);
}

void	log_state(t_sim *sim, int coder_id, const char *msg)
{
	unsigned long long	curr_time;

	curr_time = get_ms(sim->sim_start);
	pthread_mutex_lock(&sim->log_mutex);
	printf("%llu %d %s\n", curr_time, coder_id, msg);
	pthread_mutex_unlock(&sim->log_mutex);
}

struct timespec	ms_to_ts(unsigned long long ms)
{
	struct timespec	ts;

	ts.tv_sec = ms / 1000;
	ts.tv_nsec = (ms % 1000) * 1000000;
	return (ts);
}
