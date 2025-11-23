#include "Button.h"
#include "Dir.h"

char current_dir[1024];

static void load_css(void)
{
    GtkCssProvider* const css_provider = gtk_css_provider_new();
    GdkDisplay* const display = gdk_display_get_default();
    GFile* const css_file_ptr = g_file_new_for_path("res/main.css");

    gtk_css_provider_load_from_file(css_provider, css_file_ptr);
    gtk_style_context_add_provider_for_display(display, GTK_STYLE_PROVIDER(css_provider), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
    g_object_unref(css_provider);
}

void run(GtkApplication* const app, const gpointer user_data)
{
    sprintf(current_dir, "%s", g_get_home_dir());

    GtkBuilder* const builder = gtk_builder_new();
    gtk_builder_add_from_file(builder, "res/main.ui", NULL);

    GtkWindow* const window = GTK_WINDOW(gtk_builder_get_object(builder, "main_window"));
    gtk_window_set_application(window, app);

    GtkListBox* const list_box = GTK_LIST_BOX(gtk_builder_get_object(builder, "entry_list"));
    load_current_dir(list_box);
    
    load_css();
    load_buttons(builder, list_box);

    gtk_window_present(window);
    g_object_unref(builder);
}