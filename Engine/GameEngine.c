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