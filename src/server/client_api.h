/**
 * @file client_api.h
 * @brief A library for loading and using client player implementations.
 */

#ifndef __CLIENT_API_H__
#define __CLIENT_API_H__

#include "graph.h"
#include "move.h"

struct player;

/**
 * @brief Load a client shared library from a given path and retrieve its functions.
 *
 * @param client_path The path to the client shared library.
 * @return A pointer to a struct containing pointers to the loaded functions, or NULL on error.
 */
struct player* client__load(const char* client_path);

/**
 * @brief Unload a previously loaded client shared library and free its memory.
 *
 * @param p A pointer to the struct containing the loaded client library.
 */
void client__unload(struct player* p);

/**
 * @brief Get the name of a loaded client library.
 *
 * @param p A pointer to the struct containing the loaded client library.
 * @return The name of the loaded client library.
 */
char const* client__get_player_name(const struct player* p);

/**
 * @brief Initialize a loaded client library with the given parameters.
 *
 * @param p A pointer to the struct containing the loaded client library.
 * @param player_id The ID of the player being initialized.
 * @param graph The game board graph.
 * @param num_queens The number of queens for each player.
 * @param queens An array of queen positions for each player.
 */
void client__initialize(struct player* p, uint player_id, struct graph_t* graph, uint num_queens, uint** queens);

/**
 * @brief Let a loaded client library make a move.
 *
 * @param p A pointer to the struct containing the loaded client library.
 * @param previous_move The previous move made by the other player.
 * @return The move made by the loaded client library.
 */
struct move_t client__play(struct player* p, struct move_t previous_move);

/**
 * @brief Finalize a loaded client library.
 *
 * @param p A pointer to the struct containing the loaded client library.
 */
void client__finalize(struct player* p);

#endif // __CLIENT_API_H__