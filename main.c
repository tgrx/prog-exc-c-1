#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>


size_t search_backward(
        char const * const text,
        char const symbol,
        size_t const start_from
) {
    assert(start_from <= strlen(text));

    size_t start_position = start_from ? (start_from - 1) : start_from;

    for (size_t pos = start_position; pos; --pos) {
        if (symbol == text[pos]) {
            return pos;
        }
    }

    return 0;
}


char * normalize(char const * const path) {
    char * const path_normalized =
            (char *) calloc(strlen(path) + 1, sizeof(char));

    char symbol_current = 0;
    char symbol_previous = 0;
    char cd_parent = 0;

    size_t idx_normalized = 0;

    for (size_t idx_origin = 0; idx_origin <= strlen(path); ++idx_origin) {
        symbol_previous = symbol_current;
        symbol_current = path[idx_origin];

        if ('.' == symbol_current) {
            if (!symbol_previous || ('/' == symbol_previous)) {
                continue;
            }

            if ('.' == symbol_previous) {
                cd_parent = 1;
                idx_normalized = search_backward(
                        path, '/',
                        search_backward(
                                path, '/', idx_normalized));
                continue;
            }
        } else if (('/' == symbol_current) && ('.' == symbol_previous)) {
            if (!cd_parent) {
                continue;
            }
        }

        path_normalized[idx_normalized] = symbol_current;
        idx_normalized += 1;
        cd_parent = 0;
    }

    return path_normalized;
}


int test_normalize(char const * const path, char const * const norm_expected) {
    char * const norm_actual = normalize(path);
    int fault = 0;

    if (0 == strcmp(norm_actual, norm_expected)) {
        printf("OK: \"%s\" == \"%s\"\n", path, norm_actual);
    } else {
        fault = 1;

        fprintf(
                stderr,
                "FAILURE: \"%s\" != \"%s\", got \"%s\" instead\n",
                path,
                norm_expected,
                norm_actual
        );
    }

    free(norm_actual);

    return fault;
}


int run_tests() {
    typedef char const * str_t;

    str_t const paths[] = {
            "",
            "../bar",
            "/foo/bar",
            "/foo/bar/../baz",
            "/foo/bar/./baz/",
            "/foo/../../baz",
            "/foo/../../../../baz",
            "1/2/3/4/5/../../../../baz",
            "1/2/3/4/5/./.././.././.././../baz",
    };

    str_t const norms[] = {
            "",
            "/bar",
            "/foo/bar",
            "/foo/baz",
            "/foo/bar/baz/",
            "/baz",
            "/baz",
            "1/baz",
            "1/baz",
    };

    int fault = 0;
    size_t n_tests = (sizeof(paths) / sizeof(str_t));

    for (size_t idx_test = 0; idx_test < n_tests; ++idx_test) {
        str_t path = paths[idx_test];
        str_t norm = norms[idx_test];

        fault |= test_normalize(path, norm);
    }

    return fault;
}


int main(int argc, char ** argv) {
    int fault = run_tests();
    if (!fault) return EXIT_SUCCESS;
    return EXIT_FAILURE;
}
