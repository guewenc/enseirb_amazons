/**
 * @file game.h
 * @brief This file contains the declarations of functions and data types used to represent and manipulate the game.
 */

#ifndef __GAME_H__
#define __GAME_H__

#include "client_api.h"
#include "shape.h"

/**
 * @brief An enum representing the possible players in the game.
 *
 * UNDEFINED_PLAYER is used to represent an undefined player.
 * PLAYER_1 is the first player.
 * PLAYER_2 is the second player.
 */
enum player_n {
    UNDEFINED_PLAYER = -1,
    PLAYER_1 = 0,
    PLAYER_2 = 1,
};

/**
 * @brief A struct representing the configuration for a game.
 *
 * size is the size of the board.
 * starting_player is the player who starts the game.
 * board_shape is the shape of the board.
 * seed is the random seed used for the game.
 */
struct game_config {
    uint size;
    enum player_n starting_player;
    enum board_shape board_shape;
    int seed;
};

/**
 * @brief The structure pointer that stores the game state.
 **/
typedef struct game_s* game;

/**
 * @brief The structure constant pointer that stores the game state.
 * @details That means that it is not possible to modify the game using this
 * pointer.
 **/
typedef const struct game_s* cgame;

/**
 * @brief Returns a default configuration for the game.
 *
 * @return A game configuration struct.
 */
struct game_config game__default_config();

/**
 * @brief Allocates and initializes a new game instance.
 *
 * @return A new game instance.
 */
game game__new();

/**
 * @brief Initializes a game instance with a given configuration and player libraries.
 *
 * @param game The game instance to initialize.
 * @param game_config The game configuration to use.
 * @param lib_player1 The dynamic library for player 1.
 * @param lib_player2 The dynamic library for player 2.
 */
void game__init(game game, struct game_config* game_config, const char* lib_player1, const char* lib_player2);

/**
 * @brief Deallocates memory associated with a game instance.
 *
 * @param game The game instance to delete.
 */
void game__delete(game game);

/**
 * @brief Determines if the game is over.
 *
 * @param game The game instance to check.
 *
 * @return 1 if the game is over, 0 otherwise.
 */
int game__is_over(game game);

/**
 * @brief Switches to the next player's turn.
 *
 * @param game The game instance.
 */
void game__next_player(game game);

/**
 * @brief Plays a turn for the current player.
 *
 * @param game The game instance.
 */
void game__play(game game);

/**
 * @brief Gets the current player.
 *
 * @param game The game instance.
 *
 * @return The current player.
 */
enum player_n game__get_current_player(cgame game);

/**
 * @brief Gets the winner of the game.
 *
 * @param game The game instance.
 *
 * @return The winner of the game.
 */
enum player_n game__get_winner(cgame game);

/**
 * @brief Gets the name of a player.
 *
 * @param game The game instance.
 * @param player_id The ID of the player to get the name of.
 *
 * @return The name of the player.
 */
const char* game__get_player_name(cgame game, enum player_n player_id);

/**
 * @brief Exports the game to a file in DOT format.
 *
 * @param game The game instance.
 * @param path The path of the file to export to.
 */
void game__export(cgame game, char* path);

#endif // __GAME_H__