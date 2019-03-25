#if defined(WIN32)

#include <winsock2.h>

#else
#include <pthread.h>
#endif

#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <memory.h>
#include <libtcc.h>
#include <io.h>
#include <mongoose.h>
#include "engine.h"

struct EngineContext {
    char **include;
    size_t include_cnt;
    char **library;
    size_t library_cnt;
    struct mg_mgr *mgr;
    struct mg_connection *c;
    bool console_reader_running;
#if defined(WIN32)
    DWORD thread_id;
    HANDLE handle;
#else
    pthread_t handle;
#endif
};

void d_printf(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);
    fflush(stderr);
}

void usleep(long ms) {
    Sleep(ms);
}

#if defined(WIN32)
#define pipe(fd)  _pipe(fd, 4096, 0)
#endif

void read_pipe_and_send(int p, const struct EngineContext *ctx);

#if defined(WIN32)

DWORD WINAPI
#else
void *
#endif
console_reader(void *param) {
    int stderr_pipe[2];
    int stdout_pipe[2];
    int stderr_saved;
    int stdout_saved;
    int ret;
    FD_SET read_set;


//    struct timeval time = {1, 0};
    struct EngineContext *ctx = (struct EngineContext *) param;

    if (pipe(stderr_pipe) != 0)
        return 0;
    if (pipe(stdout_pipe) != 0) {
        close(stderr_pipe[0]);
        close(stderr_pipe[1]);
        return 0;
    }
    stdout_saved = dup(STDOUT_FILENO);
    stderr_saved = dup(STDERR_FILENO);
    if ( dup2(stdout_pipe[1], STDOUT_FILENO) == -1 ) {
        close(stderr_pipe[0]);
        close(stderr_pipe[1]);
        close(stdout_pipe[0]);
        close(stdout_pipe[1]);
        close(stderr_saved);
        close(stdout_saved);
        return 0;
    }
    close(stdout_pipe[1]);
    if ( dup2(stderr_pipe[1], STDERR_FILENO) == -1 ) {
        close(stderr_pipe[0]);
        close(stderr_pipe[1]);
        close(stdout_pipe[0]);
        close(stderr_saved);
        close(stdout_saved);
        return 0;
    }
    close(stderr_pipe[1]);

//    int max_fd = max(stderr_pipe[0], stdout_pipe[0]);
    while (ctx->console_reader_running) {
        FD_ZERO(&read_set);
//        FD_SET(stdout_pipe[0], &read_set);
//        FD_SET(stderr_pipe[0], &read_set);

//        if ( (ret = select(max_fd + 1, NULL, NULL, NULL, &time)) != SOCKET_ERROR) {
//            if (FD_ISSET(stderr_pipe[0], &read_set)) {
                read_pipe_and_send(stderr_pipe[0], ctx);
//            }
//            if (FD_ISSET(stdout_pipe[0], &read_set)) {
                read_pipe_and_send(stdout_pipe[0], ctx);
//            }
//        }
    }
    dup2(stdout_saved, STDOUT_FILENO);
    dup2(stderr_saved, STDERR_FILENO);
    close(stderr_pipe[0]);
    close(stdout_pipe[0]);
    close(stderr_saved);
    close(stdout_saved);
    printf("Read console complete\n");
    return 0;
}

void read_pipe_and_send(int p, const struct EngineContext *ctx) {
    int buf_read;
    char buf[2048];

    while ((buf_read = read(p, buf, sizeof(buf))) > 0) {
        if (ctx->c != 0) {
            mg_printf(ctx->c, "%x\r\n%.*s\r\n", buf_read, buf_read, buf);
            mg_mgr_poll(ctx->mgr, 100);
        } else {
            //
        }
    }
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

struct EngineContext *engine_context(struct mg_mgr *mgr) {
    struct EngineContext *ctx = calloc(1, sizeof(struct EngineContext));
    ctx->mgr = mgr;
    return ctx;
}

void engine_add_include(struct EngineContext *ctx, const char *path) {
    if (ctx != 0) {
        if (ctx->include == 0) {
            ctx->include = malloc(sizeof(char *));
        } else {
            ctx->include = realloc(ctx->include, sizeof(char *) * (ctx->include_cnt + 1));
        }
        ctx->include[ctx->include_cnt] = strdup(path);
        ctx->include_cnt++;
    }
}

void engine_add_library(struct EngineContext *ctx, const char *path) {
    if (ctx != 0) {
        if (ctx->library == 0) {
            ctx->library = malloc(sizeof(char *));
        } else {
            ctx->library = realloc(ctx->library, sizeof(char *) * (ctx->library_cnt + 1));
        }
        ctx->library[ctx->library_cnt] = strdup(path);
        ctx->library_cnt++;
    }
}

bool engine_execute(struct EngineContext *ctx, struct mg_connection *c, const char *text, size_t size) {
    int (*main)(int, char **);
    TCCState *state;
    int i;
    char *program_text;

    state = tcc_new();
    if (state == 0 || text == 0) {
        return 0;
    }

    ctx->c = c;
    ctx->console_reader_running = true;
#if defined(WIN32)
    ctx->handle = CreateThread(NULL, 0, console_reader, ctx, 0, &ctx->thread_id);
    if (ctx->handle == 0)
        return 0;
    CloseHandle(ctx->handle);
#else
    if (pthread_create(&ctx->handle, NULL, console_reader, ctx) != 0) {
        return 0;
    }
    pthread_detach(ctx->handle);
#endif

    program_text = calloc(1, size + 1);
    memcpy(program_text, text, size);
    for (i = 0; i < ctx->include_cnt; ++i) {
        tcc_add_include_path(state, ctx->include[i]);
    }
    for (i = 0; i < ctx->library_cnt; ++i) {
        tcc_add_library_path(state, ctx->library[i]);
    }

    tcc_set_output_type(state, TCC_OUTPUT_MEMORY);
    if (tcc_compile_string(state, program_text) == -1) {
        fprintf(stderr, "Error compile\n");
        tcc_delete(state);
        free(program_text);
        return false;
    }
    free(program_text);
    tcc_add_symbol(state, "printf", d_printf);
    tcc_add_symbol(state, "usleep", usleep);
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
    i = (*main)(0, 0);
    ctx->console_reader_running = false;
    usleep(1000);
    ctx->c = 0;
    tcc_delete(state);
    return true;
}

