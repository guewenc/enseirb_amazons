#include <assert.h>
#include <limits.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "client_api.h"
#include "dir.h"
#include "game.h"
#include "graph.h"
#include "player.h"
#include "queens.h"
#include "shape.h"
#include "utils.h"

struct game_s {
    enum player_n p_winner;
    enum player_n current_player;
    struct move_t previous_move;
    struct graph_t* board;
    struct shape_t* shape;
    struct player* players[2];
    struct queens_t* queens;
};

static int check_move(cgame g, struct move_t m) {
    int is_valid_queen_move = is_valid_move_for_player(g->board, g->queens, g->current_player, m.queen_src, m.queen_dst, 0);
    int is_valid_arrow_move = is_valid_move_for_player(g->board, g->queens, g->current_player, m.queen_dst, m.arrow_dst, 1) || m.queen_src == m.arrow_dst;

    if (!is_valid_queen_move) {
        return MOVE_INVALID_QUEEN_MISPLACED;
    } else if (!is_valid_arrow_move) {
        return MOVE_INVALID_ARROW_MISPLACED;
    }
    return MOVE_REGULAR;
}

static void board_init(game game) {
    uint board_size = shape__get_size(game->shape);
    graph__init(game->board, board_size * board_size);
    shape__init_graph(game->shape, game->board);
    graph__compress(game->board);
}

static void board_update(game game, struct move_t move) {
    move_queen(game->queens, game->current_player, move);
    graph__disconnect(game->board, move.arrow_dst);
}

static enum player_n get_opposing_player_id(enum player_n player_id) {
    return player_id ^ 1;
}

static void update_winner(game game, enum player_n player_id) {
    game->p_winner = player_id;
}

/* **************************************************************** */

struct game_config game__default_config() {
    uint default_board_size = SHAPE__DEFAULT_SIZE;
    enum player_n default_starting_player = UNDEFINED_PLAYER;
    enum board_shape default_board_shape = SHAPE__DEFAULT_SHAPE;
    int default_seed = -1;

    return (struct game_config){default_board_size, default_starting_player, default_board_shape, default_seed};
}

game game__new() {
    game new_game = malloc(sizeof(struct game_s));
    if (!new_game)
        handle_error(__func__, "Not enough memory for 'g'", PROGRAM_EXIT);
    return new_game;
}

void game__init(game game, struct game_config* game_config, const char* lib_player1, const char* lib_player2) {
    if (!game) handle_error(__func__, "Invalid parameter 'g'", PROGRAM_EXIT);
    if (!game_config) handle_error(__func__, "Invalid parameter 'gc'", PROGRAM_EXIT);
    if (!lib_player1) handle_error(__func__, "Invalid parameter 'p1'", PROGRAM_EXIT);
    if (!lib_player2) handle_error(__func__, "Invalid parameter 'p2'", PROGRAM_EXIT);

    srand(game_config->seed < 0 ? time(NULL) : game_config->seed); /* Initialization of random seed */

    game->shape = shape__new();
    game->board = graph__new();
    game->queens = queens__new();

    shape__init(game->shape, game_config->size, game_config->board_shape);
    board_init(game);

    /* Initialization of initial data */
    game->current_player = game_config->starting_player == UNDEFINED_PLAYER ? rand() % 2 : game_config->starting_player;
    update_winner(game, UNDEFINED_PLAYER);
    game->previous_move = create_initial_move();

    /* Allocation and creation of copies of queens */
    uint nb_queens = 4 * ((int)(shape__get_size(game->shape) / 10 + 1));
    queens__alloc(game->queens, nb_queens);
    queens__init(game->queens, shape__get_size(game->shape));

    uint** queens_players[NUM_PLAYERS];

    struct queens_t* queens_player1 = queens__new();
    struct queens_t* queens_player2 = queens__new();
    queens__copy(game->queens, queens_player1);
    queens__copy(game->queens, queens_player2);
    queens_players[PLAYER_1] = queens__get_array(queens_player1);
    queens_players[PLAYER_2] = queens__get_array(queens_player2);

    /* Launching clients when the server is operational */
    game->players[0] = client__load(lib_player1);
    game->players[1] = client__load(lib_player2);

    /* Initialization of clients */
    for (enum player_n player_id = 0; player_id < NUM_PLAYERS; player_id++)
        client__initialize(game->players[player_id], player_id, graph__copy(game->board), queens__get_nb_queens(game->queens), queens_players[player_id]);

    free(queens_player1);
    free(queens_player2);
}

void game__delete(game game) {
    if (game) { // Checks if game instance exists and is not null.
        /* Freeing the clients */
        client__finalize(game->players[0]);
        client__finalize(game->players[1]);

        /* Unloading the dynamic client library */
        client__unload(game->players[0]);
        client__unload(game->players[1]);

        /* Finally, freeing the graph, queen's array and the shape */
        graph__free(game->board);
        queens__free(game->queens);
        shape__delete(game->shape);
    }
    free(game); /* Free the game */
    game = NULL;
}

int game__is_over(game game) {
    if (!game) handle_error(__func__, "Invalid parameter 'g'", PROGRAM_EXIT);

    /* This scenario occurs only when an invalid move has been made */
    if (game->p_winner != UNDEFINED_PLAYER)
        return 1;

    /* Verify if a queen is trapped or non-existent */
    for (uint player_id = 0; player_id < NUM_PLAYERS; player_id++) {
        int is_queen_moveable = 0;
        for (uint queen_id = 0; queen_id < queens__get_nb_queens(game->queens); queen_id++) {
            if (can_move(game->board, game->queens, game->queens->array[player_id][queen_id])) {
                is_queen_moveable = 1;
            }
        }

        if (!is_queen_moveable) {
            // All the queens in the player cannot move. The other player wins the game.
            update_winner(game, get_opposing_player_id(player_id));
            return 1;
        }
    }

    // No one is blocked or has made a bad move: the game continues.
    return 0;
}

void game__next_player(game game) {
    if (!game) handle_error(__func__, "Invalid parameter 'g'", PROGRAM_EXIT);
    game->current_player = get_opposing_player_id(game->current_player);
}

void game__play(game game) {
    if (!game) handle_error(__func__, "Invalid parameter 'g'", PROGRAM_EXIT);

    struct move_t t_move = client__play(game->players[game->current_player], game->previous_move);

    /* If it is not a valid move, then the other player wins the game and the move is not played. */
    if (check_move(game, t_move) != MOVE_REGULAR) {
        update_winner(game, get_opposing_player_id(game->current_player));
        return;
    }

    board_update(game, t_move);
    game->previous_move = t_move;
}

enum player_n game__get_current_player(cgame game) {
    if (!game) handle_error(__func__, "Invalid parameter 'g'", PROGRAM_EXIT);
    return game->current_player;
}

enum player_n game__get_winner(cgame game) {
    if (!game) handle_error(__func__, "Invalid parameter 'g'", PROGRAM_EXIT);
    return game->p_winner;
}

const char* game__get_player_name(cgame game, enum player_n player_id) {
    if (player_id == UNDEFINED_PLAYER)
        return "Undefined player";
    return client__get_player_name(game->players[player_id]);
}

void game__export(cgame game, char* path) {
    FILE* file = fopen(path, "w");
    uint m = shape__get_size(game->shape);
    gsl_spmatrix_uint* csr = game->board->t;
    fprintf(file,
            "digraph G {\nnode [shape=square, width=0.5, height=0.5, "
            "style=filled, fillcolor=gray];\n");
    for (unsigned int i = 0; i < m; i++)
        for (unsigned int j = 0; j < m; j++)
            fprintf(file, "%u [pos=\"%u,%u!\"]\n", i * m + j, j, m - (i + 1));

    for (unsigned int i = 0; i < csr->size1; i++) {
        for (int k = csr->p[i]; k < csr->p[i + 1]; k++) {
            unsigned int j = csr->i[k];
            if (gsl_spmatrix_uint_get(csr, i, j))
                fprintf(file, "%u -> %u;\n", i, j);
        }
    }

    for (uint i = 0; i < 2; i++)
        for (uint j = 0; j < queens__get_nb_queens(game->queens); j++)
            if (game->queens->array[i][j] < game->board->num_vertices)
                fprintf(file, "%u [fillcolor=%s]\n", game->queens->array[i][j], i ? "blue" : "red");

    fprintf(file, "}");
    fclose(file);
}
