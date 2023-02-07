//
// Created by moonm on 2022-11-10.
//

#ifndef TETRIS_SCENE_H
#define TETRIS_SCENE_H

#include "../Object.h"

#define OBJ_MAX_SIZE 10

typedef struct scene_t scene_t;
typedef struct scene_t {
    int id;
    Object* objects[OBJ_MAX_SIZE];
    Object* obj_cache[OBJ_MAX_SIZE];
    bool obj_updated[OBJ_MAX_SIZE];
    int obj_count;

    void (* onCreate)(scene_t *self, int prevScene);
    void (* onKeyInput)(scene_t *self, int key);
    void (* notifyUpdated)(scene_t *self, int id);
    void (* _draw)(scene_t *self);
    void (* onDraw)(scene_t *self);
    void (* onDestroy)(scene_t *self);
} Scene;

Scene createScene(
        int id,
        void (* onCreate)(scene_t *self, int prevId),
        void (* onKeyInput)(scene_t *self, int key),
        void (* onDraw)(scene_t *self),
        void (* onDestroy)(scene_t *self)
);
void queueRenderThread();
void releaseRenderThread();
void purgeRenderThread();
void scene_t_draw(scene_t *self);
void scene_t_notifyUpdated(scene_t *self, int key);

#endif
