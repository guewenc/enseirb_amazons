#ifndef __TESTS_UTILS_H__
#define __TESTS_UTILS_H__

typedef void (*test_func)();

struct func_block {
    test_func func;
    char* func_name;
};

struct tests__functions {
    unsigned int count;
    struct func_block* func;
};

#endif // __TESTS_UTILS_H__