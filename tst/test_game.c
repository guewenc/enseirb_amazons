#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "game.h"
#include "tests_functions.h"
#include "tests_utils.h"

struct func_block tests_list_game[] = {
    {tests__game__new, "game__new"},
    {tests__game__init, "game__init"},
    {tests__game__delete, "game__delete"},
    {tests__game__is_over, "game__is_over"},
    {tests__game__next_player, "game__next_player"},
    {tests__game__current_player, "game__get_current_player"},
    {tests__game__default_config, "game__default_config"},
    {tests__game__play, "game__play"},
    {tests__game__get_winner, "game__get_winner"}};

struct tests__functions tests__get_game_tests() {
    return (struct tests__functions){9, tests_list_game};
}

void tests__game__new() {
    game g = game__new();
    assert(g != NULL);
    free(g);
}

void tests__game__init() {
    game g = game__new();
    struct game_config config = {
        .size = 10,
        .starting_player = PLAYER_1,
        .seed = 0,
        .board_shape = SHAPE_CLOVER,
    };
    game__init(g, &config, "./install/hagrid.so", "./install/heroine.so");
    game__delete(g);
}

void tests__game__delete() {
    game g = game__new();
    struct game_config config = {
        .size = 10,
        .starting_player = PLAYER_1,
        .seed = 10,
        .board_shape = SHAPE_CLOVER,
    };
    game__init(g, &config, "./install/hagrid.so", "./install/heroine.so");
    game__delete(g);
}

void tests__game__is_over() {
    game g = game__new();
    struct game_config config = {
        .size = 10,
        .starting_player = PLAYER_1,
        .seed = 0,
        .board_shape = SHAPE_CLOVER,
    };
    game__init(g, &config, "./install/hagrid.so", "./install/heroine.so");
    assert(!game__is_over(g));
    game__delete(g);
}

void tests__game__next_player() {
    game g = game__new();
    struct game_config config = {
        .size = 10,
        .starting_player = PLAYER_1,
        .seed = 0,
        .board_shape = SHAPE_CLOVER,
    };
    game__init(g, &config, "./install/hagrid.so", "./install/heroine.so");
    assert(game__get_current_player(g) == PLAYER_1);
    game__next_player(g);
    assert(game__get_current_player(g) == PLAYER_2);
    game__next_player(g);
    assert(game__get_current_player(g) == PLAYER_1);
    game__delete(g);
}

void tests__game__current_player() {
    game g = game__new();
    struct game_config config = {
        .size = 10,
        .starting_player = PLAYER_1,
        .seed = 0,
        .board_shape = SHAPE_CLOVER,
    };
    game__init(g, &config, "./install/hagrid.so", "./install/heroine.so");
    assert(game__get_current_player(g) == PLAYER_1);
    game__delete(g);
}

void tests__game__default_config() {
    game g = game__new();
    struct game_config c = game__default_config();
    game__init(g, &c, "./install/hagrid.so", "./install/heroine.so");
    game__delete(g);
}

void tests__game__play() {
    game g = game__new();
    struct game_config config = {
        .size = 10,
        .starting_player = PLAYER_1,
        .seed = 0,
        .board_shape = SHAPE_CLOVER,
    };
    game__init(g, &config, "./install/hagrid.so", "./install/heroine.so");
    game__play(g);
    game__delete(g);
}

void tests__game__get_winner() {
    game g = game__new();
    struct game_config config = {
        .size = 10,
        .starting_player = PLAYER_1,
        .seed = 0,
        .board_shape = SHAPE_CLOVER,
    };
    game__init(g, &config, "./install/hagrid.so", "./install/heroine.so");
    assert(game__get_winner(g) == UNDEFINED_PLAYER);
    game__delete(g);
}
