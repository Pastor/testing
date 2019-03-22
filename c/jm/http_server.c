#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "v7.h"
#include "mongoose.h"

static sig_atomic_t s_stop_signal = false;
static struct mg_serve_http_opts s_http_server_opts;
static struct v7 *v7;

static void handle_upload(struct mg_connection *nc, int ev, void *p) {
    switch (ev) {
        case MG_EV_HTTP_REQUEST: {
            int ret = -1;
            struct http_message *hm = (struct http_message *) p;

            nc->flags |= MG_F_SEND_AND_CLOSE;
            if (mg_vcmp(&hm->uri, "/execute") == 0) {
                //Good
            } else {
                mg_printf(nc, "HTTP/1.1 200 OK\r\n"
                              "Content-Type: application/json\r\n"
                              "Access-Control-Allow-Origin: *\r\n"
                              "Connection: close\r\n\r\n");
                break;
            }

            mg_printf(nc, "HTTP/1.1 200 OK\r\n"
                          "Content-Type: application/json\r\n"
                          "Access-Control-Allow-Origin: *\r\n"
                          "Connection: close\r\n\r\n"
                          "{\"code\": %d, \"message\": \"%s\"}", ret, "Good");
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

static double sum(double a, double b) {
    return a + b;
}

static enum v7_err js_sum(struct v7 *v7, v7_val_t *res) {
    double arg0 = v7_get_double(v7, v7_arg(v7, 0));
    double arg1 = v7_get_double(v7, v7_arg(v7, 1));
    double result = sum(arg0, arg1);

    *res = v7_mk_number(v7, result);
    return V7_OK;
}


int main(int argc, char **argv) {
    int i;
    struct mg_mgr mgr;
    struct mg_connection *c;
    char *s_http_port = "8091";
    struct mg_bind_opts bind_opts;
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
    mg_register_http_endpoint(c, "/execute", handle_upload);
    mg_set_protocol_http_websocket(c);
    s_stop_signal = false;

    {
        v7_val_t result;
        enum v7_err rcode = V7_OK;

        v7 = v7_create();
        v7_set_method(v7, v7_get_global(v7), "sum", &js_sum);
        rcode = v7_exec(v7, "print('sum = ' + sum(1.2, 3.4))", &result);
        if (rcode != V7_OK) {
            v7_print_error(stderr, v7, "Evaluation error", result);
        }
    }
    while (s_stop_signal == false) {
        mg_mgr_poll(&mgr, 100);
    }
    v7_destroy(v7);
    return EXIT_SUCCESS;
}
