/**
 * @file queens.h
 * @brief Header file for the queen pieces of the game.
 */

#ifndef __QUEENS_H__
#define __QUEENS_H__

#include "utils.h"

/**
 * @struct queens_t
 * @brief A structure to hold the queen pieces of the game.
 *
 * This structure contains an array of arrays of uints to store the positions
 * of each player's queen pieces on the board.
 */
struct queens_t {
    uint nb_queens; /* The number of queens per player */
    uint* array[2]; /** Array of arrays of uints to store the positions of each player's queens */
};

/**
 * @brief Get the number of queens per player.
 *
 * @param queens Pointer to a `queens_t` structure containing the queens array.
 * @return The number of queens per player.
 */
uint queens__get_nb_queens(struct queens_t* queens);

/**
 * @brief Get the queens array.
 *
 * @param queens Pointer to a `queens_t` structure containing the queens array.
 * @return A pointer to the queens array.
 */
uint** queens__get_array(struct queens_t* queens);

/**
 * @brief Create a new queens_t structure and return its pointer.
 *
 * @return A pointer to a newly created queens_t structure.
 */
struct queens_t* queens__new();

/**
 * @brief Allocate memory for the queen pieces of a queens_t structure.
 *
 * @param queens The queens_t structure to allocate memory for.
 */
void queens__alloc(struct queens_t* queens, uint nb_queens);

/**
 * @brief Initialize the queen pieces of a queens_t structure.
 *
 * @param queens The queens_t structure to initialize.
 * @param size The size of the game board.
 */
void queens__init(struct queens_t* queens, uint size);

/**
 * @brief Copy the queen pieces of a queens_t structure to another array of arrays of uints.
 *
 * @param queens_src The queens_t structure to copy the queen pieces from.
 * @param queens_dst The array of arrays of uints to copy the queen pieces to.
 */
void queens__copy(struct queens_t* queens_src, struct queens_t* queens_dst);

/**
 * @brief Free the memory allocated for the queen pieces of a queens_t structure.
 *
 * @param queens The queens_t structure to free the memory for.
 */
void queens__free(struct queens_t* queens);

/**
 * @brief Check if a queen piece exists on the board for a given player.
 *
 * @param queens The queens_t structure containing the queen pieces.
 * @param player_id The ID of the player to check for.
 * @param idx The index to check for the queen piece at.
 * @return 1 if the queen piece exists for the player, 0 otherwise.
 */
int queens__queen_exist_for_player(struct queens_t* queens, uint player_id, uint idx);

/**
 * @brief Check if a queen piece exists on the board for any player.
 *
 * @param queens The queens_t structure containing the queen pieces.
 * @param idx The index to check for the queen piece at.
 * @return 1 if the queen piece exists for any player, 0 otherwise.
 */
int queens__exist_queens(struct queens_t* queens, uint idx);

#endif // __QUEENS_H__