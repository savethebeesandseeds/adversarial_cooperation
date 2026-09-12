#ifndef AC_TEST_SUPPORT_H
#define AC_TEST_SUPPORT_H

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "ac/status.h"

typedef int (*ac_test_function)(void);

typedef struct {
    size_t run;
    size_t failed;
} ac_test_suite;

static inline int ac_test_fail(
    const char *file,
    int line,
    const char *expression,
    const char *detail)
{
    if (detail != NULL) {
        (void)fprintf(
            stderr,
            "%s:%d: assertion failed: %s (%s)\n",
            file,
            line,
            expression,
            detail);
    } else {
        (void)fprintf(
            stderr,
            "%s:%d: assertion failed: %s\n",
            file,
            line,
            expression);
    }
    return 1;
}

static inline void ac_test_run_one(
    ac_test_suite *suite,
    const char *name,
    ac_test_function function)
{
    int result;

    suite->run += 1U;
    result = function();
    if (result == 0) {
        (void)printf("ok %zu - %s\n", suite->run, name);
    } else {
        suite->failed += 1U;
        (void)printf("not ok %zu - %s\n", suite->run, name);
    }
}

static inline int ac_test_finish(const ac_test_suite *suite)
{
    (void)printf(
        "%zu test%s, %zu failure%s\n",
        suite->run,
        suite->run == 1U ? "" : "s",
        suite->failed,
        suite->failed == 1U ? "" : "s");
    return suite->failed == 0U ? 0 : 1;
}

#define AC_TEST_ASSERT(condition)                                             \
    do {                                                                      \
        if (!(condition)) {                                                   \
            return ac_test_fail(__FILE__, __LINE__, #condition, NULL);        \
        }                                                                     \
    } while (0)

#define AC_TEST_ASSERT_EQ_INT(actual_expression, expected_expression)         \
    do {                                                                      \
        long long ac_test_actual_value = (long long)(actual_expression);      \
        long long ac_test_expected_value = (long long)(expected_expression);  \
        if (ac_test_actual_value != ac_test_expected_value) {                 \
            char ac_test_detail[96];                                          \
            (void)snprintf(                                                   \
                ac_test_detail,                                               \
                sizeof ac_test_detail,                                        \
                "actual=%lld expected=%lld",                                 \
                ac_test_actual_value,                                         \
                ac_test_expected_value);                                      \
            return ac_test_fail(                                              \
                __FILE__,                                                     \
                __LINE__,                                                     \
                #actual_expression " == " #expected_expression,             \
                ac_test_detail);                                              \
        }                                                                     \
    } while (0)

#define AC_TEST_ASSERT_STATUS(actual_expression, expected_status)             \
    AC_TEST_ASSERT_EQ_INT((actual_expression), (expected_status))

#define AC_TEST_ASSERT_MEMORY(actual, expected, length)                       \
    do {                                                                      \
        if (memcmp((actual), (expected), (length)) != 0) {                    \
            return ac_test_fail(                                              \
                __FILE__,                                                     \
                __LINE__,                                                     \
                "memcmp(" #actual ", " #expected ", " #length ") == 0",  \
                NULL);                                                        \
        }                                                                     \
    } while (0)

#define AC_TEST_ASSERT_ZERO(buffer, length)                                   \
    do {                                                                      \
        size_t ac_test_zero_index;                                            \
        for (ac_test_zero_index = 0U;                                         \
             ac_test_zero_index < (size_t)(length);                           \
             ++ac_test_zero_index) {                                          \
            if (((const uint8_t *)(buffer))[ac_test_zero_index] != 0U) {      \
                return ac_test_fail(                                          \
                    __FILE__,                                                 \
                    __LINE__,                                                 \
                    #buffer " is all zero",                                  \
                    NULL);                                                    \
            }                                                                 \
        }                                                                     \
    } while (0)

#define AC_TEST_RUN(suite, function)                                          \
    ac_test_run_one(&(suite), #function, (function))

#endif
