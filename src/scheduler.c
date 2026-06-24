/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   scheduler.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbenhass <jbenhass@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/03 19:41:22 by jbenhass          #+#    #+#             */
/*   Updated: 2026/06/24 20:10:07 by jbenhass         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	grantable(t_sim *s, int id)
{
	int					r;
	unsigned long long	now;

	r = (id + 1) % s->args->nb_coders;
	now = get_ms(s->sim_start);
	if (now < s->dongles[id].available)
		return (0);
	if (now < s->dongles[r].available)
		return (0);
	return (1);
}

static void	grant(t_sim *s, int id)
{
	int					r;
	unsigned long long	busy;

	r = (id + 1) % s->args->nb_coders;
	busy = get_ms(s->sim_start) + s->args->time_to_compile
		+ s->args->dongle_cooldown;
	s->dongles[id].available = busy;
	s->dongles[r].available = busy;
	s->granted[id] = 1;
	pthread_cond_signal(&s->coder_wake[id]);
}

static void	schedule_pass(t_sim *s)
{
	int		ndef;
	t_node	node;

	ndef = 0;
	while (s->pq.size > 0)
	{
		node = *pq_peek(&s->pq);
		pq_pop(&s->pq);
		if (grantable(s, node.id))
			grant(s, node.id);
		else
			s->backfill[ndef++] = node;
	}
	while (ndef > 0)
	{
		ndef--;
		pq_push(&s->pq, s->backfill[ndef].id, s->backfill[ndef].key);
	}
}

static void	sched_sleep(t_sim *s)
{
	int					i;
	int					r;
	unsigned long long	ready;
	unsigned long long	best;
	struct timespec		ts;

	best = (unsigned long long)-1;
	i = 0;
	while (i < s->pq.size)
	{
		r = (s->pq.data[i].id + 1) % s->args->nb_coders;
		ready = s->dongles[s->pq.data[i].id].available;
		if (s->dongles[r].available > ready)
			ready = s->dongles[r].available;
		if (ready < best)
			best = ready;
		i++;
	}
	ts = ms_to_ts(s->sim_start + best);
	pthread_cond_timedwait(&s->sched_wake, &s->lock, &ts);
}


void	*scheduler_routine(void *args)
{
	t_sim	*s;

	s = (t_sim *)args;
	pthread_mutex_lock(&s->lock);
	while (!s->started)
		pthread_cond_wait(&s->start_cond, &s->lock);
	while (!s->stop)
	{
		if (s->pq.size == 0)
			pthread_cond_wait(&s->sched_wake, &s->lock);
		else
		{
			schedule_pass(s);
			if (s->pq.size > 0)
				sched_sleep(s);
		}
	}
	pthread_mutex_unlock(&s->lock);
	return (NULL);
}
