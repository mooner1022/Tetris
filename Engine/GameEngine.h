//
// Created by moonm on 2022-10-02.
//

#ifndef TETRIS_GAMEENGINE_H
#define TETRIS_GAMEENGINE_H

#include <stdbool.h>

#define DEBUG true

#define SCENE_COUNT  3
#define OBJ_MAX_SIZE 10

typedef struct {
    unsigned short width;
    unsigned short height;
} Size;

Size sizeOf(short width, short height);

typedef struct {
    int x;
    int y;
} Position;

Position positionOf(unsigned short x, unsigned short y);
Position clonePosition(Position* orgPos);
void copyPosition(Position* org, Position* target);
void addPosition(Position* left, Position* right);

typedef struct object_t object_t;
typedef struct scene_t scene_t;
typedef struct scene_t {
    object_t* objects[OBJ_MAX_SIZE];
    object_t* obj_cache[OBJ_MAX_SIZE];
    bool obj_updated[OBJ_MAX_SIZE];
    int obj_count;

    void (* onCreate)(scene_t *self);
    void (* onKeyInput)(scene_t *self, int key);
    void (* notifyUpdated)(scene_t *self, int id);
    void (* _draw)(scene_t *self);
    void (* onDraw)(scene_t *self);
    void (* onDestroy)(scene_t *self);
} Scene;

Scene createScene(
        void (* onCreate)(scene_t *self),
        void (* onKeyInput)(scene_t *self, int key),
        void (* onDraw)(scene_t *self),
        void (* onDestroy)(scene_t *self)
);
void scene_t_draw(scene_t *self);
void scene_t_notifyUpdated(scene_t *self, int key);

typedef struct game_t game_t;
typedef struct game_t {
    Scene* scenes[SCENE_COUNT];
    Scene* currentScene;

    void (* start)(game_t *self);
    void (* addScene)(game_t *self, short id, Scene* scene);
    void (* showScene)(game_t *self, short id);
    void (* drawWindow)(game_t *self);
} Game;

void game_t_start(game_t *self);
void game_t_addScene(game_t *self, short id, Scene* scene);
void game_t_showScene(game_t *self, short id);
void game_t_drawWindow(game_t *self);

Game createGameWindow(Size size);
Game* getGameInstance();

#endif
