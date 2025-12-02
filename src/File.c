#include "File.h"
#include "Util.h"

int run_file(const char* const file)
{
    char command[PATH_MAX_LENGTH];
    
    set_buffer_format(command, "xdg-open '%s'", file);
    return system(command);
}