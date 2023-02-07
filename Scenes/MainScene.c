//
// Created by moonm on 2022-10-03.
//

#include <stdio.h>
#include <windows.h>
#include "../Console/Console.h"
#include "../Engine/GameEngine.h"
#include "../KeyBinding.h"
#include "../GlobalValues.h"
#include "MainScene.h"

Object titleBlock;
Object box;

int cursorPosition = 0;

HANDLE UIUpdateThread = NULL;

MenuItem menuitem[MENU_ITEM_SIZE] = {
        {"START GAME", SCENE_GAME},
        { "INFO", SCENE_INFO}
};

DWORD WINAPI UIThreadFunc(void *args) {
    bool flag = false;
    while (UIUpdateThread != NULL) {
        queueRenderThread();
        moveto(14, 8);
        if (flag == true) {
            setColor(COLOR_BLACK);
            printf("T E T R I S");
        } else {
            setColor(COLOR_YELLOW);
            printf("T E T R I S");
        }
        releaseRenderThread();

        Sleep(500);
        flag = !flag;
    }
}

void main_onCreate(Scene *self, int prevId) {
    setConsoleSize((Size){20, 20});

    titleBlock = getShapeByIndex(3, true);
    titleBlock.position.x = 26;
    titleBlock.position.y = 8;
    self->objects[1] = &titleBlock;
    self->notifyUpdated(self, 1);

    box = newBox(sizeOf(30, 5), COLOR_GREEN);
    box.position.x = 4;
    box.position.y = 6;

    //self->objects[0] = &object;
    self->objects[0] = &box;
    self->notifyUpdated(self, 0);

    self->obj_count = 2;

    UIUpdateThread = CreateThread(
            NULL,
            0,
            UIThreadFunc,
            NULL,
            0,
            NULL
            );
    //WaitForSingleObject(UIUpdateThread, 30000);
    if (prevId != -1)
        clear();
}

void main_onKeyInput(Scene *self, int key) {
    switch (key) {
        case KEY_UP:
            if (cursorPosition > 0)
                cursorPosition--;
            self->_draw(self);
            break;
        case KEY_DOWN:
            if (cursorPosition < MENU_ITEM_SIZE - 1)
                cursorPosition++;
            self->_draw(self);
            break;
        case KEY_SPACE:
            getGameInstance()->showScene(getGameInstance(), (short) menuitem[cursorPosition].sceneId);
            break;
        default:
            break;
    }
}

void main_onDraw(Scene *self) {
    moveto(18, 10);
    setColor(COLOR_YELLOW);
    printf("> ");
    setColor(COLOR_YELLOW);
    printf("Team. ");
    setColor(COLOR_RED);
    printf("OverFlow");
    setColor(COLOR_YELLOW);
    printf(" <");

    for (int i = 0; i < MENU_ITEM_SIZE; ++i) {
        moveto(12, 15 + i);
        if (cursorPosition == i) {
            setColor(COLOR_GREEN);
            printf("> ");
        } else {
            setColor(COLOR_WHITE);
            printf("  ");
        }
        printf("%s", menuitem[i].name);
    }
}

void main_onDestroy(Scene *self) {
    if (UIUpdateThread != NULL) {
        CloseHandle(UIUpdateThread);
        UIUpdateThread = NULL;
    }
}

Scene main_createScene(int id) {
    Scene nScene = createScene(
            id,
            main_onCreate,
            main_onKeyInput,
            main_onDraw,
            main_onDestroy
    );
    return nScene;
}