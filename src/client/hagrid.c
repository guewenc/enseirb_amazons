#include <stdio.h>
#include "dir.h"
#include "player_common.h"

#define __PLAYER_NAME "Hagrid"

// The higher the slower but better
#define OPT_DEPT 1.2
#define RATIO_KEPT 1

static struct pc__player_info* pi = NULL;

static uint** queens_possible_moves = NULL;
static uint** arrow_possible_moves = NULL;

//Useful struct for minimax_t
struct minimax_t {
    struct move_t move;
    double value;
};

//Strict copy of game is over function from game, adapted for a copy used in minmax
int game__is_over(struct graph_t* graph, struct queens_t* queens) {
    for (uint player_id = 0; player_id < NUM_PLAYERS; player_id++) {
        int is_queen_moveable = 0;
        for (uint queen_id = 0; queen_id < queens__get_nb_queens(queens); queen_id++) {
            if (can_move(graph, queens, queens->array[player_id][queen_id])) {
                is_queen_moveable = 1;
            }
        }
        if (!is_queen_moveable) {
            return 1;
        }
    }
    return 0;
}

//Checks if a move contains UINT_MAX
static inline int is_first_move(struct move_t m) {
    return m.arrow_dst == UINT_MAX || m.queen_dst == UINT_MAX || m.queen_src == UINT_MAX;
}

//Play the move m on the given graph and queens
static void play_move(struct graph_t* graph, struct queens_t* queens, uint id_p, struct move_t m) {
    if (is_first_move(m)) return;
    for (uint i = 0; i < pi->queens->nb_queens; i++) {
        if (queens->array[id_p][i] == m.queen_src) {
            queens->array[id_p][i] = m.queen_dst;
            break;
        }
    }
    graph__disconnect(graph, m.arrow_dst);
}

//Returns the amount of queens of player_id that would be blocked by the arrow at arrow_dst
static uint is_arrow_blocking_player(struct graph_t* graph, struct queens_t* queens, uint arrow_dst, uint player_id) {
    uint nb_block = 0;
    for (enum dir_t dir = FIRST_DIR; dir <= LAST_DIR; dir++)
        if (queens__queen_exist_for_player(queens, player_id, graph__get_neighbor(graph, arrow_dst, dir)))
            nb_block++;
    return nb_block;
}

//Fills the array possible_moves with the possible moves in dir d 
static uint fill_possible_moves_queens_rec(struct graph_t* graph, struct queens_t* queens, uint pos, enum dir_t dir, uint* possible_moves, uint size, uint r) {
    if (pos == UINT_MAX || queens__exist_queens(queens, pos) || (size && (rand() % r) == 1))
        return size;
    size = fill_possible_moves_queens_rec(graph, queens, graph__get_neighbor(graph, pos, dir), dir, possible_moves, size, r);
    if (possible_moves)
        possible_moves[size] = pos;
    return size + 1;
}

//Fills the array possible move with the possible move from position src, r represents the ratio of move kept
static uint fill_possible_moves_queen(struct graph_t* graph, struct queens_t* queens, uint src, uint* possible_moves, uint r) {
    uint size = 0;
    for (enum dir_t dir = FIRST_DIR; dir <= LAST_DIR; dir++) {
        size = fill_possible_moves_queens_rec(graph, queens, graph__get_neighbor(graph, src, dir), dir, possible_moves, size, r);
    }
    if (possible_moves)
        possible_moves[size] = UINT_MAX;
    return size;
}

//Fills the array possible_moves with the possible arrow moves in dir d 
static int fill_possible_moves_arrow_rec(struct graph_t* graph, struct queens_t* queens, uint pos, enum dir_t dir, uint* possible_moves, uint size, uint op) {
    if (pos == UINT_MAX || queens__exist_queens(queens, pos) || (size && !is_arrow_blocking_player(graph, queens, pos, op)))
        return size;
    size = fill_possible_moves_arrow_rec(graph, queens, graph__get_neighbor(graph, pos, dir), dir, possible_moves, size, op);
    if (possible_moves)
        possible_moves[size] = pos;
    return size + 1;
}

//Fills the array possible_moves with the possible arrow shots from position src. If the array is not empty, only arrows blocking op's queen 
static void fill_possible_moves_arrow(struct graph_t* graph, struct queens_t* queens, uint src, uint queen_src, uint* possible_moves, uint op) {
    uint size = 0;
    for (enum dir_t dir = FIRST_DIR; dir <= LAST_DIR; dir++) {
        size = fill_possible_moves_arrow_rec(graph, queens, graph__get_neighbor(graph, src, dir), dir, possible_moves, size, op);
    }
    if (possible_moves) {
        possible_moves[size] = queen_src;
        possible_moves[size + 1] = -1;
    }
}

//Returns the number of possible move for player_id
static uint get_nb_possible_moves(struct graph_t* graph, struct queens_t* queens, uint player_id) {
    uint nb_possible_move = 0;
    for (uint queen_id = 0; queen_id < queens->nb_queens; queen_id++) {
        uint queen_src = queens->array[player_id][queen_id];
        nb_possible_move += fill_possible_moves_queen(graph, queens, queen_src, NULL, 1);
    }
    return nb_possible_move;
}

//Compute the dynamic depth
static uint get_optimal_depth() {
    return (uint)(OPT_DEPT + (float)pi->board->num_vertices / (get_nb_possible_moves(pi->board, pi->queens, pi->player_id) + get_nb_possible_moves(pi->board, pi->queens, pc__get_other_player(pi))));
}

//Returns the amount of movable queens for player_id
static uint nb_movable(struct graph_t* graph, struct queens_t* queens, uint player_id) {
    uint nb_can_move = 0;
    for (uint queen_id = 0; queen_id < queens->nb_queens; queen_id++) {
        if (can_move(graph, queens, queens->array[player_id][queen_id]))
            nb_can_move += 1;
    }
    return nb_can_move;
}

//Game heuristic based on possible moves and movable queens
static double simple_heuristic(struct graph_t* graph, struct queens_t* queens) {
    double nb_possible_move = (double)get_nb_possible_moves(graph, queens, pi->player_id);
    double nb_op_move = (double)get_nb_possible_moves(graph, queens, pc__get_other_player(pi));
    double nb_movable_queens = (double)nb_movable(graph, queens, pi->player_id);
    double nb_movable_op = (double)nb_movable(graph, queens, pc__get_other_player(pi));
    double possible_move_ratio = (double)(nb_possible_move - nb_op_move) / (double)graph->num_vertices;
    double nb_movable_ratio = (double)(nb_movable_queens - nb_movable_op) / (double)queens->nb_queens;
    return possible_move_ratio + nb_movable_ratio;
}

// Copy src_g and src_q in dst_h and dst_q repectively
static void copy_graph_and_queens(struct graph_t* src_g, struct queens_t* src_q, struct graph_t* dst_g, struct queens_t* dst_q) {
    dst_g->num_vertices = src_g->num_vertices;
    gsl_spmatrix_uint_memcpy(dst_g->t, src_g->t);
    dst_q->nb_queens = src_q->nb_queens;
    for (uint player_id = 0; player_id < 2; player_id++)
        for (uint i = 0; i < src_q->nb_queens; i++)
            dst_q->array[player_id][i] = src_q->array[player_id][i];
}

//Return the max value between a and b
static struct minimax_t max(struct minimax_t a, struct minimax_t b) {
    if (a.value > b.value)
        return a;
    return b;
}

//Return the min value between a and b
static struct minimax_t min(struct minimax_t a, struct minimax_t b) {
    if (a.value < b.value)
        return a;
    return b;
}

//Apply the minimax algorithm, graph and queens are array of copies, depending on the depth, the algorithm apply the move. Implements alphabeta
static struct minimax_t minimax_rec(struct graph_t** graph, struct queens_t** queens, struct move_t move, int is_current_player, uint depth, uint max_depth, int alpha, int beta, double (*heuristic)(struct graph_t* graph, struct queens_t* queens)) {
    copy_graph_and_queens(graph[depth + 1], queens[depth + 1], graph[depth], queens[depth]);
    struct graph_t* g_copy = graph[depth];
    struct queens_t* q_copy = queens[depth];
    if (!is_current_player)
        play_move(g_copy, q_copy, pi->player_id, move);
    else
        play_move(g_copy, q_copy, pc__get_other_player(pi), move);
    if (!depth || (depth < max_depth && game__is_over(g_copy, q_copy)))
        return (struct minimax_t){move, heuristic(g_copy, q_copy)};
    uint player_id = is_current_player ? pi->player_id : pc__get_other_player(pi);
    uint op_id = !is_current_player ? pi->player_id : pc__get_other_player(pi);
    struct minimax_t ret = (struct minimax_t){(struct move_t){-1, -1, -1}, is_current_player ? INT_MIN : INT_MAX};
    for (uint queen_id = 0; queen_id < q_copy->nb_queens; queen_id++) {
        uint queen_src = q_copy->array[player_id][queen_id];
        fill_possible_moves_queen(g_copy, q_copy, queen_src, queens_possible_moves[depth], RATIO_KEPT);
        for (uint i = 0; queens_possible_moves[depth][i] != UINT_MAX; i++) {
            uint queen_dst = queens_possible_moves[depth][i];
            fill_possible_moves_arrow(g_copy, q_copy, queen_dst, queen_src, arrow_possible_moves[depth], op_id);
            for (uint j = 0; arrow_possible_moves[depth][j] != UINT_MAX; j++) {
                uint arrow_dst = arrow_possible_moves[depth][j];
                struct move_t next_move = (struct move_t){queen_src, queen_dst, arrow_dst};
                struct minimax_t h = {next_move, minimax_rec(graph, queens, next_move, !is_current_player, depth - 1, max_depth, alpha, beta, heuristic).value};
                if (is_current_player) {
                    ret = max(h, ret);
                    if (h.value >= beta)
                        return h;
                    if (h.value >= alpha)
                        alpha = h.value;
                } else {
                    ret = min(h, ret);
                    if (alpha >= h.value)
                        return h;
                    if (h.value <= beta)
                        beta = h.value;
                }
            }
        }
    }
    return ret;
}

//Allocate and free every used array in minimax and apply it
static struct move_t alphabeta(struct move_t move, uint depth, double (*heuristic)(struct graph_t* graph, struct queens_t* queens)) {
    struct graph_t** graph_tab = malloc(sizeof(struct graph_t*) * (depth + 2));
    struct queens_t** queens_tab = malloc(sizeof(struct graph_t*) * (depth + 2));
    queens_possible_moves = malloc(sizeof(uint*) * (depth + 1));
    arrow_possible_moves = malloc(sizeof(uint*) * (depth + 1));
    for (uint i = 0; i <= depth; i++) {
        graph_tab[i] = graph__copy(pi->board);
        queens_tab[i] = malloc(sizeof(struct queens_t));
        queens__alloc(queens_tab[i], pi->queens->nb_queens);
        queens_possible_moves[i] = malloc(sizeof(uint) * pi->board->num_vertices);
        arrow_possible_moves[i] = malloc(sizeof(uint) * pi->board->num_vertices);
    }
    graph_tab[depth + 1] = pi->board;
    queens_tab[depth + 1] = pi->queens;

    struct minimax_t m = minimax_rec(graph_tab, queens_tab, move, 1, depth, depth, INT_MIN, INT_MAX, heuristic);

    for (uint i = 0; i <= depth; i++) {
        graph__free(graph_tab[i]);
        queens__free(queens_tab[i]);
        free(queens_possible_moves[i]);
        free(arrow_possible_moves[i]);
    }
    free(graph_tab);
    free(queens_tab);
    free(queens_possible_moves);
    free(arrow_possible_moves);
    return m.move;
}

char const* get_player_name() { return __PLAYER_NAME; }

void initialize(unsigned int player_id, struct graph_t* graph, unsigned int num_queens, unsigned int* queens[NUM_PLAYERS]) {
    pi = pc__init(player_id, graph, num_queens, queens);
}

struct move_t play(struct move_t previous_move) {
    // printf("Depth : %u\n", get_optimal_depth());
    struct move_t move = alphabeta(previous_move, get_optimal_depth(), simple_heuristic);
    // printf("Move : %u %u %u\n", move.queen_src, move.queen_dst, move.arrow_dst);
    pc__play_op_move(pi, previous_move);
    pc__play_my_move(pi, move);
    return move;
}

void finalize() { pc__free(pi); }
