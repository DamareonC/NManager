#pragma once

#include "GlobalState.h"

void load_directory_and_set_state(GlobalState* const global_state, const char* const path);
bool load_directory(GlobalState* const global_state, const char* const path);
void load_parent(GlobalState* const global_state);
void load_child_activate(GtkListBoxRow* const list_box, GlobalState* const data);
void load_child_clicked(GtkButton* const button, GlobalState* const data);