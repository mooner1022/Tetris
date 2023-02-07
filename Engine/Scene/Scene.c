//
// Created by moonm on 2022-11-10.
//

#include "Scene.h"
#include "windows.h"

HANDLE renderThreadMutex = NULL;

Scene createScene(
        int id,
        void (* onCreate)(scene_t *self, int prevId),
        void (* onKeyInput)(scene_t *self, int key),
        void (* onDraw)(scene_t *self),
        void (* onDestroy)(scene_t *self)
) {
    Scene nScene;
    nScene.id         = id;
    nScene.onCreate   = onCreate;
    nScene.onKeyInput = onKeyInput;
    nScene.onDraw     = onDraw;
    nScene.onDestroy  = onDestroy;
    nScene.obj_count  = 1;

    for (int i = 0; i < OBJ_MAX_SIZE; ++i) {
        nScene.objects[i] = NULL;
        nScene.obj_cache[i] = NULL;
        nScene.obj_updated[i] = false;
    }

    nScene._draw = scene_t_draw;
    nScene.notifyUpdated = scene_t_notifyUpdated;

    return nScene;
}

void queueRenderThread() {
    if (renderThreadMutex != NULL)
        WaitForSingleObject(renderThreadMutex, INFINITE);
}

void releaseRenderThread() {
    if (renderThreadMutex != NULL)
        ReleaseMutex(renderThreadMutex);
}

void purgeRenderThread() {
    if (renderThreadMutex != NULL)
        CloseHandle(renderThreadMutex);
}

void scene_t_draw(scene_t *self) {
    /*
    system("cls");
    for (int i = 0; i < self->obj_count; ++i) {
        object_t_draw(self->objects[i]);
    }
    self->onDraw(self);
     */

    // Prevent multiple render running
    if (renderThreadMutex == NULL)
        renderThreadMutex = CreateMutex(NULL, FALSE, NULL);

    queueRenderThread();

    for (int i = 0; i < OBJ_MAX_SIZE; ++i) {
        if (self->obj_updated[i]) {
            Object* obj = self->objects[i];
            Object* cache = self->obj_cache[i];

            if (cache != NULL) {
                cache->erase(cache);

                memset(cache, 0, sizeof(Object));
                free(cache);
            }
            Object* newCache = (Object*)malloc(sizeof(Object));
            cloneObject(obj, newCache);
            self->obj_cache[i] = newCache;
            self->obj_updated[i] = false;

            obj->draw(obj);
        }
    }
    self->onDraw(self);
    releaseRenderThread();
}

void scene_t_notifyUpdated(scene_t *self, int key) {
    if (0 <= key && key < OBJ_MAX_SIZE)
        self->obj_updated[key] = true;
}