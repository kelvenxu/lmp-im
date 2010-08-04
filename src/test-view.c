#include <gtk/gtk.h>

static gint delete_event(GtkWidget *widget,
                         GdkEvent *event,
		         gpointer data )
{
    gtk_main_quit();

    return TRUE;
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
    
    gtk_main();

    g_object_unref(text_buffer);
    
    return 0;
}
