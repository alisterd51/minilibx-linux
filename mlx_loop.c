/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mlx_loop.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: Charlie Root <ol@epitech.net>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2000/08/02 18:58:11 by Charlie Root      #+#    #+#             */
/*   Updated: 2021/11/01 13:46:39 by anclarma         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "mlx_int.h"

extern void	(*(mlx_int_param_event[]))();

static int	win_count(t_xvar *xvar)
{
	int			i;
	t_win_list	*win;

	i = 0;
	win = xvar->win_list;
	while (win)
	{
		win = win->next;
		++i;
	}
	return (i);
}

int	mlx_loop_end(t_xvar *xvar)
{
	xvar->end_loop = 1;
	return (1);
}

int	mlx_loop(t_xvar *xvar)
{
	XEvent		ev;
	t_win_list	*win;

	mlx_int_set_win_event_mask(xvar);
	xvar->do_flush = 0;
	while (win_count(xvar) && !xvar->end_loop)
	{
		while (!xvar->end_loop && (!xvar->loop_hook || XPending(xvar->display)))
		{
			XNextEvent(xvar->display, &ev);
			win = xvar->win_list;
			while (win && (win->window != ev.xany.window))
				win = win->next;
			if (win
				&& ev.type == ClientMessage
				&& ev.xclient.message_type == xvar->wm_protocols
				&& (unsigned long)ev.xclient.data.l[0] == xvar->wm_delete_window
				&& win->hooks[DestroyNotify].hook)
				win->hooks[DestroyNotify].hook(win->hooks[DestroyNotify].param);
			if (win && ev.type < MLX_MAX_EVENT && win->hooks[ev.type].hook)
				mlx_int_param_event[ev.type](xvar, &ev, win);
		}
		XSync(xvar->display, False);
		xvar->loop_hook(xvar->loop_param);
	}
	return (0);
}
