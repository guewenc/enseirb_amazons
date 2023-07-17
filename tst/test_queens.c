#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "move.h"
#include "queens.h"
#include "tests_functions.h"
#include "tests_utils.h"

struct func_block tests_list_queens[] = {
    {tests__queens__new, "queens__new"},
    {tests__queens__init, "queens__init"},
    {tests__queens__alloc, "queens__alloc"},
    {tests__queens__copy, "queens__copy"},
    {tests__queens__free, "queens__free"}};

struct tests__functions tests__get_queens_tests() {
    return (struct tests__functions){5, tests_list_queens};
}

void tests__queens__new() {
    struct queens_t* q = queens__new();
    queens__alloc(q, 4);
    assert(q != NULL);
    queens__free(q);
}

void tests__queens__alloc() {
    struct queens_t* q = queens__new();
    queens__alloc(q, 4);
    assert(q->array[0] != NULL);
    assert(q->array[1] != NULL);
    queens__free(q);
}

void tests__queens__init() {
    struct queens_t* q = queens__new();
    queens__alloc(q, 4);
    queens__init(q, 20);
    assert(q->array[0][0] == 20);
    assert(q->array[0][1] == 1);
    assert(q->array[0][2] == 39);
    assert(q->array[0][3] == 18);
    assert(q->array[1][0] == 379);
    assert(q->array[1][1] == 398);
    assert(q->array[1][2] == 399 - 39);
    assert(q->array[1][3] == 399 - 18);
    queens__free(q);
}

void tests__queens__copy() {
    struct queens_t* q = queens__new();
    struct queens_t* copy = queens__new();
    queens__alloc(q, 4);
    queens__init(q, 20);
    queens__copy(q, copy);
    for (int i = 0; i < 4; i++) {
        assert(q->array[0][i] == copy->array[0][i]);
        assert(q->array[1][i] == copy->array[1][i]);
    }
    queens__free(copy);
    queens__free(q);
}

void tests__queens__free() {
    struct queens_t* q = queens__new();
    queens__alloc(q, 4);
    queens__free(q);
}
