#include <stdio.h>
#include <stdlib.h>

#include "utils.h"

void handle_error(const char* func, char* error_msg, int quit) {
    fprintf(stderr, "[Error][%s] %s\n", func, error_msg);
    if (quit)
        exit(EXIT_FAILURE);
}