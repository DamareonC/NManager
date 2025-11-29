#pragma once

#include "GlobalState.h"

void load_directory_and_set_state(GlobalState* const global_state, const char* const path);
bool load_directory(GlobalState* const global_state, const char* const path);
void load_parent_directory(GlobalState* const global_state);