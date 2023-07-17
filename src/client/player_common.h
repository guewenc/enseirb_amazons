/**
 * @file player_common.h
 * @brief Defines functions and structures used by the player in the game.
 */

#ifndef __PLAYER_COMMON_H__
#define __PLAYER_COMMON_H__

#include "graph.h"
#include "move.h"
#include "player.h"
#include "queens.h"
#include "utils.h"

/**
 * @brief Struct containing information for a player.
 */
struct pc__player_info {
    unsigned int player_id; /**< ID of the player. */
    struct graph_t* board; /**< Pointer to the game board graph. */
    struct queens_t* queens; /**< Pointer to the player's queen positions. */
    unsigned int nb_turn; /**< Number of turns played by the player. */
};

/**
 * @brief Initializes a player with a given player ID, graph, number of queens, and their positions.
 *
 * @param player_id ID of the player.
 * @param graph Pointer to the game board graph.
 * @param num_queens Number of queens for each player.
 * @param queens Positions of the queens for each player.
 * @return Pointer to the player information struct.
 */
struct pc__player_info* pc__init(unsigned int player_id,
                                 struct graph_t* graph,
                                 unsigned int num_queens,
                                 unsigned int* queens[NUM_PLAYERS]);

/**
 * @brief Returns the ID of the other player.
 *
 * @param pi Pointer to the player information struct.
 * @return ID of the other player.
 */
uint pc__get_other_player(struct pc__player_info* pi);

/**
 * @brief Frees memory used by the player information struct and associated resources.
 *
 * @param pi Pointer to the player information struct.
 */
void pc__free(struct pc__player_info* pi);

/**
 * @brief Plays the given move for the player.
 *
 * @param pi Pointer to the player information struct.
 * @param m Move to play.
 */
void pc__play_my_move(struct pc__player_info* pi, struct move_t m);

/**
 * @brief Plays the given move for the opponent player.
 *
 * @param pi Pointer to the player information struct.
 * @param m Move to play.
 */
void pc__play_op_move(struct pc__player_info* pi, struct move_t m);

/**
 * @brief Selects a random queen that can be moved by the player.
 *
 * @param pi Pointer to the player information struct.
 * @return Index of the selected queen.
 */
uint select_random_queen(struct pc__player_info* pi);

/**
 * @brief Selects a random index from the given array based on weights and count.
 *
 * @param array Array of weights for each index.
 * @param size Size of the array.
 * @param count Number of indices with non-zero weight.
 * @return Randomly selected index.
 */
uint select_random_from(uint* array, uint size, uint count);

/**
 * @brief Selects the index of the maximum weight in the given array.
 *
 * @param array Array of weights for each index.
 * @param size Size of the array.
 * @param count Number of indices with non-zero weight.
 * @return Index of the maximum weight.
 */
uint select_max_from(uint* array, uint size, uint count);

/**
 * @brief Counts the number of possible moves for the given queen on the board.
 *
 * @param pi Pointer to the player information struct.
 * @param queen_src Index of the queen to check.
 * @return Number of possible moves for the queen.
 */
uint possible_moves(struct pc__player_info* pi, uint queen_src);

#endif // __PLAYER_COMMON_H__
