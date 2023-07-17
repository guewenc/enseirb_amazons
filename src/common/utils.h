/**
 * @file utils.h
 * @brief This file contains utility functions used throughout the codebase.
 */

#ifndef __UTILS_H__
#define __UTILS_H__

#include <assert.h>

#define PROGRAM_CONTINUE 0
#define PROGRAM_EXIT 1

/**
 * @brief Defines a typedef for an unsigned integer.
 */
typedef unsigned int uint;

/**
 * @brief Print an error message and optionally end the program.
 *
 * @param func The name of the function that raised the error.
 * @param error_msg The error message to print.
 * @param quit If set to PROGRAM_EXIT, the program will terminate after printing the error message. If set to PROGRAM_CONTINUE, the program will continue to run.
 */
void handle_error(const char* func, char* error_msg, int quit);

#endif // __UTILS_H__