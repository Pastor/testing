#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <malloc.h>
#include <memory.h>
#include <libtcc.h>
#include "engine.h"

struct EngineContext {

};

void d_printf(const char *fmt, ...) {
    va_list args;

    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);
    fflush(stderr);
}

static double sum(double a, double b) {
    return a + b;
}

static void print(const char *text) {
    fprintf(stdout, "%s", text);
    fflush(stdout);
}

//{
//
//static double sum(double a, double b) {
//    return a + b;
//}
//
//static enum v7_err js_sum(struct v7 *v7, v7_val_t *res) {
//    double arg0 = v7_get_double(v7, v7_arg(v7, 0));
//    double arg1 = v7_get_double(v7, v7_arg(v7, 1));
//    double result = sum(arg0, arg1);
//
//    *res = v7_mk_number(v7, result);
//    return V7_OK;
//}
//
//static struct v7 *v7;
//v7_val_t result;
//enum v7_err rcode = V7_OK;
//
//v7 = v7_create();
//v7_set_method(v7, v7_get_global(v7), "sum", &js_sum);
//rcode = v7_exec(v7, "print('sum = ' + sum(1.2, 3.4))", &result);
//if (rcode != V7_OK) {
//v7_print_error(stderr, v7, "Evaluation error", result);
//}
//v7_destroy(v7);
//}

struct EngineContext *engine_context() {
    struct EngineContext *ctx = malloc(sizeof(struct EngineContext));
    return ctx;
}

bool engine_execute(struct EngineContext *ctx, const char *text, size_t size) {
    int (*main)(int argc, char **argv);
    TCCState *state;
    int retcode;

    state = tcc_new();
    if (state == 0 || text == 0) {
        return 0;
    }
    char *program_text = calloc(1, size + 1);
    memcpy(program_text, text, size);

    tcc_add_include_path(state, "F:/GitHub/testing/c/jm/3rdparty/tinycc/sysroot");
    tcc_add_library_path(state, "F:/GitHub/testing/c/jm/3rdparty/tinycc/win");
    tcc_set_output_type(state, TCC_OUTPUT_MEMORY);
    if (tcc_compile_string(state, program_text) == -1) {
        fprintf(stderr, "Error compile\n");
        tcc_delete(state);
        free(program_text);
        return false;
    }
    free(program_text);
    tcc_add_symbol(state, "sum", sum);
    tcc_add_symbol(state, "print", print);
    tcc_add_symbol(state, "printf", d_printf);
    tcc_add_symbol(state, "hello_text", "Hello world!");
    if (tcc_relocate(state, TCC_RELOCATE_AUTO) < 0) {
        fprintf(stderr, "Error relocate");
        tcc_delete(state);
        return false;
    }
    main = tcc_get_symbol(state, "main");
    if (main == 0) {
        fprintf(stderr, "Error get main function\n");
        tcc_delete(state);
        return false;
    }
    retcode = (*main)(0, 0);
    tcc_delete(state);
    return true;
}

