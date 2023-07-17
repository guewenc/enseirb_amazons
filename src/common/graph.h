/**
 * @file graph.h
 * @brief This header file defines functions to manipulate graphs.
 */

#ifndef _AMAZON_GRAPH_H_
#define _AMAZON_GRAPH_H_

#include <gsl/gsl_spblas.h>
#include <gsl/gsl_spmatrix.h>
#include <gsl/gsl_spmatrix_uint.h>
#include <stddef.h>

#include "dir.h"
#include "utils.h"

struct graph_t {
    unsigned int num_vertices; // Number of vertices in the graph
    gsl_spmatrix_uint* t; // Sparse matrix of size n*n,
                          // t[i][j] > 0 means there is an edge from i to j
                          // t[i][j] == DIR_NORTH means that j is NORTH of i
                          // t[i][j] == DIR_SOUTH means that j is SOUTH of i
                          // and so on
};

/**
 * @brief Creates a new graph.
 *
 * @return A pointer to the newly created graph.
 */
struct graph_t* graph__new();

/**
 * @brief Initializes a graph.
 *
 * @param board The graph to initialize.
 * @param num_vertices The number of vertices of the graph.
 */
void graph__init(struct graph_t* board, uint num_vertices);

/**
 * @brief Compresses a graph using the compressed sparse row (CSR) format.
 *
 * @param board The graph to compress.
 */
void graph__compress(struct graph_t* board);

/**
 * @brief Creates a copy of a graph.
 *
 * @param board The graph to copy.
 * @return A pointer to the newly created copy of the graph.
 */
struct graph_t* graph__copy(struct graph_t* board);

/**
 * @brief Compress the matrix of the graph and replace it by the compressed matrix. Free the old matrix.
 *
 * @param b - The address of the graph.
 */
void graph__compress(struct graph_t* b);

/**
 * @brief Disconnects a vertex from its neighbors in a graph.
 *
 * @param board The graph.
 * @param pos The position of the vertex to disconnect.
 */
void graph__disconnect(struct graph_t* board, uint pos);

/**
 * @brief Returns the neighbor of a vertex in a graph in a given direction.
 *
 * @param board The graph.
 * @param pos The position of the vertex.
 * @param dir The direction in which to search for the neighbor.
 * @return The position of the neighbor if found, UINT_MAX otherwise.
 */
uint graph__get_neighbor(struct graph_t* board, uint pos, enum dir_t dir);

/**
 * @brief Checks if a vertex in a graph is isolated.
 *
 * @param g The graph.
 * @param pos The position of the vertex.
 * @return 1 if the vertex is isolated, 0 otherwise.
 */
int is_isolated(struct graph_t* g, uint pos);

/**
 * @brief Frees the memory allocated for a graph.
 *
 * @param board The graph to free.
 */
void graph__free(struct graph_t* board);

#endif // _AMAZON_GRAPH_H_
