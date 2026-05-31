/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbenhass <jbenhass@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/29 02:20:56 by jbenhass          #+#    #+#             */
/*   Updated: 2026/05/29 05:01:40 by jbenhass         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/parsing.h"

int	main(int argc, const char **argv)
{
	t_args				args;
	t_parsing_errors	ret;

	ret = parse_args(argc, argv, &args);
	if (ret != OK)
		return (print_error(ret));
	printf("nb coders: %lu\n", args.nb_coders);
	return (OK);
}
