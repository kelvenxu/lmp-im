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

	LmpimDB *db;
	gchar *db_filename;
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
	gchar *chinese = db_query_symbol(priv->db, ch);
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

static void
lmp_im_object_query_wubi_code(LmpIMObject *im, GPtrArray *arr)
{
	gint i = 0;

	LmpIMObjectPrivate *priv = LMP_IM_OBJECT_GET_PRIVATE(im);

	for(i = 0; i < arr->len; ++i)
	{
		CodeInfo *info = g_ptr_array_index(arr, i);
		gchar *code = db_query_wubi_code(priv->db, info->chinese);

		if(info->code)
		{
			g_free(info->code);
			info->code = NULL;
		}

		if(code)
		{
			info->code = code;
		}
	}
}

void 
lmp_im_update_db_freq(LmpIMObject *im, CodeInfo *info)
{
	g_return_if_fail(LMP_IM_IS_OBJECT(im));
	g_return_if_fail(info != NULL);

	LmpIMObjectPrivate *priv = LMP_IM_OBJECT_GET_PRIVATE(im);

	info->freq++;

	switch(priv->mode)
	{
		case LMP_IM_MODE_PINYIN:
			db_update_pinyin_freq (priv->db, info);	
			break;
		case LMP_IM_MODE_WUBI:
			db_update_wubi_freq (priv->db, info);	
			break;
		default:
			break;
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
			db_reset(priv->db);
			lmp_im_window_backspace(LMP_IM_WINDOW(priv->im_window));
			const gchar *code = lmp_im_window_get_code(LMP_IM_WINDOW(priv->im_window));
			lmp_im_window_set_debug_text(LMP_IM_WINDOW(priv->im_window), code);
			if(code && strlen(code) > 0)
			{
				GPtrArray *array = db_query_wubi(priv->db, code);
				if(!array)
				{
					db_close(priv->db);
					db_open(priv->db, priv->db_filename);
					array = db_query_wubi(priv->db, code);
				}

				if(array && array->len > 0)
				{
					lmp_im_window_set_candidate(LMP_IM_WINDOW(priv->im_window), array);
					lmp_im_window_page_first(LMP_IM_WINDOW(priv->im_window));
					g_ptr_array_free(array, FALSE);
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
			CodeInfo *info = lmp_im_window_candidate_index(LMP_IM_WINDOW(priv->im_window), index);
			if(info)
			{
				g_signal_emit_by_name(im, "commit", info->chinese);

				lmp_im_update_db_freq(im, info);
			}

			db_reset(priv->db);
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
			CodeInfo *info = lmp_im_window_candidate_index(LMP_IM_WINDOW(priv->im_window), 0);
			if(info)
			{
				g_signal_emit_by_name(im, "commit", info->chinese);
			}

			db_reset(priv->db);
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
		const gchar *code = lmp_im_window_get_code(LMP_IM_WINDOW(priv->im_window));
        lmp_im_window_set_debug_text(LMP_IM_WINDOW(priv->im_window), code);
		if(code)
		{
			GPtrArray *array = NULL;
			db_query_next(priv->db);
			if(priv->mode == LMP_IM_MODE_PINYIN)
			{
				array = db_query_pinyin(priv->db, &(code[1]));
				lmp_im_object_query_wubi_code(im, array);
			}
			else if(priv->mode == LMP_IM_MODE_WUBI)
			{
				array = db_query_wubi(priv->db, code);
			}

			if(array && array->len > 0)
			{
				lmp_im_window_set_candidate(LMP_IM_WINDOW(priv->im_window), array);
				g_ptr_array_free(array, FALSE);
			}
		}

		if(lmp_im_window_has_candidate(LMP_IM_WINDOW(priv->im_window)))
		{
			lmp_im_window_page_down(LMP_IM_WINDOW(priv->im_window));
			return TRUE;
		}
		else
		{
			// 输入“=“
			if(lmp_im_window_has_code(LMP_IM_WINDOW(priv->im_window)))
			{
				const gchar *code = lmp_im_window_get_code(LMP_IM_WINDOW(priv->im_window));
                lmp_im_window_set_debug_text(LMP_IM_WINDOW(priv->im_window), code);
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
			// 输入"-"
			if(lmp_im_window_has_code(LMP_IM_WINDOW(priv->im_window)))
			{
				const gchar *code = lmp_im_window_get_code(LMP_IM_WINDOW(priv->im_window));
                lmp_im_window_set_debug_text(LMP_IM_WINDOW(priv->im_window), code);
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
        lmp_im_window_set_debug_text(LMP_IM_WINDOW(priv->im_window), code);
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
        lmp_im_window_set_debug_text(LMP_IM_WINDOW(priv->im_window), code);
		if(code)
		{
			GPtrArray *array = NULL;
			if(code[0] == 'z') // 当是z开头时,输入拼音查询
			{
				priv->mode = LMP_IM_MODE_PINYIN;
				lmp_im_window_set_mode(LMP_IM_WINDOW(priv->im_window), LMP_IM_MODE_PINYIN);

				array = db_query_pinyin(priv->db, &(code[1]));
				lmp_im_object_query_wubi_code(im, array);
			}
			else
			{
				priv->mode = LMP_IM_MODE_WUBI;
				lmp_im_window_set_mode(LMP_IM_WINDOW(priv->im_window), LMP_IM_MODE_WUBI);

				array = db_query_wubi(priv->db, code);
			}

			if(array && array->len > 0)
			{
				lmp_im_window_show(LMP_IM_WINDOW(priv->im_window));
				lmp_im_window_set_candidate(LMP_IM_WINDOW(priv->im_window), array);
				lmp_im_window_page_first(LMP_IM_WINDOW(priv->im_window));

				g_ptr_array_free(array, FALSE);
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
    static guint32 time;

	LmpIMObject *im = LMP_IM_OBJECT(context);
	LmpIMObjectPrivate *priv = LMP_IM_OBJECT_GET_PRIVATE(im);

#if 0
	if(event->type == GDK_KEY_PRELEASE && (event->state & GDK_CONTROL_MASK))
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
#endif
    gboolean control_con1 = FALSE;
    gboolean shift_con2 = FALSE;
    static gboolean shift_con3 = FALSE;

    if(event->type == GDK_KEY_PRESS)
    {
        //shift_con3 = (event->keyval == GDK_KEY_Shift_L) && (event->state & GDK_SHIFT_MASK);
        shift_con3 = event->keyval == GDK_KEY_Shift_L;
    }

    control_con1 = event->type == GDK_KEY_RELEASE && 
                   event->keyval == GDK_KEY_space && 
                   (event->state & GDK_CONTROL_MASK);

    shift_con2 = event->type == GDK_KEY_RELEASE && 
                 event->keyval == GDK_KEY_Shift_L && 
                 (event->state & GDK_SHIFT_MASK);

    if(control_con1 || (shift_con2 && shift_con3))
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

	if(event->type == GDK_KEY_PRESS 
			&& event->keyval == GDK_KEY_space 
			&& (event->state & GDK_CONTROL_MASK))
	{
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
lmp_im_object_install_db(LmpIMObject *self)
{
	LmpIMObjectPrivate *priv = LMP_IM_OBJECT_GET_PRIVATE(self);

	const gchar *homedir = g_getenv("HOME");
	priv->db_filename = g_build_filename(homedir, ".lmpim", "lmp-table.db", NULL);
	
	if(!g_file_test(priv->db_filename, G_FILE_TEST_IS_REGULAR))
	{
		gchar *path = g_build_filename(homedir, ".lmpim", NULL);
		g_mkdir_with_parents(path, 0777);
		g_free(path);

		GFile *src = g_file_new_for_path(DATADIR"/lmp-table.db");
		GFile *des = g_file_new_for_path(priv->db_filename);
		g_file_copy(src, des, G_FILE_COPY_NONE, NULL, NULL, NULL, NULL);
	}
}

static void     
lmp_im_object_reset(GtkIMContext *context)
{
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
		gint self_w;
		gint self_h;

		gdk_window_get_origin(priv->client_window, &x, &y);

		GdkDisplay *display = gdk_display_get_default();
		gint cursor_size = gdk_display_get_default_cursor_size(display);

		GdkWindow *root_window = gdk_get_default_root_window();
		gint root_w = gdk_window_get_width(root_window);
		gint root_h = gdk_window_get_height(root_window);

		gtk_window_get_size(GTK_WINDOW(priv->im_window), &self_w, &self_h);

		gint abs_x = x + area->x;
		gint abs_y = y + area->y;

		if(abs_x + self_w > root_w)
		{
			abs_x = root_w - self_w;
		}

		if(abs_y + cursor_size + self_h >= root_h)
		{
			abs_y = abs_y - self_h - 4; // 输入法窗口与客户窗口离开一点，不要靠得太近。
		}
		else
		{
			abs_y = abs_y + cursor_size + 4;
		}

		lmp_im_window_move(LMP_IM_WINDOW(priv->im_window), abs_x, abs_y);
	}
}

static void
lmp_im_object_dispose(LmpIMObject *self)
{
	LmpIMObjectPrivate *priv = LMP_IM_OBJECT_GET_PRIVATE(self);

	if(priv->db_filename)
	{
		g_free(priv->db_filename);
		priv->db_filename = NULL;
	}

	db_close(priv->db);
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

	priv->mode = LMP_IM_MODE_ENGLISH;
	priv->old_keyval = 0;

	priv->db = g_object_new(LMPIM_TYPE_DB, NULL);

	lmp_im_object_install_db(self);
	db_open(priv->db, priv->db_filename);
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

