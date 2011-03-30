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
#include "type.h"

#include <gdk/gdkkeysyms.h>
#include <string.h>

//G_DEFINE_TYPE(LmpIMObject, lmp_im_object, GTK_TYPE_IM_CONTEXT);

#define LMP_IM_OBJECT_GET_PRIVATE(o) \
	(G_TYPE_INSTANCE_GET_PRIVATE((o), LMP_IM_TYPE_OBJECT, LmpIMObjectPrivate))


struct _LmpIMObjectPrivate 
{
	GdkWindow *client_window;

	GtkWidget *im_window;

	LmpIMMode mode;
	guint old_keyval;
};

static void     
lmp_im_object_set_client_window(GtkIMContext *context, GdkWindow *window)
{
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
}

#if 0
static void     
lmp_im_object_get_preedit_string(GtkIMContext *context, gchar **str, PangoAttrList **attrs, gint *cursor_pos)
{
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

static void
lmp_im_object_symbol(LmpIMObject *im, GdkEventKey *event)
{
	LmpIMObjectPrivate *priv = LMP_IM_OBJECT_GET_PRIVATE(im);

	gunichar ch = gdk_keyval_to_unicode(event->keyval);
	gchar *chinese = db_query_symbol(ch);
	if(chinese)
	{
		g_signal_emit_by_name(im, "commit", chinese);
		g_free(chinese);
	}
	else
	{
		lmp_im_object_send_keyval(im, event);
	}
}

static gboolean
lmp_im_object_wubi_mode(GtkIMContext *context, GdkEventKey *event)
{
	LmpIMObject *im = LMP_IM_OBJECT(context);
	LmpIMObjectPrivate *priv = LMP_IM_OBJECT_GET_PRIVATE(im);

	// 功能键
	if(event->keyval == GDK_KEY_Tab || 
			event->keyval == GDK_KEY_Home ||
			event->keyval == GDK_KEY_Left ||
			event->keyval == GDK_KEY_Right ||
			event->keyval == GDK_KEY_Down ||
			event->keyval == GDK_KEY_Up ||
			event->keyval == GDK_KEY_Page_Up ||
			event->keyval == GDK_KEY_Page_Down ||
			event->keyval == GDK_KEY_End ||
			event->keyval == GDK_KEY_Begin)
	{
		lmp_im_window_clear(LMP_IM_WINDOW(priv->im_window));
		gtk_widget_hide(priv->im_window);

		return FALSE;
	}

	// 大写A-Z
	if((event->keyval >= GDK_KEY_A) && (event->keyval <= GDK_KEY_Z))
	{
		lmp_im_window_clear(LMP_IM_WINDOW(priv->im_window));
		gtk_widget_hide(priv->im_window);

		lmp_im_object_send_keyval(im, event);
		return TRUE;
	}	

	// 退格键
	if(event->keyval == GDK_KEY_BackSpace)
	{
		if(lmp_im_window_get_code(LMP_IM_WINDOW(priv->im_window)))
		{
			lmp_im_window_backspace(LMP_IM_WINDOW(priv->im_window));
			const gchar *code = lmp_im_window_get_code(LMP_IM_WINDOW(priv->im_window));
			if(code && strlen(code) > 0)
			{
				GPtrArray *array = db_query_wubi(code);
				if(!array)
				{
					db_close();
					db_open(DATADIR"/wubi.db");
					array = db_query_wubi(code);
				}

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

	// 数字键
	if(event->keyval == GDK_KEY_0 || 
					event->keyval == GDK_KEY_1 ||
					event->keyval == GDK_KEY_2 ||
					event->keyval == GDK_KEY_3 ||
					event->keyval == GDK_KEY_4 ||
					event->keyval == GDK_KEY_5 ||
					event->keyval == GDK_KEY_6 ||
					event->keyval == GDK_KEY_7 ||
					event->keyval == GDK_KEY_8 ||
					event->keyval == GDK_KEY_9)
	{
		gint index = event->keyval - GDK_KEY_0;
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

	// 空格键
	if(event->keyval == GDK_KEY_space)
	{
		// 按空格，选择第一个
		if(lmp_im_window_has_candidate(LMP_IM_WINDOW(priv->im_window)))
		{
			const char *chinese = lmp_im_window_candidate_index(LMP_IM_WINDOW(priv->im_window), 0);
			if(chinese)
			{
				g_signal_emit_by_name(im, "commit", chinese);
			}

			lmp_im_window_clear(LMP_IM_WINDOW(priv->im_window));
			gtk_widget_hide(priv->im_window);
			return TRUE;
		}
		else if(lmp_im_window_has_code(LMP_IM_WINDOW(priv->im_window)))
		{
			return FALSE;
		}
		else 
		{
			lmp_im_object_send_keyval(im, event);
			return TRUE;
		}
	}

	// 后翻页
	if(event->keyval == GDK_KEY_equal) 
	{
		// "="号翻页
		if(lmp_im_window_has_candidate(LMP_IM_WINDOW(priv->im_window)))
		{
			lmp_im_window_page_down(LMP_IM_WINDOW(priv->im_window));
			return TRUE;
		}
		else
		{
			if(lmp_im_window_has_code(LMP_IM_WINDOW(priv->im_window)))
			{
				const gchar *code = lmp_im_window_get_code(LMP_IM_WINDOW(priv->im_window));
				g_signal_emit_by_name(im, "commit", code);
			}

			lmp_im_window_clear(LMP_IM_WINDOW(priv->im_window));
			gtk_widget_hide(priv->im_window);

			lmp_im_object_symbol(im, event);

			return TRUE;
		}
	}

	// 前翻页
	if(event->keyval == GDK_KEY_minus)
	{
		// "-"号翻页
		if(lmp_im_window_has_candidate(LMP_IM_WINDOW(priv->im_window)))
		{
			lmp_im_window_page_up(LMP_IM_WINDOW(priv->im_window));
			return TRUE;
		}
		else
		{
			if(lmp_im_window_has_code(LMP_IM_WINDOW(priv->im_window)))
			{
				const gchar *code = lmp_im_window_get_code(LMP_IM_WINDOW(priv->im_window));
				g_signal_emit_by_name(im, "commit", code);
			}

			lmp_im_window_clear(LMP_IM_WINDOW(priv->im_window));
			gtk_widget_hide(priv->im_window);

			lmp_im_object_symbol(im, event);
			return TRUE;
		}
	}

	// 符号，不用Shift键
	if(event->keyval == GDK_KEY_grave ||
					event->keyval == GDK_KEY_quoteleft ||
					event->keyval == GDK_KEY_minus ||
					event->keyval == GDK_KEY_equal ||
					event->keyval == GDK_KEY_bracketleft ||
					event->keyval == GDK_KEY_bracketright ||
					event->keyval == GDK_KEY_backslash ||
					event->keyval == GDK_KEY_semicolon || 
					event->keyval == GDK_KEY_apostrophe ||
					event->keyval == GDK_KEY_quoteright ||
					event->keyval == GDK_KEY_period ||
					event->keyval == GDK_KEY_comma ||
					event->keyval == GDK_KEY_slash) 
	{
		lmp_im_window_clear(LMP_IM_WINDOW(priv->im_window));
		gtk_widget_hide(priv->im_window);

		lmp_im_object_symbol(im, event);

		return TRUE;
	}

	// 符号，Shift键配合
	if((event->state & GDK_SHIFT_MASK) && 
					(event->keyval == GDK_KEY_asciitilde ||
					event->keyval == GDK_KEY_exclam ||
					event->keyval == GDK_KEY_at ||
					event->keyval == GDK_KEY_numbersign ||
					event->keyval == GDK_KEY_dollar ||
					event->keyval == GDK_KEY_percent ||
					event->keyval == GDK_KEY_asciicircum ||
					event->keyval == GDK_KEY_ampersand ||
					event->keyval == GDK_KEY_asterisk ||
					event->keyval == GDK_KEY_parenleft ||
					event->keyval == GDK_KEY_parenright ||
					event->keyval == GDK_KEY_underscore ||
					event->keyval == GDK_KEY_plus ||
					event->keyval == GDK_KEY_braceleft ||
					event->keyval == GDK_KEY_braceright ||
					event->keyval == GDK_KEY_bar ||
					event->keyval == GDK_KEY_colon ||
					event->keyval == GDK_KEY_quotedbl ||
					event->keyval == GDK_KEY_less ||
					event->keyval == GDK_KEY_greater ||
					event->keyval == GDK_KEY_question))
	{
		lmp_im_window_clear(LMP_IM_WINDOW(priv->im_window));
		gtk_widget_hide(priv->im_window);

		lmp_im_object_symbol(im, event);

		return TRUE;
	}

	// 回车键
	if(event->keyval == GDK_KEY_Return)
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

	// 小写字母
	if((event->keyval >= GDK_KEY_a) && 
					(event->keyval <= GDK_KEY_z) && 
					(!(event->state & GDK_SHIFT_MASK)) &&
					(!(event->state & GDK_CONTROL_MASK)))
	{
		lmp_im_window_show(LMP_IM_WINDOW(priv->im_window));
		lmp_im_window_append_code_char(LMP_IM_WINDOW(priv->im_window), event->keyval);
		const gchar *code = lmp_im_window_get_code(LMP_IM_WINDOW(priv->im_window));
		if(code)
		{
			GPtrArray *array = NULL;
			if(code[0] == 'z') // 当是z开头时,输入拼音查询
			{
				array = db_query_pinyin(&(code[1]));
				priv->mode = LMP_IM_MODE_PINYIN;

				lmp_im_window_set_mode(LMP_IM_WINDOW(priv->im_window), LMP_IM_MODE_PINYIN);
			}
			else
			{
				array = db_query_wubi(code);
				priv->mode = LMP_IM_MODE_WUBI;

				lmp_im_window_set_mode(LMP_IM_WINDOW(priv->im_window), LMP_IM_MODE_WUBI);
			}

			if(array && array->len > 0)
			{
				lmp_im_window_show(LMP_IM_WINDOW(priv->im_window));
				lmp_im_window_set_candidate(LMP_IM_WINDOW(priv->im_window), array);
			}
			else
			{
				//lmp_im_window_set_no_candidate(LMP_IM_WINDOW(priv->im_window));
			}
		}

		return TRUE;
	}

	return FALSE;
}

static gboolean
lmp_im_object_english_mode(GtkIMContext *context, GdkEventKey *event)
{
	LmpIMObject *im = LMP_IM_OBJECT(context);
	LmpIMObjectPrivate *priv = LMP_IM_OBJECT_GET_PRIVATE(im);

	if(event->keyval == GDK_KEY_BackSpace || 
			event->keyval == GDK_KEY_Escape ||
			event->keyval == GDK_KEY_Return || 
			event->keyval == GDK_KEY_Tab ||
			event->keyval == GDK_KEY_Home ||
			event->keyval == GDK_KEY_Left ||
			event->keyval == GDK_KEY_Right ||
			event->keyval == GDK_KEY_Up ||
			event->keyval == GDK_KEY_Down ||
			event->keyval == GDK_KEY_Page_Up ||
			event->keyval == GDK_KEY_Page_Down ||
			event->keyval == GDK_KEY_End ||
			event->keyval == GDK_KEY_Begin)
	{
		return FALSE;
	}

	// 注意顺序不能变,先判断CONTROL_MASK, 最后才是SHIFT_MASK
	if(event->state & GDK_CONTROL_MASK)
	{
		return FALSE;
	}

	// alt key
	if(event->state & GDK_MOD1_MASK)
	{
		return FALSE;
	}

	if(event->state & GDK_LOCK_MASK)
	{
		return FALSE;
	}

	if(event->state & GDK_SHIFT_MASK)
	{
		lmp_im_object_send_keyval(im, event);
		return TRUE;
	}

	lmp_im_object_send_keyval(im, event);
	return TRUE;
}

static gboolean 
lmp_im_object_filter_keypress(GtkIMContext *context, GdkEventKey *event)
{
	LmpIMObject *im = LMP_IM_OBJECT(context);
	LmpIMObjectPrivate *priv = LMP_IM_OBJECT_GET_PRIVATE(im);

	if(event->type == GDK_KEY_RELEASE && event->keyval == GDK_KEY_space && event->state & GDK_CONTROL_MASK)
	{
		lmp_im_window_clear(LMP_IM_WINDOW(priv->im_window));
		gtk_widget_hide(priv->im_window);

		if(priv->mode == LMP_IM_MODE_ENGLISH)
		{
			priv->mode = LMP_IM_MODE_WUBI;
		}
		else
		{
			priv->mode = LMP_IM_MODE_ENGLISH;
		}

		return FALSE;
	}
	else if(event->type == GDK_KEY_PRESS && event->keyval == GDK_KEY_Escape)
	{
		lmp_im_window_clear(LMP_IM_WINDOW(priv->im_window));
		gtk_widget_hide(priv->im_window);

		priv->mode = LMP_IM_MODE_ENGLISH;
		return FALSE;
	}

	if(event->type == GDK_KEY_PRESS && event->keyval == GDK_KEY_space && event->state & GDK_CONTROL_MASK)
	{
		//priv->old_keyval = event->keyval;
		return FALSE;
	}

	if(event->type != GDK_KEY_PRESS) 
	{
		return FALSE;
	}

	if(priv->mode == LMP_IM_MODE_ENGLISH)
	{
		return lmp_im_object_english_mode(context, event);
	}
	else
	{
		return lmp_im_object_wubi_mode(context, event);
	}
}


static void     
lmp_im_object_focus_in(GtkIMContext *context)
{
	LmpIMObject *im = LMP_IM_OBJECT(context);
	LmpIMObjectPrivate *priv = LMP_IM_OBJECT_GET_PRIVATE(im);

	if(lmp_im_window_has_code(LMP_IM_WINDOW(priv->im_window)))
	{
		lmp_im_window_show(LMP_IM_WINDOW(priv->im_window));
	}
}

static void     
lmp_im_object_focus_out(GtkIMContext *context)
{
	LmpIMObject *im = LMP_IM_OBJECT(context);
	LmpIMObjectPrivate *priv = LMP_IM_OBJECT_GET_PRIVATE(im);

	gtk_widget_hide(priv->im_window);
}

static void     
lmp_im_object_reset(GtkIMContext *context)
{
	g_print("object reset\n");
}

static void     
lmp_im_object_set_cursor_location(GtkIMContext *context, GdkRectangle *area)
{
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
}

static void
lmp_im_object_finalize(LmpIMObject *self)
{
}

static void
lmp_im_object_init(LmpIMObject *self)
{
	LmpIMObjectPrivate *priv = LMP_IM_OBJECT_GET_PRIVATE(self);
	
	priv->client_window = NULL;
	priv->im_window = lmp_im_window_new();

	gtk_im_context_set_use_preedit(GTK_IM_CONTEXT(self), FALSE);

	//priv->english_mode = TRUE; //输入法默认不应开启
	priv->mode = LMP_IM_MODE_ENGLISH;
	priv->old_keyval = 0;

	db_open(DATADIR"/wubi.db");
}

static void
lmp_im_object_class_init(LmpIMObjectClass *self_class)
{
	GObjectClass *object_class = G_OBJECT_CLASS(self_class);

	g_type_class_add_private(self_class, sizeof(LmpIMObjectPrivate));

	object_class->dispose  = (void (*)(GObject *object))lmp_im_object_dispose;
	object_class->finalize = (void (*)(GObject *object))lmp_im_object_finalize;

	GTK_IM_CONTEXT_CLASS(self_class)->set_client_window   = lmp_im_object_set_client_window;
	GTK_IM_CONTEXT_CLASS(self_class)->filter_keypress     = lmp_im_object_filter_keypress;
	//GTK_IM_CONTEXT_CLASS(self_class)->reset               = lmp_im_object_reset;
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

