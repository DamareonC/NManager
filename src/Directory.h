#pragma once

#include "GlobalState.h"

bool load_directory(GlobalState* const global_state, const char* const path);
void reload_directory(GlobalState* const global_state);
void load_directory_and_set_global_state(GlobalState* const global_state, const char* const path);
void load_child(GlobalState* const global_state, const char* const filename);
void load_parent_directory(GlobalState* const global_state);