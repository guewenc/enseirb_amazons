/**
 * @file shape.h
 * @brief This file contains the declarations of functions and data types used to represent and manipulate the shape of a board
 */

#ifndef __SHAPE_H__
#define __SHAPE_H__

#include "graph.h"
#include "utils.h"

/**
 * @brief An enum representing the possible shapes of the board in a game.
 *
 * SHAPE_SQUARE represents a square board.
 * SHAPE_DONUT represents a donut-shaped board.
 * SHAPE_CLOVER represents a clover-shaped board.
 * SHAPE_EIGHT represents an eight-shaped board.
 * SHAPE_COUNT is the number of possible board shapes.
 */
enum board_shape {
    SHAPE_SQUARE = 'c',
    SHAPE_DONUT = 'd',
    SHAPE_CLOVER = 't',
    SHAPE_EIGHT = '8',
    SHAPE_COUNT = 4
};

#define SHAPE__DEFAULT_SHAPE SHAPE_SQUARE
#define SHAPE__DEFAULT_SIZE 8

struct shape_t;

/**
 * @brief Allocates and returns a new instance of shape_t
 *
 * @return A pointer to a new instance of shape_t
 */
struct shape_t* shape__new();

/**
 * @brief Initializes a shape_t instance with given parameters
 *
 * @param s A pointer to a shape_t instance
 * @param size The size of the game board
 * @param board_shape The shape of the game board
 */
void shape__init(struct shape_t* s, uint size, char board_shape);

/**
 * @brief Initializes the given graph according to the shape
 *
 * @param s A pointer to a shape_t instance
 * @param b A pointer to a graph_t instance
 */
void shape__init_graph(struct shape_t* s, struct graph_t* b);

/**
 * @brief Returns the board shape of the given shape_t instance
 *
 * @param s A pointer to a shape_t instance
 * @return The board shape of the shape_t instance
 */
enum board_shape shape__get_board_shape(struct shape_t* s);

/**
 * @brief Returns the size of the game board of the given shape_t instance
 *
 * @param s A pointer to a shape_t instance
 * @return The size of the game board of the shape_t instance
 */
uint shape__get_size(struct shape_t* s);

/**
 * @brief Deletes a shape_t instance
 *
 * @param s A pointer to a shape_t instance to delete
 */
void shape__delete(struct shape_t* s);

#endif /* __SHAPE_H__ */