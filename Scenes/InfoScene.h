//
// Created by moonm on 2022-11-14.
//

#ifndef TETRIS_INFOSCENE_H
#define TETRIS_INFOSCENE_H

#include "../Engine/GameEngine.h"

void info_onCreate(Scene *self, int prevId);
void info_onKeyInput(Scene *self, int key);
void info_onDraw(Scene *self);
void info_onDestroy(Scene *self);

Scene info_createScene(int id);

#endif //TETRIS_INFOSCENE_H
