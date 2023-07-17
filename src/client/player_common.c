#include "player_common.h"
#include <limits.h>
#include <math.h>
#include "utils.h"

uint pc__get_other_player(struct pc__player_info* pi) { return pi->player_id ^ 1; }

static inline int is_first_move(struct move_t m) {
    return m.arrow_dst == UINT_MAX || m.queen_dst == UINT_MAX || m.queen_src == UINT_MAX;
}

static void play_move(struct pc__player_info* pi, uint id_p, struct move_t m) {
    if (is_first_move(m)) return;

    for (uint i = 0; i < pi->queens->nb_queens; i++) {
        if (pi->queens->array[id_p][i] == m.queen_src) {
            pi->queens->array[id_p][i] = m.queen_dst;
            break;
        }
    }

    graph__disconnect(pi->board, m.arrow_dst);
}

struct pc__player_info* pc__init(uint player_id,
                                 struct graph_t* graph,
                                 uint num_queens,
                                 uint* queens[NUM_PLAYERS]) {
    if (!graph)
        handle_error(__func__, "Invalid parameter 'graph'", 1);
    if (!queens)
        handle_error(__func__, "Invalid parameter 'queens'", 1);
    struct pc__player_info* pi = malloc(sizeof(struct pc__player_info));
    if (!pi)
        handle_error(__func__, "Not enough memory for 'pi'", 1);

    pi->player_id = player_id;
    pi->board = graph;
    pi->queens = queens__new();
    pi->queens->nb_queens = num_queens;
    pi->nb_turn = 0;

    for (uint p = 0; p < NUM_PLAYERS; p++)
        pi->queens->array[p] = queens[p];

    return pi;
}

void pc__free(struct pc__player_info* pi) {
    if (pi) {
        graph__free(pi->board);
        queens__free(pi->queens);
    }
    free(pi);
}

void pc__play_my_move(struct pc__player_info* pi, struct move_t m) {
    play_move(pi, pi->player_id, m);
}

void pc__play_op_move(struct pc__player_info* pi, struct move_t m) {
    play_move(pi, pc__get_other_player(pi), m);
}

uint select_random_queen(struct pc__player_info* pi) {
    int queen = -1;
    for (int i = 0; i < (int)pi->queens->nb_queens; i++) {
        if (can_move(pi->board, pi->queens, pi->queens->array[pi->player_id][i])) {
            return pi->queens->array[pi->player_id][i];
        }
    }

    return queen;
}

uint select_random_from(uint* array, uint size, uint count) {
    if (!count)
        return UINT_MAX;
    uint random = rand() % count;
    for (uint i = 0, cpt = 0; i < size; i++) {
        cpt += array[i];
        if (cpt > random) {
            return i;
        }
    }
    return -1;
}

uint select_max_from(uint* array, uint size, uint count) {
    if (!count)
        return UINT_MAX;
    uint max = 0;
    uint max_i = -1;
    for (uint i = 0; i < size; i++) {
        if (array[i] > max) {
            max = array[i];
            max_i = i;
        }
    }
    return max_i;
}

uint possible_moves(struct pc__player_info* pi, uint queen_src) {
    uint count = 0;
    for (uint i = 0; i < pi->board->num_vertices; i++) {
        count += (can_reach_position(pi->board, pi->queens, queen_src, i) > 0);
    }
    return count;
}
