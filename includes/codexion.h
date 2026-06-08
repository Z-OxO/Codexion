/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbenhass <jbenhass@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/31 16:53:21 by jbenhass          #+#    #+#             */
/*   Updated: 2026/06/05 18:17:39 by jbenhass         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#ifndef CODEXION_H
# define CODEXION_H

# include "../includes/parsing.h"
# include <pthread.h>
# include <stdlib.h>
# include <sys/time.h>
# include <unistd.h>

typedef struct s_sim	t_sim;

typedef struct s_dongle
{
	bool				held;
	unsigned long long	available;

}						t_dongle;

typedef struct s_lease
{
	long long			grant_at;
	int					active;
}						t_lease;

typedef struct s_coder
{
	t_sim				*sim;
	bool				need_lease;
	unsigned int		id;
	unsigned int		compile_count;
	unsigned long long	last_compile_start;
	unsigned long long	debug_start;

}						t_coder;

typedef struct s_node
{
	int					id;
	unsigned long long	key;
}						t_node;

typedef struct s_pqueue
{
	t_node				*data;
	int					size;
}						t_pqueue;

typedef struct s_sim
{
	struct s_args		*args;

	bool				stop;
	unsigned long long	sim_start;

	t_coder				*coders;
	pthread_cond_t		*coder_wake;
	bool				*granted;

	t_pqueue			pq;

	t_dongle			*dongles;

	pthread_mutex_t		lock;
	pthread_mutex_t		log_mutex;

	pthread_cond_t		sched_wake;
	pthread_cond_t		mon_cond;

}						t_sim;

// INIT.C
int						clean_sim(t_sim *sim);
int						init_sim(t_sim *sim, t_args *args);

// UTILS.C
unsigned long long		get_ms(unsigned long long sim_start);
void					log_state(t_sim *sim, unsigned long long sim_start,
							int coder_id, const char *msg);

// SCHEDULER.C
void					*scheduler_routine(void *args);

// CODER.C
void					*coder_routine(void *args);

// HEAP.C

#endif
