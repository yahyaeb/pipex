/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yel-bouk <yel-bouk@student.42nice.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/16 18:55:38 by yel-bouk          #+#    #+#             */
/*   Updated: 2025/02/17 12:21:50 by yel-bouk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex.h"

int main(int argc, char*argv[])
{
	if(argc == 2)
	{
		int id = fork();
		if(id == 0)
		{
			int id1 = fork();
			if(id1 == 0)
				execlp(argv[1], "ping", "-c", "3", "google.com", NULL);
			else
			{
				ft_printf("I am going to wait\n");
				wait(NULL);
			}
		}
		else
		{
			wait(NULL);
			ft_printf("I guess my child just finished running the following command\n CIAO!\n");
	
		}
	}
	else
		ft_printf("\n");

	return 0;
}