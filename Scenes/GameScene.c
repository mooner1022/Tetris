//
// Created by moonm on 2022-10-06.
//

#include <stdio.h>
#include <windows.h>
#include "../Console/Console.h"
#include "../Engine/GameEngine.h"
#include "../KeyBinding.h"
#include "GameScene.h"

static Scene* nScene;
static Object currentBlock;
static Object gameAreaWall;

Position limitStartPos = { 2, 0 };
Position limitEndPos   = { GAMEAREA_WIDTH, 1 };
//static Object* currentBlock = NULL;

int maxShapeHeight = GAMEAREA_HEIGHT;
static int stackedBlocks[GAMEAREA_HEIGHT][GAMEAREA_WIDTH / 2] = { 0 };

int gameState = GAME_PLAYING;
int totalBlockCount = 0;
int stage = 1;
int score = 0;
int lastKey = -1;

HANDLE GameUIThread = NULL;

Position coordOfShape(int num);
void rotateObject(Object *object);
int canMove(Object *object, Direction dir, int distance);
int moveObject(Object *object, Direction direction, int distance);

void assignObjToMap(Object *object) {
    for (int i = 0; i < 4; ++i) {
        Position pos = object->shape[i];
        pos.x *= 2;
        pos.x += (object->position.x - 1);
        pos.y += (object->position.y - 1);
        stackedBlocks[pos.y][pos.x / 2] = object->color;

        if (pos.y < maxShapeHeight)
            maxShapeHeight = pos.y;
    }
}

int checkLineFilled() {
    int filled = 0;
    for (int i = maxShapeHeight; i < GAMEAREA_HEIGHT; ++i) {
        bool isLineFilled = true;
        for (int j = 0; j < GAMEAREA_WIDTH / 2; ++j) {
            if (stackedBlocks[i][j] == 0) {
                isLineFilled = false;
                break;
            }
        }
        if (isLineFilled) {
            size_t size = sizeof(int) * GAMEAREA_WIDTH;
            for (int j = i; j > maxShapeHeight; --j) {
                memcpy_s(stackedBlocks[j], size, stackedBlocks[j - 1], size);
            }
            memset(stackedBlocks[maxShapeHeight], 0, size);
            filled++;
        }
    }
    return filled;
}

void updateScore(int rows) {
    int added;
    switch (rows) {
        case 1:
            added = 40;
            break;
        case 2:
            added = 100;
            break;
        case 3:
            added = 300;
            break;
        case 4:
            added = 1200;
            break;
        default:
            added = 0;
            break;
    }
    score += added;
}

DWORD WINAPI GameUIThreadFunc(void *args) {

    int moveState = STATE_DEFAULT;

    while (GameUIThread != NULL) {
        moveState = moveObject(&currentBlock, DIR_DOWN, 1);

        if (moveState == STATE_CONFLICT) {
            assignObjToMap(&currentBlock);
            int filled = checkLineFilled();
            updateScore(filled);

            if (maxShapeHeight <= 1) {
                gameState = GAME_OVER;
                nScene->_draw(nScene);
                return 0;
            }
            currentBlock = getRandomShape();
            //currentBlock = getShapeByIndex(1, false);
            currentBlock.position.x = (GAMEAREA_WIDTH / 4) * 2;
            currentBlock.position.y = 0;

            nScene->objects[ID_CURRENT_BLOCK] = &currentBlock;

            if (++totalBlockCount % 6 == 0)
                stage++;

            // Redraw UI when block position fixed
            system("cls");
            nScene->notifyUpdated(nScene, ID_GAMEAREA_BOX);
        }
        nScene->notifyUpdated(nScene, ID_CURRENT_BLOCK);
        nScene->_draw(nScene);

        Sleep(200 + ((1 / stage) * 800));
    }
}

void game_onCreate(Scene *self) {
    nScene = self;

    limitStartPos = positionOf(2, 0);
    limitEndPos   = positionOf(GAMEAREA_WIDTH + 1, 1);

    // Initialize all elements to 0
    for (int i = 0; i < GAMEAREA_HEIGHT; ++i) {
        for (int j = 0; j < GAMEAREA_WIDTH / 2; ++j) {
            stackedBlocks[i][j] = 0;
        }
    }

    // Create and initialize objects
    currentBlock = getRandomShape();
    currentBlock.position.x = 10;
    currentBlock.position.y = 1;
    self->objects[ID_CURRENT_BLOCK] = &currentBlock;
    nScene->notifyUpdated(nScene, ID_CURRENT_BLOCK);

    gameAreaWall = newBox(sizeOf(GAMEAREA_WIDTH + 2, GAMEAREA_HEIGHT + 1), COLOR_WHITE);
    gameAreaWall.position.x = 0;
    gameAreaWall.position.y = 0;
    self->objects[ID_GAMEAREA_BOX] = &gameAreaWall;
    nScene->notifyUpdated(nScene, ID_GAMEAREA_BOX);

    self->obj_count = 2;

    // Show countdown
    for (int i = 0; i < 3; ++i) {
        setColor(COLOR_WHITE);
        moveto(1, 10);
        switch (i) {
            case 0:
                printf("             _____ \n"
                       "             |___ / \n"
                       "               |_ \\ \n"
                       "              ___) |\n"
                       "             |____/ \n");
                break;
            case 1:
                printf("             ____  \n"
                       "             |___ \\ \n"
                       "               __) |\n"
                       "              / __/ \n"
                       "             |_____|\n");
                break;
            case 2:
                printf("               _     \n"
                       "               / |    \n"
                       "               | |    \n"
                       "               | |    \n"
                       "               |_|    \n");
                break;
            default:
                break;
        }
        gameAreaWall.draw(&gameAreaWall);
        Sleep(1000);
    }
    moveto(1, 10);
    setColor(COLOR_BLACK);
    printf("                      \n"
           "                      \n"
           "                      \n"
           "                      \n"
           "                      \n");

    // Create game update tick thread
    GameUIThread = CreateThread(
            NULL,
            0,
            GameUIThreadFunc,
            NULL,
            0,
            NULL
    );
    //WaitForSingleObject(UIUpdateThread, 30000);
}

void game_onKeyInput(Scene *self, int key) {
    if (gameState != GAME_PLAYING)
        return;

    lastKey = key;
    int moveState = STATE_DEFAULT;
    switch (key) {
        case KEY_LEFT:
            moveState = moveObject(&currentBlock, DIR_LEFT, 2);
            break;
        case KEY_RIGHT:
            moveState = moveObject(&currentBlock, DIR_RIGHT, 2);
            break;
        case KEY_DOWN:
            moveState = moveObject(&currentBlock, DIR_DOWN, 1);
            break;
        case KEY_UP:
            rotateObject(&currentBlock);
            //shiftSpace(&currentBlock);
            moveState = STATE_DEFAULT;
            break;
        default: break;
    }

    if (moveState == STATE_DEFAULT) {
        self->notifyUpdated(self, ID_CURRENT_BLOCK);
        self->_draw(self);
    }
}

void game_onDraw(Scene *self) {
    if (maxShapeHeight != GAMEAREA_HEIGHT) {
        for (int i = max(maxShapeHeight - 1, 0); i < GAMEAREA_HEIGHT - 1; ++i) {
            for (int j = 0; j < GAMEAREA_WIDTH / 2; ++j) {
                int color = stackedBlocks[i][j];
                if (color != 0) {
                    moveto((j + 1) * 2, i + 1);
                    setColor(color);
                    printBlock(BLOCK_FILLED);
                }
            }
        }
    }

    Color color;
    if (gameState == GAME_PLAYING)
        color = COLOR_GREEN;
    else
        color = COLOR_RED;
    for (int i = 3; i < GAMEAREA_WIDTH; i += 2) {
        moveto(i, 1);
        setColor(color);
        printBlock(LINE);
        setColor(COLOR_BLACK);
        printBlock(SPACE);
    }

    setColor(COLOR_BLACK);
    drawLine(&limitStartPos, &limitEndPos, SPACE);

    if (gameState == GAME_OVER) {
        setColor(COLOR_WHITE);
        int midPos = GAMEAREA_HEIGHT / 2;
        moveto(9, midPos - 1);
        printf("                ");
        moveto(9, midPos);
        printf("<< GAME OVER! >>");
        moveto(9, midPos + 1);
        printf("                ");
    }

    if (DEBUG) {
        setColor(COLOR_YELLOW);
        moveto_origin(16, 5, ORIGIN_END);
        printf("score=      %02d", score);
        moveto_origin(16, 4, ORIGIN_END);
        printf("maxHeight=  %02d", maxShapeHeight);
        moveto_origin(16, 3, ORIGIN_END);
        printf("lastKey=    %02d", lastKey);
        moveto_origin(16, 2, ORIGIN_END);
        printf("x, y= (%02d, %02d)", currentBlock.position.x, currentBlock.position.y);
        moveto_origin(16, 1, ORIGIN_END);
        printf("h, w= (%02d, %02d)", currentBlock.size.height, currentBlock.size.width);
    }
}

void game_onDestroy(Scene *self) {
    if (GameUIThread != NULL) {
        CloseHandle(GameUIThread);
        GameUIThread = NULL;

        size_t size = sizeof(int) * GAMEAREA_WIDTH;
        for (int j = GAMEAREA_HEIGHT; j > maxShapeHeight; --j) {
            memset(stackedBlocks[j], 0, size);
        }
    }
}

Scene game_createScene() {
    Scene scene = createScene(
            game_onCreate,
            game_onKeyInput,
            game_onDraw,
            game_onDestroy
    );
    return scene;
}

/*
 * Moved to Shape.c
Position coordOfShape(int num) {
    Position pos;
    pos.x = (short) (num % 4);
    pos.y = (short) (num / 4);
    return pos;
}
 */

void movePosition(Position *pos, Direction dir, int distance) {
    switch (dir) {
        case DIR_UP:
            pos->y -= distance;
            break;
        case DIR_DOWN:
            pos->y += distance;
            break;
        case DIR_RIGHT:
            pos->x += distance;
            break;
        case DIR_LEFT:
            pos->x -= distance;
            break;
        default:
            break;
    }
}

bool checkBound(Position p) {
    return (0 < p.x && p.x < GAMEAREA_WIDTH && 0 <= p.y && p.y < GAMEAREA_HEIGHT);
}

void rotateObject(Object *object) {
    Position center = coordOfShape(object->center);
    Position updated[SHAPE_SIZE];

    Position npos = {0, 0};
    Position cpos = {0, 0};
    for (int i = 0; i < 4; ++i) {
        copyPosition(&npos, &object->shape[i]);
        int hor = center.x - npos.x;
        int ver = center.y - npos.y;
        npos.x = center.x + ver;
        npos.y = center.y - hor;

        copyPosition(&cpos, &npos);
        addPosition(&cpos, &object->position);

        if (!checkBound(cpos))
            return;

        updated[i] = npos;
    }

    size_t memSize = sizeof(Position) * SHAPE_SIZE;
    memcpy_s(&object->shape, memSize, updated, memSize);
    //flipShape(object->shape);
    int tmp = object->size.width;
    object->size.width = object->size.height;
    object->size.height = tmp;
}

int canMove(Object *object, Direction dir, int distance) {
    Position simulatedPos = clonePosition(&object->position);
    movePosition(&simulatedPos, dir, distance);

    bool checkConflict = simulatedPos.y + object->size.height >= maxShapeHeight;
    Position p = {0, 0};
    for (int i = 0; i < SHAPE_SIZE; ++i) {
        copyPosition(&p, &object->shape[i]);
        addPosition(&p, &simulatedPos);

        if (!checkBound(p)) {
            if (p.y >= GAMEAREA_HEIGHT)
                return STATE_CONFLICT;
            else
                return STATE_WALL;
        }

        if (checkConflict && stackedBlocks[p.y - 1][p.x / 2] != SPACE)
            return STATE_CONFLICT;
    }

    return STATE_DEFAULT;
}

int moveObject(Object *object, Direction direction, int distance) {
    int status = canMove(object, direction, distance);
    if (status != STATE_DEFAULT)
        return status;

    movePosition(&(object->position), direction, distance);
    return STATE_DEFAULT;
}