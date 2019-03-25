#ifndef JM_ENGINE_H
#define JM_ENGINE_H

#include <stdint.h>
#include <stdbool.h>

struct EngineContext;
struct mg_mgr;
struct mg_connection;

struct EngineContext *engine_context(struct mg_mgr *mgr);
void                  engine_add_include(struct EngineContext *ctx, const char *path);
void                  engine_add_library(struct EngineContext *ctx, const char *path);
bool                  engine_execute(struct EngineContext *ctx, struct mg_connection *c, const char *text, size_t size);

#endif //JM_ENGINE_H
