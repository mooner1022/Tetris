//
// Created by moonm on 2022-10-06.
//

#ifndef TETRIS_GAMESCENE_H
#define TETRIS_GAMESCENE_H

#include "../Engine/GameEngine.h"

#define GAMEAREA_WIDTH  20
#define GAMEAREA_HEIGHT 20

#define ID_CURRENT_BLOCK 0
#define ID_GAMEAREA_BOX  1
#define ID_NEXTBLOCK_BOX 2
#define ID_NEXT_BLOCK    3

#define DIR_UP    0
#define DIR_RIGHT 1
#define DIR_DOWN  2
#define DIR_LEFT  3

#define GAME_PLAYING 0
#define GAME_OVER    1
#define GAME_PAUSED  2

#define STATE_DEFAULT  0
#define STATE_WALL     1
#define STATE_CONFLICT 2

typedef int Direction;

void game_onCreate(Scene *self, int prevId);
void game_onKeyInput(Scene *self, int key);
void game_onDraw(Scene *self);
void game_onDestroy(Scene *self);

Scene game_createScene(int id);

#endif
