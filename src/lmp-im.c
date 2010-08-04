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

static GtkIMContextInfo *info;

void im_module_init(GTypeModule *type_module)
{
	fprintf(stderr, "%s : %s\n", __FILE__, __func__);

	lmp_im_window_register_type(type_module);
	lmp_im_object_register_type(type_module);
}

void im_module_exit(void)
{
	fprintf(stderr, "%s : %s\n", __FILE__, __func__);

	if(info != NULL) 
	{
		g_free((void *)info->context_id);
		g_free((void *)info->context_name);
		g_free((void *)info->domain);
		g_free((void *)info->domain_dirname);
		g_free((void *)info->default_locales);
		g_free(info);
		info = NULL;
	}
}

void im_module_list(const GtkIMContextInfo ***contexts, int *n_contexts)
{
	fprintf(stderr, "%s : %s\n", __FILE__, __func__);

	info = g_new(GtkIMContextInfo, 1);
	info->context_id = g_strdup("lmp-im");
	info->context_name = g_strdup("Little Pudding Input Method");
	info->domain = g_strdup("gtk+");
	info->domain_dirname = g_strdup("");
	info->default_locales = g_strdup("");

	*contexts = (const GtkIMContextInfo **)&info;
	*n_contexts = 1;
}

GtkIMContext *im_module_create(const gchar *context_id)
{
	fprintf(stderr, "%s : %s\n", __FILE__, __func__);


	if((context_id == NULL) || (context_id[0] == '\0')) 
	{
		return NULL;
	}

	LmpIMObject *im = g_object_new(LMP_IM_TYPE_OBJECT, NULL);
	//GTKIMCONTEXTTIM_GET_CLASS(im)->set(im, context_id);

	return GTK_IM_CONTEXT(im);
}

