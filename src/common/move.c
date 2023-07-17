#include "move.h"
#include <string.h>
#include "queens.h"

struct move_t create_initial_move() {
    return (struct move_t){UINT_MAX, UINT_MAX, UINT_MAX};
}

int is_initial_move(struct move_t m) {
    return m.arrow_dst == UINT_MAX || m.queen_dst == UINT_MAX || m.queen_src == UINT_MAX;
}

int can_reach_direction(struct graph_t* board, struct queens_t* queens, uint pos1, uint pos2, enum dir_t dir) {
    if (pos1 == UINT_MAX || queens__exist_queens(queens, pos1)) {
        return 0;
    }
    if (pos1 == pos2) {
        return 1;
    }
    return can_reach_direction(board, queens, graph__get_neighbor(board, pos1, dir), pos2, dir);
}

int can_reach_position(struct graph_t* board, struct queens_t* queens, uint pos1, uint pos2) {
    for (enum dir_t d = FIRST_DIR; d <= LAST_DIR; d++) {
        if (can_reach_direction(board, queens, graph__get_neighbor(board, pos1, d), pos2, d)) {
            return d;
        }
    }
    return 0;
}

int is_valid_move_for_player(struct graph_t* board, struct queens_t* queens, uint player_id, uint id_src, uint id_dst, int is_arrow) {
    if (!is_arrow && !queens__queen_exist_for_player(queens, player_id, id_src))
        return 0;
    return can_reach_position(board, queens, id_src, id_dst);
}

void move_queen(struct queens_t* queens, uint player_id, struct move_t move) {
    for (uint queen_id = 0; queen_id < queens->nb_queens; queen_id++) {
        if (queens->array[player_id][queen_id] == move.queen_src) { // If the queen is the one to be moved
            queens->array[player_id][queen_id] = move.queen_dst; // Move the queen to its destination position
            return;
        }
    }

    // If the function reaches this point, it means the queen to be moved was not found
    assert(0);
}

int can_move(struct graph_t* board, struct queens_t* queens, uint pos) {
    /* Check if the queen is isolated */
    if (is_isolated(board, pos))
        return 0;

    /* Check if the queen has any neighboring empty cells */
    for (enum dir_t dir = FIRST_DIR; dir <= LAST_DIR; dir++) {
        uint neighbor = graph__get_neighbor(board, pos, dir);
        if (neighbor != UINT_MAX && !queens__exist_queens(queens, neighbor)) {
            return 1;
        }
    }

    /* If no neighboring empty cells are found, the queen cannot move */
    return 0;
}
