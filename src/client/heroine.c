#include <stdio.h>
#include "player_common.h"

#define __PLAYER_NAME "Heroine"
#define TREE_MAX_DEPTH 3

static struct pc__player_info* pi = NULL;

char const* get_player_name() { return __PLAYER_NAME; }

void initialize(unsigned int player_id, struct graph_t* graph, unsigned int num_queens, unsigned int* queens[NUM_PLAYERS]) {
    pi = pc__init(player_id, graph, num_queens, queens);
}

// Returns number of neighboring queens of opponent player
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

// Returns score of choosing to move queen_src
uint queen_src_score(struct pc__player_info* pi, uint queen_src, uint depth) {
    if (depth == 0) {
        return 0;
    }
    uint possibleMoves;
    uint count = 0;
    for (uint i = 0; i < pi->board->num_vertices; i++) {
        if (queen_src != UINT_MAX) {
            possibleMoves = (can_reach_position(pi->board, pi->queens, queen_src, i) > 0);
            if (possibleMoves != 0) {
                count += possibleMoves * (queen_src_score(pi, i, depth - 1) + 1);
            }
        }
    }
    return count;
}

// Returns score of queen_dst if queen is in queen_src
uint queen_dst_score(struct pc__player_info* pi, uint queen_src, uint queen_dst) {
    return is_valid_move_for_player(pi->board, pi->queens, pi->player_id, queen_src, queen_dst, 1) * (possible_moves(pi, queen_dst) + 1);
}

// Returns score of choosing arrow_dst if queen is moved to queen_dst
uint arrow_dst_score(struct pc__player_info* pi, uint queen_dst, uint arrow_dst) {
    return (can_reach_position(pi->board, pi->queens, queen_dst, arrow_dst) > 0) * (neighboring_queens(pi, arrow_dst) + 1);
}

// Selects queen__src position for player pi
// The function creates and fills an array with the positions' score and chooses the position with the highest one
uint select_queen_src(struct pc__player_info* pi) {
    uint count = 0;
    uint* array = malloc(sizeof(uint) * pi->queens->nb_queens);
    for (uint i = 0; i < pi->queens->nb_queens; i++) {
        int queenScore = queen_src_score(pi, pi->queens->array[pi->player_id][i], 2);
        array[i] = queenScore;
        count += (queenScore > 0);
    }
    uint queen_src = select_max_from(array, pi->queens->nb_queens, count);
    free(array);
    return pi->queens->array[pi->player_id][queen_src];
}

// Selects queen_dst or arrow_dst position depending on the function in parameter
// The function creates and fills an array with the positions' score and chooses the position with the highest one
uint select_dst(struct pc__player_info* pi, uint source, uint (*score_function)(struct pc__player_info*, uint, uint)) {
    uint count = 0;
    uint* array = malloc(sizeof(uint) * pi->board->num_vertices);
    for (uint i = 0; i < pi->board->num_vertices; i++) {
        int moveScore = score_function(pi, source, i);
        array[i] = moveScore;
        count += (moveScore > 0);
    }
    uint destination = select_max_from(array, pi->board->num_vertices, count);
    free(array);
    return destination;
}

// Chooses the optimal move according to the player
struct move_t optimal_move(struct pc__player_info* pi) {
    struct move_t target_move = (struct move_t){-1, -1, -1};

    // Chooses queen_src to move
    uint queen_src = select_queen_src(pi);
    target_move.queen_src = queen_src;
    if (queen_src >= pi->board->num_vertices)
        return target_move;

    // Chooses queen_dst to move queen_src to
    target_move.queen_dst = select_dst(pi, queen_src, queen_dst_score);
    if (target_move.queen_dst >= pi->board->num_vertices) {
        return target_move;
    }

    // Chooses arrow_dst position
    target_move.arrow_dst = select_dst(pi, target_move.queen_dst, arrow_dst_score);
    if (target_move.arrow_dst == UINT_MAX) {
        target_move.arrow_dst = target_move.queen_src;
    }

    return target_move;
}

// Saves other player's move and chooses and plays move
struct move_t play(struct move_t previous_move) {
    pc__play_op_move(pi, previous_move);
    pi->nb_turn++;

    struct move_t target_move = (struct move_t){-1, -1, -1};
    target_move = optimal_move(pi);

    pc__play_my_move(pi, target_move);
    pi->nb_turn++;
    return target_move;
}

void finalize() { pc__free(pi); }
