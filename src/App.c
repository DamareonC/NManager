#include "Button.h"
#include "Dir.h"
#include "Globals.h"

Globals globals;

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
    GtkBuilder* const builder = gtk_builder_new();
    gtk_builder_add_from_file(builder, "res/main.ui", NULL);

    GtkWindow* const main_window = GTK_WINDOW(gtk_builder_get_object(builder, "main_window"));
    gtk_window_set_application(main_window, app);

    GtkListBox* const list_box = GTK_LIST_BOX(gtk_builder_get_object(builder, "entry_list"));
    
    if (load_dir(list_box, g_get_home_dir()))
    {
        sprintf(globals.current_path, "%s", g_get_home_dir());
        globals.path_entry_buffer = gtk_entry_get_buffer(GTK_ENTRY(gtk_builder_get_object(builder, "path_entry")));
    }
    
    load_css();
    load_buttons(builder, list_box);

    gtk_window_present(main_window);
    g_object_unref(builder);
}