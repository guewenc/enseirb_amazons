#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "colors.h"
#include "tests_functions.h"

// #define DEBUG

#ifndef DEBUG
#define TRACE(format, ...)
#else
#define TRACE(format, ...) printf(format, __VA_ARGS__)
#endif

unsigned int passed_tests = 0;
unsigned int tests_count = 0;

void running_tests() { TRACE(TST_CYAN "%s\n" TST_RESET, "Running tests..."); }

void print_success() { TRACE(TST_GREEN "%s\n" TST_RESET, "Test passed !"); }

void print_failed() { TRACE(TST_RED "%s\n" TST_RESET, "Test failed !"); }

void execute_tests(struct tests__functions t) {
    TRACE("%s", "\n");
    for (unsigned int i = 0; i < t.count; i++) {
        int pid = fork();
        if (!pid) {
            TRACE(">> Run test on [%s] ... ", t.func[i].func_name);
            fclose(stderr);
            fclose(stdout);
            alarm(5);
            t.func[i].func();
            exit(EXIT_SUCCESS);
        }

        int status;
        if (waitpid(pid, &status, 0) != -1) {
            if (WIFEXITED(status)) {
                int es = WEXITSTATUS(status);
                if (es == EXIT_SUCCESS) {
                    print_success();
                    passed_tests++;
                } else {
                    print_failed();
                }
            } else {
                print_failed();
            }
        }
        tests_count++;
    }
}

void print_summary() {
    TRACE("%s", "\n");
    if (passed_tests == tests_count)
        TRACE(TST_GREEN "100%% %s" TST_RESET, "tests passed");
    else {
        TRACE(TST_RED "%d%s test passed" TST_RESET,
              (int)(passed_tests * 100) / tests_count,
              "%");
    }
    TRACE(", %d tests failed out of %d.\n", tests_count - passed_tests, tests_count);
}

int main(void) {
    running_tests();

    execute_tests(tests__get_game_tests());
    execute_tests(tests__get_graph_tests());
    execute_tests(tests__get_queens_tests());
    execute_tests(tests__get_shape_tests());

    print_summary();

    return passed_tests == tests_count ? EXIT_SUCCESS : EXIT_FAILURE;

    return EXIT_SUCCESS;
}