//
// Created by moonm on 2022-10-03.
//

#ifndef TETRIS_MAINSCENE_H
#define TETRIS_MAINSCENE_H

#include "../Engine/GameEngine.h"

#define MENU_ITEM_SIZE 2

typedef struct menuitem_t {
    char* name;
    int sceneId;
} MenuItem;

void main_onCreate(Scene *self);
void main_onKeyInput(Scene *self, int key);
void main_onDraw(Scene *self);
void main_onDestroy(Scene *self);

Scene main_createScene();

#endif
