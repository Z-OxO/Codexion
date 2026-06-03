/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heap.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbenhass <jbenhass@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/03 17:09:06 by jbenhass          #+#    #+#             */
/*   Updated: 2026/06/03 17:35:37 by jbenhass         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	has_priority(t_node a, t_node b)
{
	if (a.key != b.key)
		return (a.key < b.key);
	return (a.id < b.id);
}
static void	swap_node(t_pqueue *pq, int a, int b)
{
	t_node	tmp;

	tmp = pq->data[a];
	pq->data[a] = pq->data[b];
	pq->data[b] = tmp;
}

void	pq_push(t_pqueue *pq, int id, unsigned long long key)
{
	int	parent;
	int	i;

	pq->data[pq->size] = (t_node){id, key};
	i = pq->size;
	pq->size++;
	while (i > 0)
	{
		parent = (i - 1) / 2;
		if (has_priority(pq->data[i], pq->data[parent]))
		{
			swap_node(pq, parent, i);
			i = parent;
		}
		else
			break ;
	}
}

void	pq_pop(t_pqueue *pq)
{
	int	i;
	int	left;
	int	right;
	int	best;

	if (pq->size <= 0)
		return ;
	pq->size--;
	pq->data[0] = pq->data[pq->size];
	i = 0;
	while (1)
	{
		left = 2 * i + 1;
		right = 2 * i + 2;
		best = i;
		if (left < pq->size && has_priority(pq->data[left], pq->data[best]))
			best = left;
		if (right < pq->size && has_priority(pq->data[right], pq->data[best]))
			best = right;
		if (best == i)
			break ;
		swap_nodes(pq, i, best);
		i = best;
	}
}
