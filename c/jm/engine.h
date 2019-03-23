#ifndef JM_ENGINE_H
#define JM_ENGINE_H

#include <stdint.h>
#include <stdbool.h>

struct EngineContext;

struct EngineContext *engine_context();
bool engine_execute(struct EngineContext *ctx, const char *text, size_t size);

#endif //JM_ENGINE_H
