/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exc.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: otaraki <otaraki@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/27 21:05:55 by otaraki           #+#    #+#             */
/*   Updated: 2023/09/17 17:06:14 by otaraki          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/minishell.h"

char *check_path(char **s_path, char *cmd)
{
	int i;
	char *j_cmd;
	char *j_path;

	i = 0;
	if (access(cmd, X_OK) == 0)
		return (cmd);
	j_cmd = ft_strjoin("/", cmd);
	if (!j_cmd)
		return (NULL);
	while (s_path[i])
	{
		j_path = ft_strjoin(s_path[i], j_cmd);
		if (access(j_path, X_OK) == 0)
		{
			free(j_cmd);
			return (j_path);
		}
		free(j_path);
		i++;
	}
	free(j_cmd);
	return (NULL);
}

void excute_one_cmd(t_token **args, char **contents, t_env **env)
{
	char *path;
	char *str;
	char **splited_path;

	(void)args;
	if (!contents[0])
		return;
	else
	{
		path = value_by_key(*env, "PATH");
		if (path == NULL)
			return; // error handel : PATH NOT FOUND;(to add)
		splited_path = ft_split(path, ':');
		str = check_path(splited_path, contents[0]);
		if (!str)
			return;									 // error handel
		execve(str, contents, get_normal_env(*env)); // check case of failure
	}
}

void one_cmd(t_token **cmds, char **args, t_env **env)
{
	int save;
	int out;

	if (!args[0])
		return ;
	out = dup(1);
	if (is_bult_in(args[0]) == BULT_IN)
	{
		if ((*cmds)->fdin != 0)
		{
			dup2((*cmds)->fdin, STDIN_FILENO);
			close((*cmds)->fdin);
		}

		if ((*cmds)->fdout != 1)
		{
			dup2((*cmds)->fdout, STDOUT_FILENO);
			close((*cmds)->fdout);
		}
		save = ft_bultin(*cmds, args, env);
		dup2(out, STDOUT_FILENO);
	}
	else
		multi_cmd(cmds, env);
}

void exceute_it(t_token **data, t_env **env)
{
	t_token *iter;
	int numb_pipes;

	iter = *data;
	numb_pipes = -1;
	while (iter)
	{
		open_red(&iter, iter->content, env);
		numb_pipes++;
		iter = iter->forward;
	}
	if (numb_pipes == 0)
		one_cmd(data, (*data)->content, env);
	else
		multi_cmd(data, env);
}