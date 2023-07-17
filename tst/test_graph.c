#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "graph.h"
#include "tests_functions.h"
#include "tests_utils.h"

struct func_block tests_list_graph[] = {
    {tests__graph__init, "graph__init"},
    {tests__graph__free, "graph__free"},
    {tests__graph__copy, "graph__copy"},
    {tests__graph__compress, "graph__compress"}};

struct tests__functions tests__get_graph_tests() {
    return (struct tests__functions){4, tests_list_graph};
}

void tests__graph__init() {
    struct graph_t* g = graph__new();
    graph__init(g, 4);
    assert(g->num_vertices == 4);
    gsl_spmatrix_uint* tmp = gsl_spmatrix_uint_alloc(4, 4);
    assert(gsl_spmatrix_uint_equal(tmp, g->t));
    gsl_spmatrix_uint_free(tmp);
    graph__free(g);
}

void tests__graph__free() {
    struct graph_t* g = graph__new();
    graph__init(g, 4);
    graph__free(g);
}

void tests__graph__copy() {
    struct graph_t* g = graph__new();
    graph__init(g, 20);
    graph__compress(g);
    struct graph_t* g1 = graph__copy(g);
    assert(g1->num_vertices == g->num_vertices);
    assert(gsl_spmatrix_uint_equal(g->t, g1->t));
    graph__free(g);
    graph__free(g1);
}

void tests__graph__compress() {
    struct graph_t* g = graph__new();
    graph__init(g, 4);
    struct graph_t* g1 = graph__new();
    graph__init(g1, 4);
    graph__compress(g);
    gsl_spmatrix_uint* tmp = gsl_spmatrix_uint_compress(g1->t, GSL_SPMATRIX_CSR);
    assert(gsl_spmatrix_uint_equal(g->t, tmp));
    gsl_spmatrix_uint_free(tmp);
    graph__free(g);
    graph__free(g1);
}