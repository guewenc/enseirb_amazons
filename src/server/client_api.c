#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>

#include "client_api.h"
#include "utils.h"

struct player {
    void* client_dl;
    char const* (*get_player_name)();
    void (*initialize)(uint, struct graph_t*, uint, uint**);
    struct move_t (*play)(struct move_t);
    void (*finalize)();
};

// Checks for errors in dlsym function
static void check_dlsym(const char* func) {
    char* dl_error;
    if ((dl_error = dlerror()))
        handle_error(func, dl_error, 1);
}

static void* load_and_check_dlsym(const char* func, struct player* client_p, const char* func_to_load) {
    void* ptr = dlsym(client_p->client_dl, func_to_load);
    check_dlsym(func);
    return ptr;
}

struct player* client__load(const char* client_path) {
    struct player* client_p = malloc(sizeof(struct player));

    if (!client_p)
        handle_error(__func__, "Not enough memory", 1);

    client_p->client_dl = dlopen(client_path, RTLD_LAZY);

    if (!client_p->client_dl)
        handle_error(__func__, "Invalid player configuration file", 1);

    dlerror(); // Clears any existing error

    // Retrieves functions from player configuration file
    client_p->get_player_name =
        load_and_check_dlsym(__func__, client_p, "get_player_name");
    client_p->initialize = load_and_check_dlsym(__func__, client_p, "initialize");
    client_p->play = load_and_check_dlsym(__func__, client_p, "play");
    client_p->finalize = load_and_check_dlsym(__func__, client_p, "finalize");

    return client_p;
}

void client__unload(struct player* p) {
    if (dlclose(p->client_dl))
        handle_error(__func__, "Failed on freeing the configuration file player", 0);
    free(p);
}

char const* client__get_player_name(const struct player* p) {
    return p->get_player_name();
}

void client__initialize(struct player* p, uint player_id, struct graph_t* graph, uint num_queens, uint** queens) {
    return p->initialize(player_id, graph, num_queens, queens);
}

struct move_t client__play(struct player* p, struct move_t previous_move) {
    return p->play(previous_move);
}

void client__finalize(struct player* p) { p->finalize(); }