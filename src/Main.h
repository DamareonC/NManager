#include "Button.h"
#include "Directory.h"
#include "Menu.h"

static void s_entry_list_activate(const GtkListBox* const list_box, GtkListBoxRow* const list_box_row, GlobalState* const global_state)
{
    load_child(global_state, gtk_label_get_text(GTK_LABEL(gtk_list_box_row_get_child(list_box_row))));
}

static GlobalState* s_init_global_state(GtkBuilder* const builder, GtkWindow* const main_window)
{
    static GlobalState global_state;
    global_state.main_window = main_window;
    global_state.entry_list = GTK_LIST_BOX(gtk_builder_get_object(builder, "entry_list"));
    global_state.path_entry_buffer = gtk_entry_get_buffer(GTK_ENTRY(gtk_builder_get_object(builder, "path_entry")));
    global_state.show_hidden = false;
    set_global_state(&global_state, g_get_home_dir());

    g_signal_connect(global_state.entry_list, "row-activated", G_CALLBACK(s_entry_list_activate), &global_state);

    return &global_state;
}

static void s_load_css(void)
{
    GtkCssProvider* const css_provider = gtk_css_provider_new();
    GFile* const css_file_ptr = g_file_new_for_path("res/css/main.css");

    gtk_css_provider_load_from_file(css_provider, css_file_ptr);
    gtk_style_context_add_provider_for_display(gdk_display_get_default(), GTK_STYLE_PROVIDER(css_provider), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
    
    g_object_unref(css_provider);
    g_object_unref(css_file_ptr);
}

static void init_app(GtkApplication* const app, const gconstpointer user_data)
{
    GtkBuilder* const builder = gtk_builder_new();
    gtk_builder_add_from_file(builder, "res/ui/main.ui", NULL);

    GtkWindow* const main_window = GTK_WINDOW(gtk_builder_get_object(builder, "main_window"));
    gtk_window_set_application(main_window, app);

    GlobalState* const global_state = s_init_global_state(builder, main_window);
    reload_directory(global_state);
    
    s_load_css();
    load_buttons(builder, global_state);
    load_menu(app, global_state);
    gtk_window_present(main_window);

    g_object_unref(builder);
}