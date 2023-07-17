#include "export.h"
#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include "utils.h"

#define PERMISSIONS S_IRWXU

int export__create_dir(const char* path) {
    struct stat st;
    if (stat(path, &st) == -1) {
        if (mkdir(path, PERMISSIONS) == -1) {
            handle_error(__func__, strerror(errno), 0);
            return -1;
        }
        return 0;
    }
    return 0;
}

void export__compile_dot() {
    DIR* dir;
    struct dirent* ent;

    char command[1024];

    dir = opendir(EXPORT_DIR);
    if (dir == NULL) {
        handle_error(__func__, "Cannot open directory", 0);
        return;
    }

    while ((ent = readdir(dir)) != NULL) {
        if (strstr(ent->d_name, ".dot") != NULL) {
            snprintf(command, sizeof(command), "dot -Kneato -Tpng ./export/%s -o ./export/%s.png", ent->d_name, ent->d_name);
            system(command);
        }
    }

    closedir(dir);

    // Removes dot files
    system("rm ./export/*.dot");
}

void export__remove_dir(const char* directory) {
    struct stat st;
    if (stat(directory, &st) != -1) {
        char command[512];
        sprintf(command, "rm -r %s", directory);
        if (system(command) == -1) {
            handle_error(__func__, strerror(errno), 0);
            return;
        }
    }
}
