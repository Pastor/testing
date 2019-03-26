#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "v7.h"
#include "mongoose.h"
#include "engine.h"

static sig_atomic_t s_stop_signal = false;
static struct mg_serve_http_opts s_http_server_opts;
static  struct EngineContext *ctx;

static char *cwd() {
    static char current_dir[MAX_PATH * 2];
    unsigned long current_dir_size = 0;

    if (current_dir[0] != 0)
        return current_dir;

#if defined(WIN32)
    current_dir_size = GetCurrentDirectoryA(sizeof(current_dir), current_dir);
    current_dir[current_dir_size] = 0;
#else
    getcwd(current_dir, MAX_PATH);
#endif
    return current_dir;
}

static char *cwd_append(const char *path) {
    char *p = cwd();
    char *n = calloc(1, strlen(p) + strlen(p) + 2);
    strcpy(n, p);
    strcat(n, "/");
    strcat(n, path);
    return n;
}

static void handle_execute(struct mg_connection *nc, int ev, void *p) {
    switch (ev) {
        case MG_EV_HTTP_REQUEST: {
            int ret = -1;
            struct http_message *hm = (struct http_message *) p;


            if (mg_vcmp(&hm->uri, "/execute") == 0 && mg_vcmp(&hm->method, "POST") == 0) {
                mg_printf(nc, "HTTP/1.1 200 OK\r\n"
                              "Transfer-Encoding: chunked\r\n"
                              "Content-Type: text/plain\r\n"
                              "Access-Control-Allow-Origin:  *\r\n"
                              "Connection: keep-alive\r\n\r\n");
                engine_execute(ctx, nc, hm->body.p, hm->body.len);
                mg_printf(nc, "0\r\n\r\n");
            } else {
                mg_printf(nc, "HTTP/1.1 200 OK\r\n"
                              "Content-Type: application/json\r\n"
                              "Connection: close\r\n\r\n"
                              "{\"code\": 1, \"message\": \"Bad request\"}");
            }
            nc->flags |= MG_F_SEND_AND_CLOSE;
            nc->user_data = 0;
            break;
        }
        default:
            break;
    }
}

static void ev_handler(struct mg_connection *nc, int ev, void *ev_data) {
    if (ev == MG_EV_HTTP_REQUEST) {
        mg_serve_http(nc, ev_data, s_http_server_opts);
    }
}

int main(int argc, char **argv) {
    int i;
    struct mg_mgr mgr;
    struct mg_connection *c;
    char *s_http_port = "8091";
    struct mg_bind_opts bind_opts;
    char *push;
#if MG_ENABLE_SSL
    char *ssl_cert = 0;
    char *ssl_key = 0;
#endif

    for (i = 1; i < argc; ++i) {
        if (strcmp("-port", argv[i]) == 0) {
            s_http_port = argv[++i];
        }
#if MG_ENABLE_SSL
        else if (strcmp("-ssl_cert", argv[i]) == 0) {
            ssl_cert = argv[++i];
        } else if (strcmp("-ssl_key", argv[i]) == 0) {
            ssl_key = argv[++i];
        }
#endif
    }

    memset(&bind_opts, 0, sizeof(bind_opts));
    mg_mgr_init(&mgr, 0);
#if MG_ENABLE_SSL
    if (ssl_key && ssl_cert) {
        bind_opts.ssl_cert = ssl_cert;
        bind_opts.ssl_key = ssl_key;
        c = mg_bind_opt(&mgr, s_http_port, ev_handler, bind_opts);
    } else
#endif
    {
        c = mg_bind(&mgr, s_http_port, ev_handler);
    }
    if (c == 0) {
        fprintf(stderr, "[http] Cannot start server on port %s\n", s_http_port);
        exit(EXIT_FAILURE);
    }
    s_http_server_opts.document_root = ".";
    s_http_server_opts.document_root = "web_root";
    mg_register_http_endpoint(c, "/execute", handle_execute);
    mg_set_protocol_http_websocket(c);
    s_stop_signal = false;

    ctx = engine_context(&mgr);
#if defined(WIN32)
    push = cwd_append("win");
#else
    push = cwd_append("unix");
#endif
    engine_add_library(ctx, push);
    free(push);
    push = cwd_append("sysroot");
    engine_add_include(ctx, push);
    free(push);
    while (s_stop_signal == false) {
        mg_mgr_poll(&mgr, 100);
    }

    return EXIT_SUCCESS;
}
