#include <stdio.h>
#include "player_common.h"

#define __PLAYER_NAME "Handy Capé"
#define TREE_MAX_DEPTH 3

static struct pc__player_info* pi = NULL;

char const* get_player_name() { return __PLAYER_NAME; }

void initialize(unsigned int player_id, struct graph_t* graph, unsigned int num_queens, unsigned int* queens[NUM_PLAYERS]) {
    pi = pc__init(player_id, graph, num_queens, queens);
}

uint neighboring_queens(struct pc__player_info* pi, uint arrow_dst) {
    uint op_player = pi->player_id ^ 1;
    uint count = 0;
    for (int i = 0; i < (int)pi->queens->nb_queens; i++) {
        uint queen = pi->queens->array[op_player][i];
        for (enum dir_t d = FIRST_DIR; d <= LAST_DIR; d++) {
            uint neigh = graph__get_neighbor(pi->board, arrow_dst, d);
            if (queen == neigh) {
                count++;
            }
        }
    }
    return count;
}

uint select_max_from_uint(uint* array, uint size, uint count) {
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

uint select_max_from_int(int* array, uint size, uint count) {
    if (!count)
        return UINT_MAX;
    int max = -INT_MAX;
    uint max_i = -1;
    for (uint i = 0; i < size; i++) {
        if (array[i] > max) {
            max = array[i];
            max_i = i;
        }
    }
    return max_i;
}

uint select_min_from(uint* array, uint size, uint count) {
    if (!count)
        return UINT_MAX;
    uint j = 0;
    while (array[j] == 0) {
        j++;
    }
    uint min = array[j];
    uint min_i = j;
    for (uint i = 0; i < size; i++) {
        if (array[i] < min && array[i] != 0) {
            min = array[i];
            min_i = i;
        }
    }
    return min_i;
}

uint queen_src_score(struct pc__player_info* pi, uint queen_src /*, uint depth*/) {
    uint possibleMoves;
    uint count = 0;
    for (uint i = 0; i < pi->board->num_vertices; i++) {
        if (queen_src != UINT_MAX) {
            possibleMoves = (can_reach_position(pi->board, pi->queens, queen_src, i) > 0);
            count += possibleMoves;
            /*
            if (possibleMoves != 0 && depth > 0) {
                count += possibleMoves + (queen_src_score(pi, i, depth - 1) + 1) / (possibleMoves);
            }
            */
        }
    }
    return count;
}

int adv_move_reduced(struct pc__player_info* pi, uint queen_dst) {
    int count = 0;
    uint pos;
    uint queen;
    uint d;
    for (uint q = 0; q < pi->queens->nb_queens; q++) {
        queen = pi->queens->array[pc__get_other_player(pi)][q];
        if ((d = can_reach_position(pi->board, pi->queens, queen, queen_dst))) {
            pos = queen_dst;
            while (pos != UINT_MAX) {
                pos = graph__get_neighbor(pi->board, pos, d);
                count++;
            }
        }
    }
    return count;
}

int adv_move_added(struct pc__player_info* pi, uint queen_src) {
    int count = 0;
    uint pos;
    uint queen;
    uint d;
    for (uint q = 0; q < pi->queens->nb_queens; q++) {
        queen = pi->queens->array[pc__get_other_player(pi)][q];
        if ((d = can_reach_position(pi->board, pi->queens, queen, queen_src))) {
            pos = queen_src;
            while (pos != UINT_MAX) {
                pos = graph__get_neighbor(pi->board, pos, d);
                count++;
            }
        }
    }
    return count;
}

int player_move_reduced(struct pc__player_info* pi, uint queen_dst, uint queen_src, int isqueen) {
    int count = 0;
    uint pos;
    uint queen;
    uint d;
    for (uint q = 0; q < pi->queens->nb_queens; q++) {
        queen = pi->queens->array[pi->player_id][q];
        if ((queen == queen_src) && (isqueen))
            ;
        else {
            if ((d = can_reach_position(pi->board, pi->queens, queen, queen_dst))) {
                pos = queen_dst;
                while (pos != UINT_MAX) {
                    pos = graph__get_neighbor(pi->board, pos, d);
                    count++;
                }
            }
        }
    }
    return count;
}

int player_move_added(struct pc__player_info* pi, uint queen_src) {
    int count = 0;
    uint pos;
    uint queen;
    uint d;
    for (uint q = 0; q < pi->queens->nb_queens; q++) {
        queen = pi->queens->array[pi->player_id][q];
        if (queen != queen_src)
            if ((d = can_reach_position(pi->board, pi->queens, queen, queen_src))) {
                pos = queen_src;
                while (pos != UINT_MAX) {
                    pos = graph__get_neighbor(pi->board, pos, d);
                    count++;
                }
            }
    }
    return count;
}

int sum_queen_move(struct pc__player_info* pi, uint src, uint dst) {
    return player_move_added(pi, src) + adv_move_reduced(pi, dst) - player_move_reduced(pi, dst, src, 1) - adv_move_added(pi, src) - (int)possible_moves(pi, src) + (int)possible_moves(pi, dst);
}

int sum_arrow_move(struct pc__player_info* pi, uint src, uint dst) {
    return adv_move_reduced(pi, dst) - player_move_reduced(pi, dst, src, 0);
}

int can_reach_direction_arrow(struct graph_t* board, struct queens_t* queens, uint pos1, uint pos2, uint queen_src, enum dir_t dir) {
    if (pos1 == UINT_MAX || queens__exist_queens(queens, pos1)) {
        return 0;
    }
    if (pos1 == pos2) {
        return 1;
    }
    return can_reach_direction_arrow(board, queens, graph__get_neighbor(board, pos1, dir), pos2, queen_src, dir);
}

int can_reach_position_arrow(struct graph_t* board, struct queens_t* queens, uint pos1, uint pos2, uint queen_src) {
    if (queen_src == pos2)
        return 1;
    for (enum dir_t d = FIRST_DIR; d <= LAST_DIR; d++) {
        if (can_reach_direction_arrow(board, queens, graph__get_neighbor(board, pos1, d), pos2, queen_src, d)) {
            return d;
        }
    }
    return 0;
}

uint queen_dst_score(struct pc__player_info* pi, uint queen_src, uint queen_dst) {
    return is_valid_move_for_player(pi->board, pi->queens, pi->player_id, queen_src, queen_dst, 1) * (possible_moves(pi, queen_dst) + 1);
}

uint arrow_dst_score(struct pc__player_info* pi, uint queen_dst, uint arrow_dst) {
    return (can_reach_position(pi->board, pi->queens, queen_dst, arrow_dst) > 0) * (adv_move_reduced(pi, arrow_dst) + 1);
}

struct move_t slct_move(struct pc__player_info* pi) {
    int max = -INT_MAX;
    struct move_t ret;
    for (uint q = 0; q < pi->queens->nb_queens; q++) {
        if (can_move(pi->board, pi->queens, pi->queens->array[pi->player_id][q])) {
            for (uint i = 0; i < pi->board->num_vertices; i++) {
                if (can_reach_position(pi->board, pi->queens, pi->queens->array[pi->player_id][q], i)) {
                    for (uint j = 0; j < pi->board->num_vertices; j++) {
                        if (can_reach_position_arrow(pi->board, pi->queens, i, j, pi->queens->array[pi->player_id][q])) {
                            if (max < sum_queen_move(pi, pi->queens->array[pi->player_id][q], i) + sum_arrow_move(pi, i, j)) {
                                max = sum_queen_move(pi, pi->queens->array[pi->player_id][q], i) + sum_arrow_move(pi, i, j);
                                ret = (struct move_t){pi->queens->array[pi->player_id][q], i, j};
                            }
                        }
                    }
                }
            }
        }
    }
    return ret;
}

struct move_t optimal_move(struct pc__player_info* pi) {
    struct move_t target_move = (struct move_t){-1, -1, -1};
    /*
    uint queen_src = select_queen_src(pi);
    target_move.queen_src = queen_src;
    if (queen_src >= pi->board->num_vertices)
        return target_move;

    target_move.queen_dst = select_dst(pi, queen_src, queen_dst_score);
    if (target_move.queen_dst >= pi->board->num_vertices) {
        return target_move;
    }
    */
    struct move_t tmp = slct_move(pi);
    /*if ((tmp.queen_src >= pi->board->num_vertices) || (tmp.queen_dst >= pi->board->num_vertices)){
      return target_move;}
    tmp.arrow_dst = select_dst(pi, tmp.queen_dst, arrow_dst_score);
    if (tmp.arrow_dst == UINT_MAX) {
        tmp.arrow_dst = tmp.queen_src;
        }*/
    target_move = tmp;
    return target_move;
}

struct move_t play(struct move_t previous_move) {
    pc__play_op_move(pi, previous_move);
    pi->nb_turn++;

    struct move_t target_move = (struct move_t){-1, -1, -1};
    target_move = optimal_move(pi);
    // printf("%u %u %u\n", target_move.queen_src, target_move.queen_dst, target_move.arrow_dst);
    pc__play_my_move(pi, target_move);
    pi->nb_turn++;
    return target_move;
}

void finalize() { pc__free(pi); }
