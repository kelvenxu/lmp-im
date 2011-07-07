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
#include <string.h>

//G_DEFINE_TYPE(LmpIMWindow, lmp_im_window, GTK_TYPE_WINDOW);

#define LMP_IM_WINDOW_GET_PRIVATE(o)\
	(G_TYPE_INSTANCE_GET_PRIVATE((o), LMP_TYPE_IM_WINDOW, LmpIMWindowPrivate))

struct _LmpIMWindowPrivate 
{
	//GtkWidget *handle;
	GtkWidget *frame;
	GtkWidget *vbox;
	GtkWidget *code_label;
	GtkWidget *cand_label;
	GtkWidget* sepatator;

	GString *code_str;
	GString *cand_str;

	GPtrArray *cand_arr;

	gint cand_page;
	gint cand_page_num;

	gint position_x;
	gint position_y;

	LmpIMMode mode;
};

enum
{
	CANDIDATE_NUM = 5,
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
	//priv->handle = gtk_handle_box_new();
	priv->frame = gtk_frame_new(NULL);
	priv->vbox = gtk_vbox_new(FALSE, 0);
	priv->code_label = gtk_label_new("");
	priv->cand_label = gtk_label_new("");

	priv->sepatator = gtk_hseparator_new();
	gtk_box_pack_start(GTK_BOX(priv->vbox), priv->code_label, TRUE, TRUE, 1);
	gtk_box_pack_start(GTK_BOX(priv->vbox), priv->sepatator, TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(priv->vbox), priv->cand_label, TRUE, TRUE, 1);

	//gtk_container_add(GTK_CONTAINER(self), priv->handle);
	//gtk_container_add(GTK_CONTAINER(priv->handle), priv->frame);
	gtk_container_add(GTK_CONTAINER(self), priv->frame);
	gtk_container_add(GTK_CONTAINER(priv->frame), priv->vbox);

	gtk_misc_set_alignment(GTK_MISC(priv->code_label), 0, 0.5);
	gtk_misc_set_alignment(GTK_MISC(priv->cand_label), 0, 0.5);

	gtk_label_set_single_line_mode(GTK_LABEL(priv->code_label), TRUE);
	gtk_label_set_single_line_mode(GTK_LABEL(priv->cand_label), TRUE);

	//gtk_widget_show(priv->handle);
	gtk_widget_show(priv->frame);
	gtk_widget_show(priv->vbox);
	gtk_widget_show(priv->code_label);
	gtk_widget_show(priv->cand_label);
	gtk_widget_show(priv->sepatator);

	priv->code_str = g_string_new("");
	priv->cand_str = g_string_new("");
	priv->cand_arr = NULL;

	priv->cand_page = 0;
	priv->cand_page_num = 0;

	priv->position_x = 0;
	priv->position_y = 0;

	priv->mode = LMP_IM_MODE_WUBI;
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

static void
lmp_im_window_set_code_text(LmpIMWindow *self, const gchar *str)
{
	LmpIMWindowPrivate *priv = LMP_IM_WINDOW_GET_PRIVATE(self);
	
	if(priv->mode == LMP_IM_MODE_PINYIN)
	{
		if(strlen(str) > 1)
		{
			gchar *s = g_strdup_printf("拼 %s", &(str[1]));
			gtk_label_set_text(GTK_LABEL(priv->code_label), s);
			g_free(s);
		}
		else
		{
			gtk_label_set_markup(GTK_LABEL(priv->code_label), "拼 ");
		}
	}
	else
	{
		gtk_label_set_markup(GTK_LABEL(priv->code_label), str);
	}
}

void 
lmp_im_window_set_cand_text(LmpIMWindow *self, const gchar *str)
{
	LmpIMWindowPrivate *priv = LMP_IM_WINDOW_GET_PRIVATE(self);
	gtk_label_set_markup(GTK_LABEL(priv->cand_label), str);
}

void
lmp_im_window_append_code_char(LmpIMWindow *self, gchar ch)
{
	LmpIMWindowPrivate *priv = LMP_IM_WINDOW_GET_PRIVATE(self);

	g_string_append_c(priv->code_str, ch);

	lmp_im_window_set_code_text(self, priv->code_str->str);

	lmp_im_window_clear_candidate(self);
}

void
lmp_im_window_clear_code(LmpIMWindow *self)
{
	LmpIMWindowPrivate *priv = LMP_IM_WINDOW_GET_PRIVATE(self);

	lmp_im_window_set_code_text(self, "");

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
		lmp_im_window_set_code_text(self, priv->code_str->str);

		lmp_im_window_clear_candidate(self);
	}

	return priv->code_str->len;
}
static void
lmp_im_window_set_candidate_range(LmpIMWindow *self, gint begin, gint end)
{
	gint i = 0;
	LmpIMWindowPrivate *priv = LMP_IM_WINDOW_GET_PRIVATE(self);
	
	g_string_erase(priv->cand_str, 0, -1);

	//if(priv->mode == LMP_IM_MODE_PINYIN)
	{
		for(i = begin; i < end && i < priv->cand_arr->len && (i - begin) < CANDIDATE_NUM; ++i)
		{
			CodeInfo *info = g_ptr_array_index(priv->cand_arr, i);
			g_string_append_printf(priv->cand_str, 
					"<span size=\"large\">%s</span><span color=\"#844798\"><sup>%d</sup><sub>%s</sub></span>  ", 
					info->chinese, i - begin, info->code);
		}
	}
#if 0
	else
	{
		for(i = begin; i < end && i < priv->cand_arr->len && (i - begin) < CANDIDATE_NUM; ++i)
		{
			CodeInfo *info = g_ptr_array_index(priv->cand_arr, i);
			g_string_append_printf(priv->cand_str, "<span size=\"large\">%s</span><span color=\"#844798\"><sup>%d</sup></span>  ", info->chinese, i - begin);
		}
	}
#endif

	lmp_im_window_set_cand_text(self, priv->cand_str->str);
}

void 
lmp_im_window_set_candidate(LmpIMWindow *self, GPtrArray *arr)
{
	LmpIMWindowPrivate *priv = LMP_IM_WINDOW_GET_PRIVATE(self);

	priv->cand_arr = arr;
	
#if 0
	gint i = 0;
	
	if(priv->mode == LMP_IM_MODE_PINYIN)
	{
		for(i = 0; i < arr->len && i < CANDIDATE_NUM; ++i)
		{
			CodeInfo *info = g_ptr_array_index(arr, i);
			g_string_append_printf(priv->cand_str, "<b>%s</b><span color=\"#844798\"><sup>%d</sup>%s</span> ", info->chinese, i, info->code);
		}
	}
	else
	{
		for(i = 0; i < arr->len && i < CANDIDATE_NUM; ++i)
		{
			CodeInfo *info = g_ptr_array_index(arr, i);
			g_string_append_printf(priv->cand_str, "<b>%s</b><span color=\"#844798\"><sup>%d</sup></span> ", info->chinese, i);
		}
	}

	lmp_im_window_set_cand_text(self, priv->cand_str->str);
#endif

	lmp_im_window_set_candidate_range(self, 0, CANDIDATE_NUM);

	priv->cand_page = 0;
	priv->cand_page_num = priv->cand_arr->len / CANDIDATE_NUM;

	if(priv->cand_arr->len % CANDIDATE_NUM)
	{
		priv->cand_page_num++;
	}
}

void 
lmp_im_window_page_up(LmpIMWindow *self)
{
	LmpIMWindowPrivate *priv = LMP_IM_WINDOW_GET_PRIVATE(self);

	if(!priv->cand_arr || priv->cand_arr->len <= 0)
		return;

	if(priv->cand_page <= 0)
		return;

	priv->cand_page--;

	gint start = priv->cand_page * CANDIDATE_NUM;

	if(start >= 0 && start < priv->cand_arr->len)
	{
#if 0
		gint i = 0;
		g_string_erase(priv->cand_str, 0, -1);

		for(i = priv->cand_page * CANDIDATE_NUM; i < priv->cand_arr->len && i < start + CANDIDATE_NUM; ++i)
		{
			CodeInfo *info = g_ptr_array_index(priv->cand_arr, i);
			g_string_append_printf(priv->cand_str, "%d. %s ", i - priv->cand_page * CANDIDATE_NUM, info->chinese);
		}

		lmp_im_window_set_cand_text(self, priv->cand_str->str);
#endif

		lmp_im_window_set_candidate_range(self, start, start + CANDIDATE_NUM);
	}
}

void 
lmp_im_window_page_down(LmpIMWindow *self)
{
	LmpIMWindowPrivate *priv = LMP_IM_WINDOW_GET_PRIVATE(self);

	if(!priv->cand_arr || priv->cand_arr->len <= 0)
		return;

	if(priv->cand_page >= priv->cand_page_num - 1)
		return;

	priv->cand_page++;

	gint start = priv->cand_page * CANDIDATE_NUM;
	
	if(start >= 0 && start < priv->cand_arr->len)
	{
#if 0
		gint i = 0;
		g_string_erase(priv->cand_str, 0, -1);

		for(i = priv->cand_page * CANDIDATE_NUM; i < priv->cand_arr->len && i < start + CANDIDATE_NUM; ++i)
		{
			CodeInfo *info = g_ptr_array_index(priv->cand_arr, i);
			g_string_append_printf(priv->cand_str, "%d. %s ", i - priv->cand_page * CANDIDATE_NUM, info->chinese);
		}

		lmp_im_window_set_cand_text(self, priv->cand_str->str);
#endif
		lmp_im_window_set_candidate_range(self, start, start + CANDIDATE_NUM);
	}
}

CodeInfo*
lmp_im_window_candidate_index(LmpIMWindow *self, gint index)
{
	CodeInfo *info = NULL;
	LmpIMWindowPrivate *priv = LMP_IM_WINDOW_GET_PRIVATE(self);

	gint id = index + priv->cand_page * CANDIDATE_NUM;
	if(priv->cand_arr && index >= 0 && index < priv->cand_arr->len)
	{
		info = g_ptr_array_index(priv->cand_arr, id);
		//return info->chinese;
	}

	return info;
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

	lmp_im_window_set_cand_text(self, "");

	g_string_erase(priv->cand_str, 0, -1);

	if(priv->cand_arr)
	{
		g_ptr_array_free(priv->cand_arr, TRUE);
		priv->cand_arr = NULL;
	}

	priv->cand_page = 0;
	priv->cand_page_num = 0;
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
	LmpIMWindowPrivate *priv = LMP_IM_WINDOW_GET_PRIVATE(self);

	gtk_window_resize(GTK_WINDOW(self), 1, 1);

	gtk_widget_show(GTK_WIDGET(self));
	gtk_window_move(GTK_WINDOW(self), priv->position_x, priv->position_y);
}

void
lmp_im_window_move(LmpIMWindow *self, int x, int y)
{
	int root_w; 
	int root_h;
	int self_w;
	int self_h;

	LmpIMWindowPrivate *priv = LMP_IM_WINDOW_GET_PRIVATE(self);

	GdkWindow *root_window = gdk_get_default_root_window();
	//gdk_drawable_get_size(GDK_DRAWABLE(root_window), &root_w, &root_h);
	root_w = gdk_window_get_width(root_window);
	root_h = gdk_window_get_height(root_window);
	gtk_window_get_size(GTK_WINDOW(self), &self_w, &self_h);

	if(x + self_w > root_w)
	{
		x = root_w - self_w;
	}

	priv->position_x = x;
	priv->position_y = y + 4;
	gtk_window_move(GTK_WINDOW(self), priv->position_x, priv->position_y);
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

void lmp_im_window_set_mode(LmpIMWindow *self, LmpIMMode mode)
{
	LmpIMWindowPrivate *priv = LMP_IM_WINDOW_GET_PRIVATE(self);
	priv->mode = mode;
}

