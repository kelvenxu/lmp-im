#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>

static gint delete_event(GtkWidget *widget, GdkEvent *event, gpointer data)
{
	gtk_main_quit();
	return TRUE;
}

static void print_symbol()
{
	fprintf(stderr, "GDK_KEY_colon : %c\n",  GDK_KEY_colon );
	fprintf(stderr, "GDK_KEY_period : %c\n",  GDK_KEY_period );
	fprintf(stderr, "GDK_KEY_semicolon : %c\n",  GDK_KEY_semicolon ); 
	fprintf(stderr, "GDK_KEY_equal : %c\n",  GDK_KEY_equal );
	fprintf(stderr, "GDK_KEY_quotedbl : %c\n",  GDK_KEY_quotedbl );
	fprintf(stderr, "GDK_KEY_numbersig: %c\n",  GDK_KEY_numbersign );
	fprintf(stderr, "GDK_KEY_apostrophe: %c\n",  GDK_KEY_apostrophe );
	fprintf(stderr, "GDK_KEY_quoteright: %c\n",  GDK_KEY_quoteright );
	fprintf(stderr, "GDK_KEY_comma : %c\n",  GDK_KEY_comma );
	fprintf(stderr, "GDK_KEY_bracketleft: %c\n",  GDK_KEY_bracketleft );
	fprintf(stderr, "GDK_KEY_bracketright: %c\n",  GDK_KEY_bracketright );
	fprintf(stderr, "GDK_KEY_asciicircum: %c\n",  GDK_KEY_asciicircum );
	fprintf(stderr, "GDK_KEY_grave : %c\n",  GDK_KEY_grave );
	fprintf(stderr, "GDK_KEY_quoteleft: %c\n",  GDK_KEY_quoteleft );
	fprintf(stderr, "GDK_KEY_bar : %c\n",  GDK_KEY_bar );
	fprintf(stderr, "GDK_KEY_less : %c\n",  GDK_KEY_less );
	fprintf(stderr, "GDK_KEY_greater : %c\n",  GDK_KEY_greater );
	fprintf(stderr, "GDK_KEY_question : %c\n",  GDK_KEY_question ); 
	fprintf(stderr, "GDK_KEY_at : %c\n",  GDK_KEY_at );
	fprintf(stderr, "GDK_KEY_exclam : %c\n",  GDK_KEY_exclam );
	fprintf(stderr, "GDK_KEY_quotedbl : %c\n",  GDK_KEY_quotedbl );
	fprintf(stderr, "GDK_KEY_numbersig: %c\n",  GDK_KEY_numbersign );
	fprintf(stderr, "GDK_KEY_dollar : %c\n",  GDK_KEY_dollar );
	fprintf(stderr, "GDK_KEY_percent |: %c\n",  GDK_KEY_percent );
	fprintf(stderr, "GDK_KEY_ampersand: %c\n",  GDK_KEY_ampersand );
	fprintf(stderr, "GDK_KEY_apostrophe: %c\n",  GDK_KEY_apostrophe );
	fprintf(stderr, "GDK_KEY_quoteright: %c\n",  GDK_KEY_quoteright );
	fprintf(stderr, "GDK_KEY_parenleft: %c\n",  GDK_KEY_parenleft );
	fprintf(stderr, "GDK_KEY_parenright: %c\n",  GDK_KEY_parenright );
	fprintf(stderr, "GDK_KEY_asterisk : %c\n",  GDK_KEY_asterisk );
	fprintf(stderr, "GDK_KEY_plus : %c\n",  GDK_KEY_plus );
	fprintf(stderr, "GDK_KEY_minus : %c\n",  GDK_KEY_minus );
	fprintf(stderr, "GDK_KEY_bracketleft: %c\n",  GDK_KEY_bracketleft );
	fprintf(stderr, "GDK_KEY_backslash: %c\n",  GDK_KEY_backslash );
	fprintf(stderr, "GDK_KEY_slash: %c\n",  GDK_KEY_slash );
	fprintf(stderr, "GDK_KEY_bracketright: %c\n",  GDK_KEY_bracketright );
	fprintf(stderr, "GDK_KEY_asciicircum: %c\n",  GDK_KEY_asciicircum );
	fprintf(stderr, "GDK_KEY_underscore: %c\n",  GDK_KEY_underscore );
	fprintf(stderr, "GDK_KEY_grave : %c\n",  GDK_KEY_grave );
	fprintf(stderr, "GDK_KEY_quoteleft: %c\n",  GDK_KEY_quoteleft );
	fprintf(stderr, "GDK_KEY_braceleft: %c\n",  GDK_KEY_braceleft );
	fprintf(stderr, "GDK_KEY_bar : %c\n",  GDK_KEY_bar );
	fprintf(stderr, "GDK_KEY_braceright: %c\n",  GDK_KEY_braceright );
	fprintf(stderr, "GDK_KEY_asciitilde: %c\n",  GDK_KEY_asciitilde );
}

int main( int   argc,
		char *argv[] )
{
	GtkTextBuffer *text_buffer;
	GtkWidget *window;
	GtkWidget *scrolled_window;
	GtkWidget *text_view;

	gtk_init(&argc, &argv);

	text_buffer = gtk_text_buffer_new(NULL);

	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	g_signal_connect(G_OBJECT(window), "delete_event",
			G_CALLBACK(delete_event), NULL);
	gtk_window_set_default_size(GTK_WINDOW(window), 512, 480);

	scrolled_window = gtk_scrolled_window_new(NULL, NULL);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window),
			GTK_POLICY_AUTOMATIC,
			GTK_POLICY_AUTOMATIC);
	gtk_container_add(GTK_CONTAINER(window), scrolled_window);

	text_view = gtk_text_view_new_with_buffer(text_buffer);
	gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(text_view), GTK_WRAP_NONE);
	gtk_container_add(GTK_CONTAINER(scrolled_window), text_view);

	gtk_widget_show_all(window);

	print_symbol();
	gtk_main();

	g_object_unref(text_buffer);

	return 0;
}
