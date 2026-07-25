#ifndef ROMMPL_TEST_H
#define ROMMPL_TEST_H
#include <stdio.h>
#include <string.h>

static int tests_run = 0;
static int tests_failed = 0;

#define ASSERT(cond) do {                                            \
    tests_run++;                                                     \
    if (!(cond)) { tests_failed++;                                   \
        printf("  FAIL %s:%d: %s\n", __FILE__, __LINE__, #cond); }   \
} while (0)

#define ASSERT_STR_EQ(a, b) do {                                     \
    tests_run++;                                                     \
    if (strcmp((a), (b)) != 0) { tests_failed++;                     \
        printf("  FAIL %s:%d: \"%s\" != \"%s\"\n",                   \
               __FILE__, __LINE__, (a), (b)); }                      \
} while (0)

#define ASSERT_INT_EQ(a, b) do {                                     \
    long _va = (long)(a), _vb = (long)(b);                           \
    tests_run++;                                                     \
    if (_va != _vb) { tests_failed++;                                \
        printf("  FAIL %s:%d: %ld != %ld\n",                         \
               __FILE__, __LINE__, _va, _vb); }                      \
} while (0)

#define TEST_SUMMARY() \
    (printf("%d run, %d failed\n", tests_run, tests_failed), tests_failed ? 1 : 0)

#endif
