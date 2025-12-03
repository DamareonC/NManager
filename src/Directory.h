#pragma once

#include "GlobalState.h"

void load_child(GlobalState* const global_state, const char* const entry_name);
bool load_directory(const GlobalState* const global_state, const char* const path);
void load_directory_and_set_state(GlobalState* const global_state, const char* const path);
void load_parent_directory(GlobalState* const global_state);
void reload_directory(const GlobalState* const global_state);