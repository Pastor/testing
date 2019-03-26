#if defined(WIN32)

#include <winsock2.h>

#else

#include <pthread.h>

#endif

#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <libtcc.h>
//#include <io.h>
#include <fcntl.h>
#include <mongoose.h>
#include "mt_common.h"
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
    volatile void *signal_stop;
};

void d_printf(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);
    fflush(stderr);
}

#if defined(WIN32)
#define pipe(fd)  _pipe(fd, 4096, _O_BINARY)
#endif

void read_pipe_and_send(int std_id, int p, const struct EngineContext *ctx);

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
    fd_set fds;
    int maxfd;
    struct timeval timeout = {0, 100};
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
    if (dup2(stdout_pipe[1], STDOUT_FILENO) == -1) {
        close(stderr_pipe[0]);
        close(stderr_pipe[1]);
        close(stdout_pipe[0]);
        close(stdout_pipe[1]);
        close(stderr_saved);
        close(stdout_saved);
        return 0;
    }
    close(stdout_pipe[1]);
    if (dup2(stderr_pipe[1], STDERR_FILENO) == -1) {
        close(stderr_pipe[0]);
        close(stderr_pipe[1]);
        close(stdout_pipe[0]);
        close(stderr_saved);
        close(stdout_saved);
        return 0;
    }
    close(stderr_pipe[1]);

    assert(fcntl(stderr_pipe[0], F_SETFL, O_NONBLOCK) != -1);
    assert(fcntl(stdout_pipe[0], F_SETFL, O_NONBLOCK) != -1);

    maxfd = stderr_pipe[0] > stdout_pipe[0] ? stderr_pipe[0] : stdout_pipe[0];

    while (ctx->console_reader_running) {
        FD_ZERO(&fds);
        FD_SET(stderr_pipe[0], &fds);
        FD_SET(stdout_pipe[0], &fds);

        if (select(maxfd + 1, &fds, NULL, NULL, &timeout) > 0) {
            if (FD_ISSET(stderr_pipe[0], &fds)) {
                read_pipe_and_send(2, stderr_pipe[0], ctx);
            }

            if (FD_ISSET(stdout_pipe[0], &fds)) {
                read_pipe_and_send(1, stdout_pipe[0], ctx);
            }
        }
    }
    dup2(stdout_saved, STDOUT_FILENO);
    dup2(stderr_saved, STDERR_FILENO);
    close(stderr_pipe[0]);
    close(stdout_pipe[0]);
    close(stderr_saved);
    close(stdout_saved);
    signal_do(ctx->signal_stop);
    printf("Read console complete\n");
    return 0;
}

void read_pipe_and_send(int std_id, int p, const struct EngineContext *ctx) {
    ssize_t buf_read;
    char buf[2048];

    while ((buf_read = read(p, buf, sizeof(buf))) > 0) {
        if (ctx->c != 0) {
            mg_printf(ctx->c, "%x\r\n%d%.*s\r\n", (int) buf_read, std_id, (int) buf_read, buf);
            mg_mgr_poll(ctx->mgr, 100);
        } else {
            break;
        }
    }
}

struct EngineContext *engine_context(struct mg_mgr *mgr) {
    struct EngineContext *ctx = calloc(1, sizeof(struct EngineContext));
    ctx->mgr = mgr;
    signal_init(&ctx->signal_stop);
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
    bool returning = false;
    char *program_text;

    state = tcc_new();
    if (state == 0 || text == 0) {
        return false;
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
        goto complete;
    }
    pthread_detach(ctx->handle);
#endif
    u_delay(100);
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
        free(program_text);
        goto complete;
    }
    free(program_text);
    tcc_add_symbol(state, "printf", d_printf);
    tcc_add_symbol(state, "usleep", u_delay);
    tcc_add_symbol(state, "hello_text", "Hello world!");
    if (tcc_relocate(state, TCC_RELOCATE_AUTO) < 0) {
        fprintf(stderr, "Error relocate");
        goto complete;
    }
    main = tcc_get_symbol(state, "main");
    if (main == 0) {
        fprintf(stderr, "Error get main function\n");
        goto complete;
    }
    i = (*main)(0, 0);
    returning = true;
    complete:
    ctx->console_reader_running = false;
    signal_wait(ctx->signal_stop, 5000);
    ctx->c = 0;
    tcc_delete(state);
    return returning;
}

