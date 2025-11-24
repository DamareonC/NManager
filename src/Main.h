#include "Button.h"
#include "Dir.h"
#include "GlobalState.h"

static GlobalState* s_load_global_state(GtkBuilder* builder)
{
    static GlobalState global_state;
    const char* const home_dir = g_get_home_dir();

    global_state.entry_list = GTK_LIST_BOX(gtk_builder_get_object(builder, "entry_list"));
    global_state.path_entry_buffer = gtk_entry_get_buffer(GTK_ENTRY(gtk_builder_get_object(builder, "path_entry")));

    gtk_entry_buffer_set_text(global_state.path_entry_buffer, home_dir, MIN(strlen(home_dir), PATH_MAX_SIZE));
    memset(global_state.current_path, 0, PATH_MAX_SIZE);
    strncpy(global_state.current_path, home_dir, PATH_MAX_SIZE);

    return &global_state;
}

static void s_load_css(void)
{
    GtkCssProvider* const css_provider = gtk_css_provider_new();
    GdkDisplay* const display = gdk_display_get_default();
    GFile* const css_file_ptr = g_file_new_for_path("res/main.css");

    gtk_css_provider_load_from_file(css_provider, css_file_ptr);
    gtk_style_context_add_provider_for_display(display, GTK_STYLE_PROVIDER(css_provider), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
    g_object_unref(css_provider);
}

static void run(GtkApplication* const app, const gpointer user_data)
{
    GtkBuilder* const builder = gtk_builder_new();
    gtk_builder_add_from_file(builder, "res/main.ui", NULL);

    GtkWindow* const main_window = GTK_WINDOW(gtk_builder_get_object(builder, "main_window"));
    gtk_window_set_application(main_window, app);

    GlobalState* const global_state = s_load_global_state(builder);
    load_dir(global_state, global_state->current_path);
    
    s_load_css();
    load_buttons(builder, global_state);

    gtk_window_present(main_window);
    g_object_unref(builder);
}