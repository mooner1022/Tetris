//
// Created by moonm on 2022-10-02.
//

#include <stdlib.h>
#include <windows.h>
#include <conio.h>
#include "GameEngine.h"
#include "Object.h"
#include "../KeyBinding.h"

static Game* INSTANCE = NULL;
HANDLE renderThreadMutex = NULL;

Size sizeOf(short width, short height) {
    Size size = { width, height };
    return size;
}

Position positionOf(unsigned short x, unsigned short y) {
    Position position;
    position.x = x;
    position.y = y;
    return position;
}

Position clonePosition(Position* orgPos) {
    Position newPos;
    newPos.x = orgPos->x;
    newPos.y = orgPos->y;
    return newPos;
}

void copyPosition(Position* org, Position* target) {
    org->x = target->x;
    org->y = target->y;
}

void addPosition(Position* left, Position* right) {
    left->x += right->x;
    left->y += right->y;
}

Scene createScene(
        void (* onCreate)(scene_t *self),
        void (* onKeyInput)(scene_t *self, int key),
        void (* onDraw)(scene_t *self),
        void (* onDestroy)(scene_t *self)
) {
    Scene nScene;
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

    WaitForSingleObject(renderThreadMutex, INFINITE);

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
    ReleaseMutex(renderThreadMutex);
}

void scene_t_notifyUpdated(scene_t *self, int key) {
    if (0 <= key && key < OBJ_MAX_SIZE)
        self->obj_updated[key] = true;
}

/* Game builtin functions */

int getKeyInput(){
    if(kbhit())
        return getch();
    else
        return -1;
}

HANDLE keyInputThread = NULL;
DWORD WINAPI GameKeyInputThread(void *args) {
    int ch;

    while ((ch = _getch()) != 27 && keyInputThread != NULL)
    {
        if (!(ch == KEY_SPACE || ch == 0 || ch == 224))
            continue;
        if (ch != KEY_SPACE)
            ch = _getch();
        if (INSTANCE != NULL && INSTANCE->currentScene != NULL) {
            Scene *scene = INSTANCE->currentScene;
            scene->onKeyInput(scene, ch);
        }
    }
}

void game_t_start(game_t *self) {
    INSTANCE = self;
    keyInputThread = CreateThread(
            NULL,
            0,
            GameKeyInputThread,
            NULL,
            0,
            NULL
    );
    WaitForSingleObject(keyInputThread, INFINITE);
}

void game_t_addScene(game_t *self, short id, Scene* scene) {
    self->scenes[id] = scene;
}

void game_t_showScene(game_t *self, short id) {
    Scene* scene = self->scenes[id];

    if (self->currentScene != NULL) {
        Scene* currentScene = self->currentScene;
        currentScene->onDestroy(currentScene);
        system("cls");
    }
    self->currentScene = scene;
    scene->onCreate(scene);
    scene->_draw(scene);
}

void game_t_drawWindow(game_t *self) {
    if (self->currentScene != NULL)
        self->currentScene->_draw(self->currentScene);
}

Game createGameWindow(Size size) {
    static Game nGame;
    nGame.start = game_t_start;
    nGame.addScene = game_t_addScene;
    nGame.showScene = game_t_showScene;
    nGame.drawWindow = game_t_drawWindow;

    INSTANCE = &nGame;
    return nGame;
}

Game* getGameInstance() {
    return INSTANCE;
}