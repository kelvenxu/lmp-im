/* vi: set sw=4 ts=4: */
/*
 * test-im.c
 *
 * This file is part of ________.
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
#include <gdk/gdkkeysyms.h>
#include <string.h>
#include "lmp-im-window.h"
#include "database.h"

static void
entry_buffer_changed_cb(GtkEntryBuffer *buffer, guint pos, gchar *arg2, guint arg3, GtkTextBuffer *text_buffer)
{
	const char *txt = gtk_entry_buffer_get_text(buffer);
	g_print("entry_buffer change %s %u %s\n", txt, pos, arg2);

	if(g_ascii_isspace(*arg2))
	{
		static GtkTextIter iter; 
		static gboolean flag = FALSE;

		if(!flag)
		{
			gtk_text_buffer_get_start_iter(text_buffer, &iter);
			flag = TRUE;
		}

		gtk_entry_buffer_delete_text(buffer, pos, 1);
		gtk_text_buffer_insert(text_buffer, &iter, txt, strlen(txt));

		guint len = gtk_entry_buffer_get_length(buffer);
		gtk_entry_buffer_delete_text(buffer, 0, len);
	}
}
	
static void
view_buffer_changed_cb(GtkTextBuffer *buffer, gpointer user_data)
{
}
static gboolean
focus_in_cb(GtkWidget *widget, GdkEventFocus *event, LmpIMWindow *im_window)
{
	//gtk_widget_show(GTK_WIDGET(im_window));
	lmp_im_window_show(im_window);
	return FALSE;
}

static gboolean
focus_out_cb(GtkWidget *widget, GdkEventFocus *event, LmpIMWindow *im_window)
{
	//gtk_widget_hide(GTK_WIDGET(im_window));
	lmp_im_window_hide(im_window);

	return FALSE;
}

static gboolean 
key_press_cb(GtkWidget *widget, GdkEventKey *event, LmpIMWindow *im_window)
{
	printf("event->keyval = %c\n", event->keyval);
	GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(widget));

	if(event->keyval == GDK_BackSpace)
	{
		lmp_im_window_backspace(im_window);
		const gchar *code = lmp_im_window_get_code(im_window);
		if(code && strlen(code) > 0)
		{
			GPtrArray *array = db_query2(code);
			if(array && array->len > 0)
			{
				lmp_im_window_set_candidate(im_window, array);
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
		g_print("digit %d\n", index);
		if(lmp_im_window_has_candidate(im_window))
		{
			const gchar *chinese = lmp_im_window_candidate_index(im_window, index);
			gtk_text_buffer_insert_at_cursor(buffer, chinese, strlen(chinese));
			lmp_im_window_clear(im_window);

			return TRUE;
		}
	}
	else if(event->keyval == GDK_space)
	{
		if(lmp_im_window_has_candidate(im_window))
		{
			const char *chinese = lmp_im_window_candidate_index(im_window, 0);
			if(chinese)
			{
				g_print("%s\n", chinese);
				gtk_text_buffer_insert_at_cursor(buffer, chinese, strlen(chinese));
			}
		}

		lmp_im_window_clear(im_window);

		return TRUE;
	}
	else
	{
		lmp_im_window_append_code_char(im_window, event->keyval);
		const gchar *code = lmp_im_window_get_code(im_window);
		if(code)
		{
			GPtrArray *array = db_query2(code);
			if(array && array->len > 0)
			{
				lmp_im_window_set_candidate(im_window, array);
			}
		}
		//g_ptr_array_free(array, TRUE);
	}

#if 0
	const gchar *code = lmp_im_window_get_code(im_window);
	GPtrArray *array = db_query2(code);
	if(array && array->len > 0)
	{
		lmp_im_window_set_candidate(im_window, array);

		CodeInfo *info = g_ptr_array_index(array, 0);
		if(info)
		{
			g_print("%s\n", info->chinese);
			gtk_text_buffer_insert_at_cursor(buffer, info->chinese, strlen(info->chinese));
		}
	}
	g_ptr_array_free(array, TRUE);
#endif

	return TRUE;
}

int main(int argc, char *argv[])
{
	gtk_init(&argc, &argv);

	GtkWidget *win = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	GtkWidget *entry = gtk_entry_new();
	GtkWidget *text_view = gtk_text_view_new();

	GtkWidget *vbox = gtk_vbox_new(FALSE, 6);

	GtkWidget *im_window = lmp_im_window_new();

	gtk_box_pack_start(GTK_BOX(vbox), entry, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(vbox), text_view, TRUE, TRUE, 0);

	gtk_container_add(GTK_CONTAINER(win), vbox);

	gtk_widget_set_size_request(win, 400, 540);
	gtk_widget_show_all(win);

	GtkEntryBuffer *entry_buffer = gtk_entry_get_buffer(GTK_ENTRY(entry));
	GtkTextBuffer *view_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_view));

	g_signal_connect(entry_buffer,  "inserted-text", G_CALLBACK(entry_buffer_changed_cb), view_buffer);
	//g_signal_connect(view_buffer,  "changed", G_CALLBACK(view_buffer_changed_cb), NULL);
	g_signal_connect(text_view, "key-press-event", G_CALLBACK(key_press_cb), im_window);
	g_signal_connect(text_view, "focus-in-event", G_CALLBACK(focus_in_cb), im_window);
	g_signal_connect(text_view, "focus-out-event", G_CALLBACK(focus_out_cb), im_window);

	g_signal_connect(win, "delete-event", G_CALLBACK(gtk_main_quit), NULL);

	db_open("../wubi.db");
	gtk_main();
	db_close();
	return 0;
}

