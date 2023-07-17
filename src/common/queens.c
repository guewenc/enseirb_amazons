#include <stdlib.h>

#include "player.h"
#include "queens.h"
#include "utils.h"

// Converts 2D coordinates to a 1D index using row-major order
static inline uint pos_to_idx(uint size, uint x, uint y) {
    return y * size + x;
}

static void place_other_player_queens(struct queens_t* queens, uint size) {
    uint nb_cases = size * size - 1;
    for (uint i = 0; i < queens__get_nb_queens(queens); i++)
        queens->array[1][i] = nb_cases - queens->array[0][i];
}

inline uint queens__get_nb_queens(struct queens_t* queens) {
    return queens->nb_queens;
}

inline uint** queens__get_array(struct queens_t* queens) {
    return queens->array;
}

struct queens_t* queens__new() {
    struct queens_t* queens_new = malloc(sizeof(struct queens_t));
    if (!queens_new)
        handle_error(__func__, "Not enough memory for 'queens_new'", PROGRAM_EXIT);
    return queens_new;
}

void queens__alloc(struct queens_t* queens, uint nb_queens) {
    queens->nb_queens = nb_queens;
    for (uint player_id = 0; player_id < NUM_PLAYERS; player_id++) {
        queens->array[player_id] = malloc(queens__get_nb_queens(queens) * sizeof(uint));
        if (!queens->array[player_id])
            handle_error(__func__, "Not enough memory for 'queens[p]'", 1);
    }
}

void queens__copy(struct queens_t* queens_src, struct queens_t* queens_dst) {
    queens__alloc(queens_dst, queens__get_nb_queens(queens_src));

    for (uint player_id = 0; player_id < NUM_PLAYERS; player_id++) {
        for (uint queen_id = 0; queen_id < queens__get_nb_queens(queens_src); queen_id++) {
            queens_dst->array[player_id][queen_id] = queens_src->array[player_id][queen_id];
        }
    }
}

void queens__init(struct queens_t* queens, uint size) {
    uint count = queens->nb_queens / 4;

    for (uint queen_id = 0; queen_id < count; queen_id++) {
        queens->array[0][0 * count + queen_id] = pos_to_idx(size, 0, 1 + 2 * queen_id);
        queens->array[0][1 * count + queen_id] = pos_to_idx(size, 1 + 2 * queen_id, 0);
        queens->array[0][2 * count + queen_id] = pos_to_idx(size, size - 1, 1 + 2 * queen_id);
        queens->array[0][3 * count + queen_id] = pos_to_idx(size, size - 1 - 1 - 2 * queen_id, 0);
    }

    place_other_player_queens(queens, size);
}

void queens__free(struct queens_t* queens) {
    // Frees each array of queen positions for each player
    for (uint player_id = 0; player_id < NUM_PLAYERS; player_id++)
        free(queens->array[player_id]);
    // Frees the overall struct
    free(queens);
}

int queens__queen_exist_for_player(struct queens_t* queens, uint player_id, uint idx) {
    for (uint id_queen = 0; id_queen < queens->nb_queens; id_queen++) {
        if (queens->array[player_id][id_queen] == idx) {
            return 1;
        }
    }
    return 0;
}

int queens__exist_queens(struct queens_t* queens, uint idx) {
    for (uint player_id = 0; player_id < NUM_PLAYERS; player_id++) {
        if (queens__queen_exist_for_player(queens, player_id, idx)) {
            return 1;
        }
    }
    return 0;
}