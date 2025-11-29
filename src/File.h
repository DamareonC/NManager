#pragma once

#include "GlobalState.h"

int run_file(const char* const file);
void run_child_file(const GlobalState* const global_state, const char* const entry_name);