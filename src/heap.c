/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heap.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbenhass <jbenhass@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/03 19:13:25 by jbenhass          #+#    #+#             */
/*   Updated: 2026/06/20 18:47:52 by jbenhass         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

// (Min-Heap)
static int	has_priority(t_node a, t_node b)
{
	if (a.key != b.key)
		return (a.key < b.key);
	return (a.id < b.id);
}

void	pq_push(t_pqueue *pq, int id, unsigned long long key)
{
	t_node	target;
	int		parent;
	int		i;

	target = (t_node){id, key};
	i = pq->size;
	pq->size++;
	while (i > 0)
	{
		parent = (i - 1) / 2;
		if (has_priority(target, pq->data[parent]))
		{
			pq->data[i] = pq->data[parent];
			i = parent;
		}
		else
			break ;
	}
	pq->data[i] = target;
}

static int	get_best_child(t_pqueue *pq, int i, t_node target)
{
	int	left;
	int	right;
	int	best;

	left = 2 * i + 1;
	right = 2 * i + 2;
	best = i;
	if (left < pq->size && has_priority(pq->data[left], target))
		best = left;
	if (right < pq->size && has_priority(pq->data[right], pq->data[best]))
		best = right;
	return (best);
}

void	pq_pop(t_pqueue *pq)
{
	t_node	target;
	int		i;
	int		best;

	if (pq->size <= 0)
		return ;
	target = pq->data[--pq->size];
	i = 0;
	while (1)
	{
		best = get_best_child(pq, i, target);
		if (best == i)
			break ;
		pq->data[i] = pq->data[best];
		i = best;
	}
	pq->data[i] = target;
}

t_node	*pq_peek(t_pqueue *pq)
{
	if (!pq->data)
		return (NULL);
	return (&pq->data[0]);
}
