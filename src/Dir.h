#pragma once

#include <gtk/gtk.h>

#include "GlobalState.h"

bool load_dir(GlobalState* const global_state, const char* const dir);
void load_parent(GlobalState* const global_state);
void load_child_activate(GtkListBoxRow* const list_box, const gpointer data);
void load_child_clicked(GtkButton* const button, const gpointer data);