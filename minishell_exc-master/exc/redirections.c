/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirections.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: otaraki <otaraki@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/01 01:35:38 by otaraki           #+#    #+#             */
/*   Updated: 2023/09/16 12:32:58 by otaraki          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/minishell.h"

int here_doc(int *fdin, char *str)
{
	char	*rd;
	(void)str;
	
	*fdin = open("/tmp/here_doc", O_RDWR | O_CREAT | O_TRUNC, 0654);
	rd = NULL;
	if (*fdin < 0)
		return -1;
	while (1)
	{
		// ft_putstr_fd("> ", 1);
		rd = readline(">");
		// printf("----%s----\n", rd);
		if (!rd || (!ft_strcmp(rd, str)))
			break;
		rd = ft_strjoin(rd, "\n");
		ft_putstr_fd(rd, *fdin);
		free(rd);
	}
	// printf("EXITING HEREDOC\n");
	free(rd);
	close(*fdin);
	return 0;
}

int	append(int *fdout, char *strout)
{
	char	*line;

	*fdout = open(strout, O_APPEND | O_WRONLY | O_CREAT, 0654);
	if (*fdout < 0)
		return -1;
	while (1)
	{
		line = readline("");
		if (!line)
			break ;
		line = ft_strjoin(line, "\n");
		ft_putstr_fd(line, *fdout);
		free(line);
	}
	free(line);
	close(*fdout);
	return 0;
}

int	red_open(int *fds, t_type red, char *f_name)
{
	if (red == GREAT)
	{
		*fds = open(f_name, O_WRONLY | O_CREAT | O_TRUNC, 0654);
		if (*fds < 0)
			return (-1);
	}
	else if (red == LESS)
	{
		*fds = open(f_name, O_RDONLY, 0654);
		if (*fds < 0)
			return (-1);
	}
	else if (red == APPEND)// still working on it
	{
		*fds = open(f_name, O_APPEND | O_CREAT, 0654);
		if (*fds < 0)
			return (-1);
	}
	else if (red == HERE_DOC)
	{
		*fds = open(f_name, O_RDONLY, 0654);
		if (*fds < 0)
			return (-1);
	}
	return (0);
}


void	open_red(t_token **data, char **cmds,  t_env **env)
{
	size_t i;
	int status; // close all file descriptors in the parent and child process

	i = 0;
	(void)env;
	while(cmds[i])
	{
		if (!ft_strcmp(cmds[i], ">"))
		{
			printf("FOUND IN\n");
			status = red_open(&(*data)->fdout, GREAT, cmds[i + 1]);
			free(cmds[i]);
			cmds[i] = NULL;
		}
		else if (!ft_strcmp(cmds[i], "<"))
		{
			status = red_open(&(*data)->fdin, LESS, cmds[i + 1]);
			free(cmds[i]);
			cmds[i] = NULL;
		}
		else if (!ft_strcmp(cmds[i], ">>"))
		{
			status = red_open(&(*data)->fdout, APPEND, cmds[i + 1]);
			free(cmds[i]);
			cmds[i] = NULL;
		}
		else if (!ft_strcmp(cmds[i], "<<"))
		{
			here_doc(&(*data)->fdin ,cmds[i + 1]);
			status = red_open(&(*data)->fdin, HERE_DOC, "/tmp/here_doc");
			free(cmds[i]);
			cmds[i] = NULL;
		}
		if (status < 0)
		{
			printf("EXITING PROCESS\n");
			exit(0);// check if theere is an error > error handing
		}
		++i;
	}
}