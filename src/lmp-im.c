/* vi: set sw=4 ts=4: */
/*
 * lmp-im.c
 *
 * This file is part of lmp-im.
 *
 * Copyright (C) 2010 - kelvenxu <kelvenxu@gmail.com>.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330,
 * Boston, MA 02111-1307, USA.
 * */

#include <gtk/gtk.h>
#include "lmp-im-object.h"

static const GtkIMContextInfo im_info = 
{
	"lmp",
	"Lmp input method",
	"gtk30",
	"/usr/share/locale",
	"zh"
};

static const GtkIMContextInfo *info_list[] = 
{
	  &im_info
};

void im_module_init(GTypeModule *type_module)
{
	lmp_im_window_register_type(type_module);
	lmp_im_object_register_type(type_module);
}

void im_module_exit(void)
{
}

void im_module_list(const GtkIMContextInfo ***contexts, int *n_contexts)
{
	*contexts = info_list;
	*n_contexts = G_N_ELEMENTS(info_list);
}

GtkIMContext *im_module_create(const gchar *context_id)
{
	if(strcmp(context_id, "lmp") == 0)
	{
		return g_object_new(LMP_IM_TYPE_OBJECT, NULL);
	}
	else
	{
		return NULL;
	}
}

