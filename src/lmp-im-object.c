/* vi: set sw=4 ts=4: */
/*
 * lpim-object.c
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

#include "lmp-im-object.h"
#include "lmp-im-window.h"
#include "lmp-im-db.h"

#include <gdk/gdkkeysyms.h>
#include <string.h>

//G_DEFINE_TYPE(LmpIMObject, lmp_im_object, GTK_TYPE_IM_CONTEXT);

#define LMP_IM_OBJECT_GET_PRIVATE(o) \
	(G_TYPE_INSTANCE_GET_PRIVATE((o), LMP_IM_TYPE_OBJECT, LmpIMObjectPrivate))

struct _LmpIMObjectPrivate 
{
	GdkWindow *client_window;

	GtkWidget *im_window;

	gboolean english_mode;
	guint old_keyval;
};

static void     
lmp_im_object_set_client_window(GtkIMContext *context, GdkWindow *window)
{
	//fprintf(stderr, "%s : %s\n", __FILE__, __func__);

	LmpIMObject *im = LMP_IM_OBJECT(context);
	LmpIMObjectPrivate *priv = LMP_IM_OBJECT_GET_PRIVATE(im);

	if(priv->client_window != NULL) 
	{
		g_object_unref(priv->client_window);
		priv->client_window = NULL;
	}

	if(window != NULL) 
	{
		priv->client_window = g_object_ref(window);
	}

	//fprintf(stderr, "%s : %s end\n", __FILE__, __func__);
}

#if 0
static void     
lmp_im_object_get_preedit_string(GtkIMContext *context, gchar **str, PangoAttrList **attrs, gint *cursor_pos)
{
	fprintf(stderr, "%s : %s\n", __FILE__, __func__);

	gchar *s = g_strdup("skelvenxu");
	*str = s;
	*attrs = pango_attr_list_new();
	*cursor_pos += 1;
}
#endif

static gboolean 
lmp_im_object_send_keyval(LmpIMObject *context, GdkEventKey *event)
{
	gunichar ch = gdk_keyval_to_unicode(event->keyval);
	if(ch != 0)
	{
		gint len;
		gchar buf[10];

		len = g_unichar_to_utf8(ch, buf);
		buf[len] = '\0';

		g_signal_emit_by_name(context, "commit", buf);

		return TRUE;
	}

	return FALSE;
}

static gboolean 
lmp_im_object_filter_keypress(GtkIMContext *context, GdkEventKey *event)
{
	LmpIMObject *im = LMP_IM_OBJECT(context);
	LmpIMObjectPrivate *priv = LMP_IM_OBJECT_GET_PRIVATE(im);

	//fprintf(stderr, "%s : %s\n", __FILE__, __func__);
	//fprintf(stderr, "state - 0x%X, keyval - 0x%X time %u\n", event->state & GDK_SHIFT_MASK, event->keyval, event->time);

	if(event->type == GDK_KEY_RELEASE && event->keyval == GDK_Shift_L && priv->old_keyval == GDK_Shift_L)
	{
		lmp_im_window_clear(LMP_IM_WINDOW(priv->im_window));
		gtk_widget_hide(priv->im_window);

		priv->english_mode = !priv->english_mode;
	}

	if(event->type == GDK_KEY_PRESS && event->keyval == GDK_Escape)
	{
		lmp_im_window_clear(LMP_IM_WINDOW(priv->im_window));
		gtk_widget_hide(priv->im_window);

		priv->english_mode = TRUE;
	}

	if(event->type != GDK_KEY_PRESS) 
	{
		return FALSE;
	}

	priv->old_keyval = event->keyval;

	if(priv->english_mode)
	{
		switch(event->keyval)
		{
			case GDK_BackSpace:
			case GDK_Escape:
			case GDK_Return:
			case GDK_Tab:
				return FALSE;
			default:
				lmp_im_object_send_keyval(im, event);
				return TRUE;
		}
	}

	if(((event->state & GDK_LOCK_MASK) && (!(event->state & GDK_SHIFT_MASK))) ||
		((!(event->state & GDK_LOCK_MASK)) && (event->state & GDK_SHIFT_MASK)))
	{
		lmp_im_object_send_keyval(im, event);
		return TRUE;
	}

	if(event->keyval == GDK_BackSpace)
	{
		if(lmp_im_window_get_code(LMP_IM_WINDOW(priv->im_window)))
		{
			lmp_im_window_backspace(LMP_IM_WINDOW(priv->im_window));
			const gchar *code = lmp_im_window_get_code(LMP_IM_WINDOW(priv->im_window));
			if(code && strlen(code) > 0)
			{
				GPtrArray *array = db_query2(code);
				if(array && array->len > 0)
				{
					lmp_im_window_set_candidate(LMP_IM_WINDOW(priv->im_window), array);
				}
			}
			else
			{
				gtk_widget_hide(priv->im_window);
			}

			return TRUE;
		}
		else
		{
			return FALSE;
		}
	}
	else if(event->keyval == GDK_0 || 
					event->keyval == GDK_1 ||
					event->keyval == GDK_2 ||
					event->keyval == GDK_3 ||
					event->keyval == GDK_4 ||
					event->keyval == GDK_5 ||
					event->keyval == GDK_6 ||
					event->keyval == GDK_7 ||
					event->keyval == GDK_8 ||
					event->keyval == GDK_9)
	{
		gint index = event->keyval - GDK_0;
		if(lmp_im_window_has_candidate(LMP_IM_WINDOW(priv->im_window)))
		{
			const gchar *chinese = lmp_im_window_candidate_index(LMP_IM_WINDOW(priv->im_window), index);
			g_signal_emit_by_name(im, "commit", chinese);
			lmp_im_window_clear(LMP_IM_WINDOW(priv->im_window));
			gtk_widget_hide(priv->im_window);

			return TRUE;
		}
		else
		{
			lmp_im_object_send_keyval(im, event);
			return TRUE;
		}
	}
	else if(event->keyval == GDK_space)
	{
		if(lmp_im_window_has_candidate(LMP_IM_WINDOW(priv->im_window)))
		{
			const char *chinese = lmp_im_window_candidate_index(LMP_IM_WINDOW(priv->im_window), 0);
			if(chinese)
			{
				g_signal_emit_by_name(im, "commit", chinese);
			}
		}
		else if(lmp_im_window_has_code(LMP_IM_WINDOW(priv->im_window)))
		{
		}
		else 
		{
			lmp_im_object_send_keyval(im, event);
		}

		lmp_im_window_clear(LMP_IM_WINDOW(priv->im_window));
		gtk_widget_hide(priv->im_window);

		return TRUE;
	}
	else if(event->keyval == GDK_colon ||
					event->keyval == GDK_semicolon || 
					event->keyval == GDK_less ||
					event->keyval == GDK_equal ||
					event->keyval == GDK_greater ||
					event->keyval == GDK_question || 
					event->keyval == GDK_at ||
					event->keyval == GDK_exclam ||
					event->keyval == GDK_quotedbl ||
					event->keyval == GDK_numbersign ||
					event->keyval == GDK_dollar ||
					event->keyval == GDK_percent ||
					event->keyval == GDK_ampersand ||
					event->keyval == GDK_apostrophe ||
					event->keyval == GDK_quoteright ||
					event->keyval == GDK_parenleft ||
					event->keyval == GDK_parenright ||
					event->keyval == GDK_asterisk ||
					event->keyval == GDK_plus ||
					event->keyval == GDK_comma ||
					event->keyval == GDK_minus ||
					event->keyval == GDK_period ||
					event->keyval == GDK_slash ||
					event->keyval == GDK_bracketleft ||
					event->keyval == GDK_backslash ||
					event->keyval == GDK_bracketright ||
					event->keyval == GDK_asciicircum ||
					event->keyval == GDK_underscore ||
					event->keyval == GDK_grave ||
					event->keyval == GDK_quoteleft ||
					event->keyval == GDK_braceleft ||
					event->keyval == GDK_bar ||
					event->keyval == GDK_braceright ||
					event->keyval == GDK_asciitilde 
					)
	{
		lmp_im_object_send_keyval(im, event);
	}
	else if(event->keyval == GDK_Return)
	{
		const gchar *code = lmp_im_window_get_code(LMP_IM_WINDOW(priv->im_window));
		if(code)
		{
			g_signal_emit_by_name(im, "commit", code);
			lmp_im_window_clear(LMP_IM_WINDOW(priv->im_window));
			gtk_widget_hide(priv->im_window);
			return TRUE;
		}
		else
		{
			return FALSE;
		}
	}
	else if((event->keyval >= GDK_a) && 
					(event->keyval <= GDK_z) && 
					(!(event->state & GDK_SHIFT_MASK)) &&
					(!(event->state & GDK_CONTROL_MASK)))
	{
		lmp_im_window_append_code_char(LMP_IM_WINDOW(priv->im_window), event->keyval);
		const gchar *code = lmp_im_window_get_code(LMP_IM_WINDOW(priv->im_window));
		if(code)
		{
			GPtrArray *array = db_query2(code);
			if(array && array->len > 0)
			{
				lmp_im_window_show(LMP_IM_WINDOW(priv->im_window));
				lmp_im_window_set_candidate(LMP_IM_WINDOW(priv->im_window), array);
			}
		}
	}
	else
	{
		return FALSE;
	}

	return TRUE;
}

static void     
lmp_im_object_focus_in(GtkIMContext *context)
{
	fprintf(stderr, "%s : %s\n", __FILE__, __func__);

	LmpIMObject *im = LMP_IM_OBJECT(context);
	LmpIMObjectPrivate *priv = LMP_IM_OBJECT_GET_PRIVATE(im);

	if(lmp_im_window_has_code(LMP_IM_WINDOW(priv->im_window)))
		//gtk_widget_show(priv->im_window);
		lmp_im_window_show(LMP_IM_WINDOW(priv->im_window));
}

static void     
lmp_im_object_focus_out(GtkIMContext *context)
{
	fprintf(stderr, "%s : %s\n", __FILE__, __func__);

	LmpIMObject *im = LMP_IM_OBJECT(context);
	LmpIMObjectPrivate *priv = LMP_IM_OBJECT_GET_PRIVATE(im);

	gtk_widget_hide(priv->im_window);
}

static void     
lmp_im_object_reset(GtkIMContext *context)
{
	//fprintf(stderr, "%s : %s\n", __FILE__, __func__);
}

static void     
lmp_im_object_set_cursor_location(GtkIMContext *context, GdkRectangle *area)
{
	//fprintf(stderr, "%s : %s\n", __FILE__, __func__);
	LmpIMObject *im = LMP_IM_OBJECT(context);
	LmpIMObjectPrivate *priv = LMP_IM_OBJECT_GET_PRIVATE(im);

	if(priv->client_window != NULL) 
	{
		gint x;
		gint y;

		gdk_window_get_origin(priv->client_window, &x, &y);

		GdkDisplay *display = gdk_display_get_default();
		gint cursor_size = gdk_display_get_default_cursor_size(display);

		lmp_im_window_move(LMP_IM_WINDOW(priv->im_window), x + area->x, y + area->y + cursor_size);
	}
}

static void
lmp_im_object_dispose(LmpIMObject *self)
{
	db_close();
	fprintf(stderr, "%s : %s\n", __FILE__, __func__);
}

static void
lmp_im_object_finalize(LmpIMObject *self)
{
	fprintf(stderr, "%s : %s\n", __FILE__, __func__);
}

static void
lmp_im_object_init(LmpIMObject *self)
{
	//fprintf(stderr, "%s : %s\n", __FILE__, __func__);
	LmpIMObjectPrivate *priv;// = self->priv;

	priv = LMP_IM_OBJECT_GET_PRIVATE(self);
	
	priv->client_window = NULL;
	priv->im_window = lmp_im_window_new();

	gtk_im_context_set_use_preedit(GTK_IM_CONTEXT(self), FALSE);
	//fprintf(stderr, "%s : %s end\n", __FILE__, __func__);

	priv->english_mode = FALSE;
	priv->old_keyval = 0;

	db_open(DATADIR"/wubi.db");
}

static void
lmp_im_object_class_init(LmpIMObjectClass *self_class)
{
	//fprintf(stderr, "%s : %s\n", __FILE__, __func__);
	GObjectClass *object_class = G_OBJECT_CLASS(self_class);

	g_type_class_add_private(self_class, sizeof(LmpIMObjectPrivate));

	object_class->dispose  = (void (*)(GObject *object))lmp_im_object_dispose;
	object_class->finalize = (void (*)(GObject *object))lmp_im_object_finalize;

	GTK_IM_CONTEXT_CLASS(self_class)->set_client_window   = lmp_im_object_set_client_window;
	GTK_IM_CONTEXT_CLASS(self_class)->filter_keypress     = lmp_im_object_filter_keypress;
	GTK_IM_CONTEXT_CLASS(self_class)->reset               = lmp_im_object_reset;
	//GTK_IM_CONTEXT_CLASS(self_class)->get_preedit_string  = lmp_im_object_get_preedit_string;
	GTK_IM_CONTEXT_CLASS(self_class)->focus_in            = lmp_im_object_focus_in;
	GTK_IM_CONTEXT_CLASS(self_class)->focus_out           = lmp_im_object_focus_out;
	GTK_IM_CONTEXT_CLASS(self_class)->set_cursor_location = lmp_im_object_set_cursor_location;
}

static GType im_type = 0;
static const GTypeInfo im_info = 
{
	sizeof(LmpIMObjectClass),
	(GBaseInitFunc)NULL,                       /* base_init */
	(GBaseFinalizeFunc)NULL,                       /* base_finalize */
	(GClassInitFunc)lmp_im_object_class_init,
	(GClassFinalizeFunc)NULL,                       /* class_finalize */
	NULL,                       /* class_data */
	sizeof(LmpIMObject),
	0,                          /* n_preallocs */
	(GInstanceInitFunc)lmp_im_object_init
};

GType lmp_im_object_get_type()
{
	if(!im_type) 
	{
		im_type = g_type_register_static(GTK_TYPE_IM_CONTEXT, "LmpIMObject", &im_info, 0);
	}

	return im_type;
}

void lmp_im_object_register_type(GTypeModule *type_module)
{
	if(!im_type) 
	{
		im_type = g_type_module_register_type(type_module, GTK_TYPE_IM_CONTEXT, "LmpIMObject", &im_info, 0);
	}
}

