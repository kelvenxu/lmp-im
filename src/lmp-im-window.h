/* vi: set sw=4 ts=4: */
/*
 * lmp-im-window.h
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

#ifndef __LMP_IM_WINDOW_H__
#define __LMP_IM_WINDOW_H__  1

#include <glib-object.h>
#include <gtk/gtk.h>
#include "type.h"

G_BEGIN_DECLS

#define LMP_TYPE_IM_WINDOW (lmp_im_window_get_type())
#define LMP_IM_WINDOW(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), LMP_TYPE_IM_WINDOW, LmpIMWindow))
#define LMP_IM_WINDOW_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), LMP_TYPE_IM_WINDOW, LmpIMWindowClass))
#define LMP_IS_IM_WINDOW(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), LMP_TYPE_IM_WINDOW))
#define LMP_IS_IM_WINDOW_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), LMP_TYPE_IM_WINDOW))
#define LMP_IM_WINDOW_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS((obj), LMP_TYPE_IM_WINDOW, LmpIMWindowClass))

typedef struct _LmpIMWindowPrivate LmpIMWindowPrivate;

typedef struct 
{
	GtkWindow parent;

	LmpIMWindowPrivate *priv;
} LmpIMWindow;

typedef struct
{
	GtkWindowClass parent;
} LmpIMWindowClass;

GType         lmp_im_window_get_type         ();
void          lmp_im_object_register_type    (GTypeModule *type_module);
void          lmp_im_window_register_type    (GTypeModule *type_module);

GtkWidget *   lmp_im_window_new              ();

void          lmp_im_window_clear_code       (LmpIMWindow *self);
void          lmp_im_window_clear_candidate  (LmpIMWindow *self);
void          lmp_im_window_clear            (LmpIMWindow *self);

int           lmp_im_window_backspace        (LmpIMWindow *self);

void          lmp_im_window_set_candidate (LmpIMWindow *self, GPtrArray *arr);
CodeInfo * lmp_im_window_candidate_index  (LmpIMWindow *self, gint index);

void          lmp_im_window_append_code_char (LmpIMWindow *self, gchar ch);
const gchar * lmp_im_window_get_code         (LmpIMWindow *self);
gboolean      lmp_im_window_has_code         (LmpIMWindow *self);
gboolean      lmp_im_window_has_candidate    (LmpIMWindow *self);

void lmp_im_window_hide      (LmpIMWindow *self);
void lmp_im_window_show      (LmpIMWindow *self);
void lmp_im_window_move      (LmpIMWindow *self, int x, int y);

void lmp_im_window_page_up   (LmpIMWindow *self);
void lmp_im_window_page_down (LmpIMWindow *self);
void lmp_im_window_page_first(LmpIMWindow *self);

void lmp_im_window_set_mode(LmpIMWindow *self, LmpIMMode mode);

G_END_DECLS
#endif /*__LMP_IM_WINDOW_H__ */
