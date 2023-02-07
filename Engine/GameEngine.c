//
// Created by moonm on 2022-10-02.
//

#include <windows.h>
#include <conio.h>
#include "GameEngine.h"
#include "Object.h"
#include "Random.h"
#include "../Console/Console.h"

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
        // Skip first value on arrow key event
        if (ch == 0 || ch == 224)
            ch = _getch();
        if (INSTANCE != NULL && INSTANCE->currentScene != NULL) {
            Scene *scene = INSTANCE->currentScene;
            scene->onKeyInput(scene, ch);
        }
    }
    return 0;
}

void game_t_start(game_t *self) {
    rand_init();
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
    int oldId = -1;

    if (self->currentScene != NULL) {
        Scene* currentScene = self->currentScene;
        currentScene->onDestroy(currentScene);
        //purgeRenderThread();
        for (int i = 0; i < currentScene->obj_count; ++i) {
            Object* obj = currentScene->objects[i];
            obj->erase(obj);
            currentScene->obj_updated[i] = false;
        }
        oldId = currentScene->id;
        clear();
    }
    self->currentScene = scene;
    scene->onCreate(scene, oldId);
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