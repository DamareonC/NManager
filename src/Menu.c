#include "Directory.h"
#include "glib.h"
#include "Menu.h"

static void s_show_hidden_activate(GSimpleAction* const action, GVariant* const param, const gpointer state)
{
    GlobalState* const global_state = state;
    global_state->show_hidden = g_variant_get_boolean(param);

    load_directory(global_state, global_state->current_path);
    g_simple_action_set_state(action, param);
}

void load_menu(GtkApplication* const app, GlobalState* const global_state)
{
    const GActionEntry action_entries[] = {
        { "show-hidden", NULL, NULL, global_state->show_hidden ? "true" : "false", s_show_hidden_activate }
    };

    g_action_map_add_action_entries(G_ACTION_MAP(app), action_entries, G_N_ELEMENTS(action_entries), global_state);
}