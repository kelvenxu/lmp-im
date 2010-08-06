/* vi: set sw=4 ts=4: */
/*
 * lmp-im-window.c
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

#include "lmp-im-window.h"
#include "type.h"

//G_DEFINE_TYPE(LmpIMWindow, lmp_im_window, GTK_TYPE_WINDOW);

#define LMP_IM_WINDOW_GET_PRIVATE(o)\
	(G_TYPE_INSTANCE_GET_PRIVATE((o), LMP_TYPE_IM_WINDOW, LmpIMWindowPrivate))

struct _LmpIMWindowPrivate 
{
	GtkWidget *frame;
	GtkWidget *vbox;
	GtkWidget *code_label;
	GtkWidget *cand_label;
	GtkWidget* sepatator;

	GString *code_str;
	GString *cand_str;

	GPtrArray *cand_arr;

	gint cand_page;
};

enum
{
	CANDIDATE_NUM = 10,
};

static void
lmp_im_window_dispose(LmpIMWindow *self)
{
}

static void
lmp_im_window_finalize(LmpIMWindow *self)
{
}

static void
lmp_im_window_init(LmpIMWindow *self)
{
	LmpIMWindowPrivate *priv = LMP_IM_WINDOW_GET_PRIVATE(self);
	priv->frame = gtk_frame_new(NULL);
	priv->vbox = gtk_vbox_new(FALSE, 0);
	priv->code_label = gtk_label_new("");
	priv->cand_label = gtk_label_new("");

	priv->sepatator = gtk_hseparator_new();
	gtk_box_pack_start(GTK_BOX(priv->vbox), priv->code_label, TRUE, TRUE, 4);
	gtk_box_pack_start(GTK_BOX(priv->vbox), priv->sepatator, TRUE, TRUE, 4);
	gtk_box_pack_start(GTK_BOX(priv->vbox), priv->cand_label, TRUE, TRUE, 4);

	//gtk_container_add(GTK_CONTAINER(self), priv->vbox);
	gtk_container_add(GTK_CONTAINER(self), priv->frame);
	gtk_container_add(GTK_CONTAINER(priv->frame), priv->vbox);

	gtk_misc_set_alignment(GTK_MISC(priv->code_label), 0, 0.5);
	gtk_misc_set_alignment(GTK_MISC(priv->cand_label), 0, 0.5);

	gtk_label_set_single_line_mode(GTK_LABEL(priv->code_label), TRUE);
	gtk_label_set_single_line_mode(GTK_LABEL(priv->cand_label), TRUE);

	gtk_widget_show(priv->frame);
	gtk_widget_show(priv->vbox);
	gtk_widget_show(priv->code_label);
	gtk_widget_show(priv->cand_label);
	gtk_widget_show(priv->sepatator);


	priv->code_str = g_string_new("");
	priv->cand_str = g_string_new("");
	priv->cand_arr = NULL;
}

static void
lmp_im_window_class_init(LmpIMWindowClass *self_class)
{
	GObjectClass *object_class = G_OBJECT_CLASS(self_class);

	g_type_class_add_private(self_class, sizeof(LmpIMWindowPrivate));
	object_class->dispose = (void (*)(GObject *object))lmp_im_window_dispose;
	object_class->finalize = (void (*)(GObject *object))lmp_im_window_finalize;
}

GtkWidget *
lmp_im_window_new()
{
	return GTK_WIDGET(g_object_new(LMP_TYPE_IM_WINDOW, "type", GTK_WINDOW_POPUP, NULL));
}

void
lmp_im_window_append_code_char(LmpIMWindow *self, gchar ch)
{
	LmpIMWindowPrivate *priv = LMP_IM_WINDOW_GET_PRIVATE(self);

	g_string_append_c(priv->code_str, ch);
	gtk_label_set_text(GTK_LABEL(priv->code_label), priv->code_str->str);

	lmp_im_window_clear_candidate(self);
}

void
lmp_im_window_clear_code(LmpIMWindow *self)
{
	LmpIMWindowPrivate *priv = LMP_IM_WINDOW_GET_PRIVATE(self);

	gtk_label_set_text(GTK_LABEL(priv->code_label), "");
	g_string_erase(priv->code_str, 0, -1);
}

const gchar *
lmp_im_window_get_code(LmpIMWindow *self)
{
	LmpIMWindowPrivate *priv = LMP_IM_WINDOW_GET_PRIVATE(self);

	if(priv->code_str && priv->code_str->len > 0)
		return priv->code_str->str;
	else
		return NULL;
}

gboolean
lmp_im_window_has_code(LmpIMWindow *self)
{
	LmpIMWindowPrivate *priv = LMP_IM_WINDOW_GET_PRIVATE(self);

	return priv->code_str && (priv->code_str->len > 0);
}

gint 
lmp_im_window_backspace(LmpIMWindow *self)
{
	LmpIMWindowPrivate *priv = LMP_IM_WINDOW_GET_PRIVATE(self);

	if(priv->code_str->len > 0)
	{
		g_string_erase(priv->code_str, priv->code_str->len - 1, 1);
		gtk_label_set_text(GTK_LABEL(priv->code_label), priv->code_str->str);

		lmp_im_window_clear_candidate(self);
	}

	return priv->code_str->len;
}

void 
lmp_im_window_set_candidate(LmpIMWindow *self, GPtrArray *arr)
{
	LmpIMWindowPrivate *priv = LMP_IM_WINDOW_GET_PRIVATE(self);

	priv->cand_arr = arr;
	
	gint i = 0;
	
	for(i = 0; i < arr->len && i < CANDIDATE_NUM; ++i)
	{
		CodeInfo *info = g_ptr_array_index(arr, i);
		g_string_append_printf(priv->cand_str, "%d. %s", i, info->chinese);
	}

	gtk_label_set_text(GTK_LABEL(priv->cand_label), priv->cand_str->str);

	priv->cand_page = 0;
}

const gchar *
lmp_im_window_candidate_index(LmpIMWindow *self, gint index)
{
	LmpIMWindowPrivate *priv = LMP_IM_WINDOW_GET_PRIVATE(self);

	gint id = index + priv->cand_page * CANDIDATE_NUM;
	if(priv->cand_arr && index >= 0 && index < priv->cand_arr->len)
	{
		CodeInfo *info = g_ptr_array_index(priv->cand_arr, id);
		return info->chinese;
	}

	return NULL;
}

gboolean 
lmp_im_window_has_candidate(LmpIMWindow *self)
{
	g_return_val_if_fail(LMP_IS_IM_WINDOW(self), FALSE);

	LmpIMWindowPrivate *priv = LMP_IM_WINDOW_GET_PRIVATE(self);

	return (priv->cand_arr) && (priv->cand_arr->len > 0);
}

void
lmp_im_window_clear_candidate(LmpIMWindow *self)
{
	LmpIMWindowPrivate *priv = LMP_IM_WINDOW_GET_PRIVATE(self);

	gtk_label_set_text(GTK_LABEL(priv->cand_label), "");

	g_string_erase(priv->cand_str, 0, -1);

	if(priv->cand_arr)
	{
		g_ptr_array_free(priv->cand_arr, TRUE);
		priv->cand_arr = NULL;
	}
}

void
lmp_im_window_clear(LmpIMWindow *self)
{
	lmp_im_window_clear_candidate(self);
	lmp_im_window_clear_code(self);
}

void 
lmp_im_window_hide(LmpIMWindow *self)
{
	lmp_im_window_clear_code(self);
	gtk_widget_hide(GTK_WIDGET(self));
}

void 
lmp_im_window_show(LmpIMWindow *self)
{
	gtk_window_resize(GTK_WINDOW(self), 1, 1);
	gtk_widget_show(GTK_WIDGET(self));
}

static GType im_win_type = 0;
static const GTypeInfo im_win_info = 
{
	sizeof(LmpIMWindowClass),
	(GBaseInitFunc)NULL,                       /* base_init */
	(GBaseFinalizeFunc)NULL,                       /* base_finalize */
	(GClassInitFunc)lmp_im_window_class_init,
	(GClassFinalizeFunc)NULL,                       /* class_finalize */
	NULL,                       /* class_data */
	sizeof(LmpIMWindow),
	0,                          /* n_preallocs */
	(GInstanceInitFunc)lmp_im_window_init
};

GType lmp_im_window_get_type()
{
	if(!im_win_type) 
	{
		im_win_type = g_type_register_static(GTK_TYPE_WINDOW, "LmpIMWindow", &im_win_info, 0);
	}

	return im_win_type;
}

void lmp_im_window_register_type(GTypeModule *type_module)
{
	if(!im_win_type) 
	{
		im_win_type = g_type_module_register_type(type_module, GTK_TYPE_WINDOW, "LmpIMWindow", &im_win_info, 0);
	}
}
