#include "shape.h"
#include "dir.h"
#include "utils.h"

#define SHAPE__DEFAULT_S_CLOVER 15
#define SHAPE__DEFAULT_S_DONUT 9
#define SHAPE__DEFAULT_S_EIGHT 12
#define SHAPE__DEFAULT_S_SQUARE 8

typedef void (*add_edge_f)(uint, uint, uint, uint, enum dir_t, uint, struct graph_t*);

struct shape_t {
    uint size;
    enum board_shape board_shape;
    add_edge_f add_edge;
};

// Returns the index of an element in a (n,m) array represented in an 1D array of size n*m
uint get_index(uint i, uint j, uint m) {
    return i * m + j;
}

// Returns i in [ a; b [
static int is_between(uint i, uint a, uint b) {
    return i >= a && i < b;
}

// Returns point (i,j) is inside the square of corners (a,a) and (b,b)
static int is_inside(uint i, uint j, uint a, uint b) {
    return is_between(i, a, b) && is_between(j, a, b);
}

// Returns i1 is between a and b or i2 is between a and b
static int is_between_both_or(uint i1, uint i2, uint a, uint b) {
    return is_between(i1, a, b) || is_between(i2, a, b);
}

// Returns i1 is between a and b and i2 is between a and b
static int is_between_both_and(uint i1, uint i2, uint a, uint b) {
    return is_between(i1, a, b) && is_between(i2, a, b);
}

// Returns point (i1,j1) or ((i2,j2)) is inside the square of corners (a,a) and (b,b)
static int is_inside_both(uint i1, uint j1, uint i2, uint j2, uint a, uint b) {
    return is_inside(i1, j1, a, b) || is_inside(i2, j2, a, b);
}

// Checks if a board shape is valid
int is_valid_shape(char board_shape) {
    enum board_shape bs[] = {SHAPE_CLOVER, SHAPE_DONUT, SHAPE_EIGHT, SHAPE_SQUARE};
    for (int i = 0; i < SHAPE_COUNT; i++)
        if ((int)bs[i] == board_shape)
            return 1;
    return 0;
}

// Initializes size for a clover-shaped board
uint init_size_clover(uint size) {
    return !(size % 5) ? size : SHAPE__DEFAULT_S_CLOVER;
}

// Initializes size for a donut-shaped board
uint init_size_donut(uint size) {
    return !(size % 3) ? size : SHAPE__DEFAULT_S_DONUT;
}

// Initializes size for an eight-shaped board
uint init_size_eight(uint size) {
    return !(size % 4) ? size : SHAPE__DEFAULT_S_EIGHT;
}

// Initializes size for a square-shaped board
uint init_size_square(uint size) { return size; }

// Adds an edge between two vertices in a square board
void add_edge_square(uint i1, uint j1, uint i2, uint j2, enum dir_t d, uint m, struct graph_t* b) {
    gsl_spmatrix_uint_set(b->t, get_index(i1, j1, m), get_index(i2, j2, m), d);
}

// Adds an edge between two vertices in a donut board
void add_edge_donut(uint i1, uint j1, uint i2, uint j2, enum dir_t d, uint m, struct graph_t* b) {
    uint square_size = m / 3;
    // Adds the edge if both vertices are not inside the inner square
    if (!is_inside_both(i1, j1, i2, j2, square_size, square_size * 2))
        gsl_spmatrix_uint_set(b->t, get_index(i1, j1, m), get_index(i2, j2, m), d);
}

// Adds an edge between two vertices in a clover board
void add_edge_clover(uint i1, uint j1, uint i2, uint j2, enum dir_t d, uint m, struct graph_t* b) {
    uint square_size = m / 5;
    // Adds the edge if one of the vertices is not inside the outer squares or if both are inside the center square
    if (!is_inside_both(i1, j1, i2, j2, square_size, square_size * 4) ||
        is_between_both_and(i1, i2, square_size * 2, square_size * 3) ||
        is_between_both_and(j1, j2, square_size * 2, square_size * 3))
        gsl_spmatrix_uint_set(b->t, get_index(i1, j1, m), get_index(i2, j2, m), d);
}

// Add an edge between two vertices in an eight board
void add_edge_eight(uint i1, uint j1, uint i2, uint j2, enum dir_t d, uint m, struct graph_t* b) {
    uint square_size = m / 4;
    // Adds the edge if the vertices are not on the horizontal or vertical axis of the center square
    if (!(is_between_both_or(i1, i2, square_size, square_size * 2) &&
          is_between_both_or(j1, j2, square_size * 2, square_size * 3)) &&
        !(is_between_both_or(i1, i2, square_size * 2, square_size * 3) &&
          is_between_both_or(j1, j2, square_size, square_size * 2)))
        gsl_spmatrix_uint_set(b->t, get_index(i1, j1, m), get_index(i2, j2, m), d);
}

/* ************************************************** */

struct shape_t* shape__new() {
    struct shape_t* new_shape = malloc(sizeof(struct shape_t));
    if (!new_shape)
        handle_error(__func__, "Not enough memory", 1);
    return new_shape;
}

void shape__init(struct shape_t* s, uint size, char board_shape) {
    uint (*init_size)(uint) = NULL;

    s->board_shape = board_shape;

    if (!is_valid_shape(board_shape))
        board_shape = SHAPE__DEFAULT_SHAPE;
    if (size < 5)
        size = SHAPE__DEFAULT_SIZE;

    switch (s->board_shape) {
        case SHAPE_SQUARE:
            init_size = init_size_square;
            s->add_edge = add_edge_square;
            break;
        case SHAPE_DONUT:
            init_size = init_size_donut;
            s->add_edge = add_edge_donut;
            break;
        case SHAPE_CLOVER:
            init_size = init_size_clover;
            s->add_edge = add_edge_clover;
            break;
        case SHAPE_EIGHT:
            init_size = init_size_eight;
            s->add_edge = add_edge_eight;
            break;
        default:
            handle_error(__func__, "Should never happen", 1);
            break;
    }

    s->size = init_size(size);
}

void shape__init_graph(struct shape_t* s, struct graph_t* b) {
    uint size = s->size;
    for (uint i = 0; i < size; i++) {
        for (uint j = 0; j < size; j++) {
            if (i > 0)
                s->add_edge(i, j, i - 1, j, DIR_NORTH, size, b);
            if (i < size - 1)
                s->add_edge(i, j, i + 1, j, DIR_SOUTH, size, b);
            if (j > 0)
                s->add_edge(i, j, i, j - 1, DIR_WEST, size, b);
            if (j < size - 1)
                s->add_edge(i, j, i, j + 1, DIR_EAST, size, b);
            if (i > 0 && j > 0)
                s->add_edge(i, j, i - 1, j - 1, DIR_NW, size, b);
            if (i < size - 1 && j < size - 1)
                s->add_edge(i, j, i + 1, j + 1, DIR_SE, size, b);
            if (j > 0 && i < size - 1)
                s->add_edge(i, j, i + 1, j - 1, DIR_SW, size, b);
            if (j < size - 1 && i > 0)
                s->add_edge(i, j, i - 1, j + 1, DIR_NE, size, b);
        }
    }

    if (s->board_shape == SHAPE_EIGHT) {
        gsl_spmatrix_uint_set(b->t, get_index(size / 2, size / 2, size), get_index(size / 2 - 1, size / 2 - 1, size), DIR_NW);
        gsl_spmatrix_uint_set(b->t, get_index(size / 2 - 1, size / 2 - 1, size), get_index(size / 2, size / 2, size), DIR_SE);
    }
}

enum board_shape shape__get_board_shape(struct shape_t* s) {
    return s->board_shape;
}

uint shape__get_size(struct shape_t* s) { return s->size; }

void shape__delete(struct shape_t* s) { free(s); }