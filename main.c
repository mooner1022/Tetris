#include "GlobalValues.h"
#include "Engine/GameEngine.h"
#include "Engine/Object.h"
#include "Console/Console.h"

// Scenes
#include "Scenes/MainScene.h"
#include "Scenes/GameScene.h"
#include "Scenes/InfoScene.h"

int main() {
    initConsole();

    Size windowSize = getConsoleSize();
    Game game = createGameWindow(windowSize);

    Scene mainScene = main_createScene(SCENE_MAIN);
    game.addScene(&game, SCENE_MAIN, &mainScene);
    //game.showScene(&game, 0);

    Scene gameScene = game_createScene(SCENE_GAME);
    game.addScene(&game, SCENE_GAME, &gameScene);

    Scene infoScene = info_createScene(SCENE_INFO);
    game.addScene(&game, SCENE_INFO, &infoScene);

    game.showScene(&game, SCENE_MAIN);

    game.start(&game);

    /*
    Object obj = getRandomShape(COLOR_GREEN);
    obj.position.x = 2;
    obj.position.y = 0;

    Object line = newLine(sizeOf(1, 15), COLOR_BLUE);
    line.position.x = 8;
    line.position.y = 1;

    while (1) {
        clear();
        if (obj.position.y > 15)
            obj.position.y = 0;
        moveObj(&obj, DIR_DOWN);
        obj.draw(&obj);
        line.draw(&line);
        if (DEBUG) {
            moveto_origin(12, 1, ORIGIN_END);
            setColor(COLOR_YELLOW);
            printf("x= %02d  y= %02d", obj.position.x, obj.position.y);
        }

        Sleep(300);
    }
     */
    return 0;
}
