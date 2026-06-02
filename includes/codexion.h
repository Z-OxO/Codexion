/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbenhass <jbenhass@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/31 16:53:21 by jbenhass          #+#    #+#             */
/*   Updated: 2026/06/02 14:33:35 by jbenhass         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#ifndef CODEXION_H
# define CODEXION_H

# include "../includes/parsing.h"
# include <pthread.h>
# include <stdlib.h>

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
	unsigned int		id;
	unsigned int		compile_count;
	unsigned long long	last_compile_start;
	unsigned long long	debug_start;

}						t_coder;

typedef struct s_sim
{
	struct s_args		*args;

	bool				stop;
	unsigned long long	sim_start;

	t_coder				*coders;
	pthread_cond_t		*coder_wake;

	t_dongle			*dongles;

	pthread_mutex_t		lock;
	pthread_mutex_t log_mutex; // USE WRITE ?

	pthread_cond_t		sched_wake;
	pthread_cond_t		mon_cond;

}						t_sim;

// INIT.C
void					clean_sim(t_sim *sim);
int						init_sim(t_sim *sim, t_args *args);

#endif
