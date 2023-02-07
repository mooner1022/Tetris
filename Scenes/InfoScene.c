//
// Created by moonm on 2022-11-14.
//

#include <stdio.h>
#include "InfoScene.h"
#include "../Console/Console.h"
#include "../GlobalValues.h"
#include "../KeyBinding.h"

void info_onCreate(Scene *self, int prevId) {
    setConsoleSize((Size){20, 20});
    self->obj_count = 0;
}

void info_onKeyInput(Scene *self, int key) {
    switch (key) {
        case KEY_LEFT:
            getGameInstance()->showScene(getGameInstance(), SCENE_MAIN);
            break;
    }
}

void info_onDraw(Scene *self) {
    int width = 20 * 2;
    moveto(0, 4);
    setColor(COLOR_YELLOW);
    printCenter(width, "T E T R I S\n");
    setColor(COLOR_YELLOW);
    printf("          > ");
    setColor(COLOR_YELLOW);
    printf("Team. ");
    setColor(COLOR_RED);
    printf("OverFlow");
    setColor(COLOR_YELLOW);
    printf(" <\n\n");

    setColor(COLOR_WHITE);
    printCenter(width, "Kim Haneung");
    printf("\n");
    printCenter(width, "Moon Minki *");
    printf("\n");
    printCenter(width, "Park Sungho");
    printf("\n\n");
    printCenter(width, "*: No one but me has\n");
    printCenter(width, "participated tho lol");
    printf("\n\n\n\n");
    printCenter(width, "press '<' to exit");
}

void info_onDestroy(Scene *self) {

}

Scene info_createScene(int id) {
    Scene scene = createScene(
            id,
            info_onCreate,
            info_onKeyInput,
            info_onDraw,
            info_onDestroy);
    return scene;
}