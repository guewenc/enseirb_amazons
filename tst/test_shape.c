#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "shape.h"
#include "tests_functions.h"
#include "tests_utils.h"

struct func_block tests_list_shape[] = {
    {tests__shape__new, "shape__new"},
    {tests__shape__init, "shape__init"},
    {tests__shape__delete, "shape__delete"},
    {tests__shape__init_graph, "shape__init_graph"},
    {tests__shape__get_board_shape, "shape__get_board_shape"},
    {tests__shape__get_size, "shape__get_size"},
};

struct tests__functions tests__get_shape_tests() {
    return (struct tests__functions){6, tests_list_shape};
}

void tests__shape__new() {
    struct shape_t* s = shape__new();
    assert(s != NULL);
    shape__delete(s);
}

void tests__shape__init() {
    struct shape_t* s = shape__new();
    shape__init(s, 16, SHAPE_SQUARE);
    assert(shape__get_size(s) == 16);
    shape__init(s, 18, SHAPE_DONUT);
    assert(shape__get_size(s) == 18);
    shape__init(s, 17, SHAPE_DONUT);
    assert(shape__get_size(s) == 9);
    shape__init(s, 17, SHAPE_CLOVER);
    assert(shape__get_size(s) == 15);
    shape__init(s, 20, SHAPE_CLOVER);
    assert(shape__get_size(s) == 20);
    shape__init(s, 17, SHAPE_EIGHT);
    assert(shape__get_size(s) == 12);
    shape__init(s, 20, SHAPE_EIGHT);
    assert(shape__get_size(s) == 20);
    shape__init(s, 2, SHAPE_EIGHT);
    assert(shape__get_size(s) == 8);
    shape__delete(s);
}

void tests__shape__delete() {
    struct shape_t* s = shape__new();
    shape__delete(s);
}

void tests__shape__init_graph() {
}

void tests__shape__get_board_shape() {
    struct shape_t* s = shape__new();
    shape__init(s, 16, SHAPE_SQUARE);
    assert(shape__get_board_shape(s) == SHAPE_SQUARE);
    shape__delete(s);
}

void tests__shape__get_size() {
    struct shape_t* s = shape__new();
    shape__init(s, 16, SHAPE_SQUARE);
    assert(shape__get_size(s) == 16);
    shape__delete(s);
}
