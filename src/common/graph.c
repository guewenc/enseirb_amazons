#include <limits.h>
#include <math.h>

#include "graph.h"

struct graph_t* graph__new() {
    struct graph_t* new_b = malloc(sizeof(struct graph_t));
    if (!new_b)
        handle_error(__func__, "Not enough memory for 'new_b'", PROGRAM_EXIT);
    return new_b;
}

void graph__init(struct graph_t* board, uint num_vertices) {
    board->num_vertices = num_vertices;
    board->t = gsl_spmatrix_uint_alloc(num_vertices, num_vertices);
}

void graph__compress(struct graph_t* board) {
    gsl_spmatrix_uint* tmp = board->t;
    board->t = gsl_spmatrix_uint_compress(board->t, GSL_SPMATRIX_CSR);
    gsl_spmatrix_uint_free(tmp);
}

struct graph_t* graph__copy(struct graph_t* board) {
    struct graph_t* new_copy = graph__new();
    graph__init(new_copy, board->num_vertices);
    graph__compress(new_copy);
    gsl_spmatrix_uint_memcpy(new_copy->t, board->t);
    return new_copy;
}

void graph__free(struct graph_t* board) {
    if (board) {
        gsl_spmatrix_uint_free(board->t);
    }
    free(board);
    board = NULL;
}

static inline void graph__replace_neighbors(struct graph_t* board, uint x, uint size, int to_no_dir) {
    enum dir_t dirs[] = {DIR_SOUTH, DIR_NORTH, DIR_EAST, DIR_WEST, DIR_SE, DIR_NW, DIR_NE, DIR_SW};
    uint i = x / size, j = x % size;
    uint* ptr = NULL;
    if (to_no_dir)
        for (uint i = 0; i < NUM_DIRS; i++)
            dirs[i] = NO_DIR;
    if (i > 0 && (ptr = gsl_spmatrix_uint_ptr(board->t, x - size, x)))
        *ptr = dirs[0];
    if (i < size - 1 && (ptr = gsl_spmatrix_uint_ptr(board->t, x + size, x)))
        *ptr = dirs[1];
    if (j > 0 && (ptr = gsl_spmatrix_uint_ptr(board->t, x - 1, x)))
        *ptr = dirs[2];
    if (j < size - 1 && (ptr = gsl_spmatrix_uint_ptr(board->t, x + 1, x)))
        *ptr = dirs[3];
    if (i > 0 && j > 0 && (ptr = gsl_spmatrix_uint_ptr(board->t, x - size - 1, x)))
        *ptr = dirs[4];
    if (i < size - 1 && j < size - 1 &&
        (ptr = gsl_spmatrix_uint_ptr(board->t, x + size + 1, x)))
        *ptr = dirs[5];
    if (j > 0 && i < size - 1 && (ptr = gsl_spmatrix_uint_ptr(board->t, x - 1 + size, x)))
        *ptr = dirs[6];
    if (j < size - 1 && i > 0 && (ptr = gsl_spmatrix_uint_ptr(board->t, x + 1 - size, x)))
        *ptr = dirs[7];
}

void graph__disconnect(struct graph_t* board, uint pos) {
    graph__replace_neighbors(board, pos, (int)sqrt(board->num_vertices), 1);
    for (int k = board->t->p[pos]; k < board->t->p[pos + 1]; k++) {
        board->t->data[k] = NO_DIR;
    }
}

// Returns the neighbor in direction d, UINT_MAX if there is none
uint graph__get_neighbor(struct graph_t* g, uint pos, enum dir_t d) {
    if (pos == UINT_MAX)
        return pos;
    for (int k = g->t->p[pos]; k < g->t->p[pos + 1]; k++) {
        if (g->t->data[k] == d)
            return g->t->i[k];
    }
    return UINT_MAX;
}

// Returns 1 if pos is isolated 0 otherwise
int is_isolated(struct graph_t* g, uint pos) {
    if (pos == UINT_MAX)
        return 1;
    for (int k = g->t->p[pos]; k < g->t->p[pos + 1]; k++)
        if (g->t->data[k])
            return 0;
    return 1;
}
