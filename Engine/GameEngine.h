//
// Created by moonm on 2022-10-02.
//

#ifndef TETRIS_GAMEENGINE_H
#define TETRIS_GAMEENGINE_H

#include <stdbool.h>
#include "Object.h"
#include "Scene/Scene.h"

#define DEBUG true

#define SCENE_COUNT  3

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
