/**
 * @file move.h
 * @brief This file contains the declarations of functions and data types used to represent and manipulate moves in the game.
 */

#ifndef _AMAZON_MOVE_H_
#define _AMAZON_MOVE_H_

#include <limits.h>
#include <stddef.h>

#include "graph.h"
#include "queens.h"
#include "utils.h"

struct move_t {
    unsigned int queen_src; // The id of the cell from where the queen moved
    unsigned int queen_dst; // The id of the cell where the queen moved to
    unsigned int arrow_dst; // The id of the cell where the arrow fell
};

enum { MOVE_REGULAR,
       MOVE_INVALID,
       MOVE_INVALID_ARROW_MISPLACED,
       MOVE_INVALID_QUEEN_MISPLACED };

/**
 * @brief Get the initial move, which is defined as a move with all three fields set to UINT_MAX.
 *
 * @return A move struct representing the initial move.
 */
struct move_t create_initial_move();

/**
 * @brief Check whether a given move is the initial move.
 *
 * @param move The move to check.
 * @return 1 if the move is the initial move, 0 otherwise.
 */
int is_initial_move(struct move_t move);

/**
 * @brief Check whether there is an edge between two positions on the board.
 *
 * @param board The game board.
 * @param queens An array of the current positions of the queens on the board.
 * @param pos1 The first position to check for an edge.
 * @param pos2 The second position to check for an edge.
 *
 * @return 1 if an edge exists between pos1 and pos2, 0 otherwise.
 */
int can_reach_position(struct graph_t* board, struct queens_t* queens, uint pos1, uint pos2);

/**
 * @brief Check if a move is valid.
 *
 * @param board The game board.
 * @param queens The queens placement on the board.
 * @param player_id The id of the player making the move.
 * @param id_src The id of the source position.
 * @param id_dst The id of the destination position.
 * @param is_arrow Indicates whether the move is an arrow shot.
 *
 * @return 1 if the move is valid, 0 otherwise.
 */
int is_valid_move_for_player(struct graph_t* board, struct queens_t* queens, uint player_id, uint id_src, uint id_dst, int is_arrow);

/**
 * @brief Move a queen piece in a queens_t structure.
 *
 * @param queens The queens_t structure containing the queen piece to move.
 * @param player_id The ID of the player moving the queen piece.
 * @param move The move_t structure containing the move to make.
 */
void move_queen(struct queens_t* queens, uint player_id, struct move_t move);

/**
 * @brief Check if a queen at a certain position can move to a neighboring empty cell.
 *
 * @param board Pointer to the game board.
 * @param queens Array of arrays representing the positions of the queens for each player.
 * @param pos Position of the queen to check.
 *
 * @return 1 if the queen can move to a neighboring empty cell, and 0 otherwise.
 */
int can_move(struct graph_t* board, struct queens_t* queens, uint pos);

#endif // _AMAZON_MOVE_H_