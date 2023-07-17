#ifndef __TESTS_FUNCTIONS_H__
#define __TESTS_FUNCTIONS_H__

#include "tests_utils.h"

/* Game test functions */

struct tests__functions tests__get_game_tests();

void tests__game__new();
void tests__game__init();
void tests__game__delete();
void tests__game__is_over();
void tests__game__next_player();
void tests__game__current_player();
void tests__game__default_config();
void tests__game__play();
void tests__game__get_winner();

/* Shape test functions */

struct tests__functions tests__get_shape_tests();

void tests__shape__new();
void tests__shape__init();
void tests__shape__delete();
void tests__shape__init_graph();
void tests__shape__get_board_shape();
void tests__shape__get_size();

/* Graph test functions */

struct tests__functions tests__get_graph_tests();

void tests__graph__init();
void tests__graph__free();
void tests__graph__copy();
void tests__graph__compress();

/* Queens tests functions */

struct tests__functions tests__get_queens_tests();

void tests__queens__new();
void tests__queens__alloc();
void tests__queens__init();
void tests__queens__copy();
void tests__queens__move();
void tests__queens__free();

#endif // __TESTS_FUNCTIONS_H__