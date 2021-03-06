/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strlen.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: areggie <areggie@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/11/05 21:12:25 by areggie           #+#    #+#             */
/*   Updated: 2022/01/04 21:01:59 by meunostu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

size_t	ft_strlen(const char *s)
{
	const unsigned char		*string;
	int						b;

	if (!s)
		return (0);
	string = (const unsigned char *)s;
	b = 0;
	while (string[b] != '\0')
	{
		b++;
	}
	return (b);
}
