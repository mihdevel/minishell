/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strjoin.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: areggie <areggie@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/10/30 15:01:51 by meunostu          #+#    #+#             */
/*   Updated: 2022/01/04 21:01:59 by meunostu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_strjoin(char *s1, char *s2)
{
	size_t		len_s1;
	size_t		len_s2;
	char		*cat_s;
	char		*start_s1;
	char		*start_cat_s;

	if (!s2)
		return (NULL);
	len_s1 = ft_strlen(s1);
	len_s2 = ft_strlen(s2);
	cat_s = malloc(sizeof(char) * (len_s1 + len_s2 + 1));
	if (!cat_s)
		return (NULL);
	start_cat_s = cat_s;
	start_s1 = s1;
	while (s1 && *s1)
		*cat_s++ = *s1++;
	while (s2 && *s2)
		*cat_s++ = *s2++;
	*cat_s = '\0';
	return (start_cat_s);
}
