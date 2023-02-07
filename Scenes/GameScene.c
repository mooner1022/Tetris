//
// Created by moonm on 2022-10-06.
//

#include <stdio.h>
#include <windows.h>
#include "../GlobalValues.h"
#include "../Console/Console.h"
#include "../Engine/GameEngine.h"
#include "../KeyBinding.h"
#include "../Engine/Random.h"
#include "GameScene.h"

static Scene* nScene;
static Object nextBlock;
static Object currentBlock;
static Object gameAreaWall;
static Object nextBlockWall;

Position limitStartPos = { 2, 0 };
Position limitEndPos   = { GAMEAREA_WIDTH, 1 };
//static Object* currentBlock = NULL;

enum {
    Current = 0,
    Next    = 2,
    Hold    = 4
};
int objIndex[6] = { -1, 0, -1, 0, -1, 0 };
int maxShapeHeight = GAMEAREA_HEIGHT;
int excludedShapeLen = 0;
static int excludedShapes[SHAPE_COUNT] = { 0 };
static int stackedBlocks[GAMEAREA_HEIGHT][GAMEAREA_WIDTH / 2] = { 0 };
//static Object blockBuffer[4] = { 0 };

int gameState = GAME_PLAYING;
int totalLineErased = 0;
int level = 1;
int score = 0;
int lastKey = -1;
int orgBest = 0;
bool isHeld = false;

HANDLE GameUIThread = NULL;

void rotateObject(Object *object);
int canMove(Object *object, Direction dir, int distance);
int moveObject(Object *object, Direction direction, int distance);
void dropObject(Object *object);
void holdObject(Object *object);

int readHighScore() {
    FILE* file = fopen("data.json", "r");
    int high = 0;
    fscanf(file, "{\"high_score\": %d}", &high);
    fclose(file);
    return high;
}

void writeHighScore(int h_score) {
    FILE* file = fopen("data.json", "w");
    fprintf(file, "{\"high_score\": %d}", h_score);
    fclose(file);
}

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
    bool isLineFilled;
    for (int i = maxShapeHeight; i < GAMEAREA_HEIGHT; ++i) {
        isLineFilled = true;
        for (int j = 0; j < GAMEAREA_WIDTH / 2; ++j) {
            if (stackedBlocks[i][j] == 0) {
                isLineFilled = false;
                break;
            }
        }
        if (isLineFilled) {
            size_t size = sizeof(int) * (GAMEAREA_WIDTH / 2);
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

bool arr_contains(const int* arr, size_t len, int val) {
    for (int i = 0; i < len; ++i) {
        if (arr[i] == val)
            return true;
    }
    return false;
}

Object getNextBlock() {
    int index;
    do {
        index = range_rand(0, SHAPE_COUNT - 1);
    } while (arr_contains(excludedShapes, excludedShapeLen, index));
    excludedShapes[excludedShapeLen++] = index;
    if (excludedShapeLen == SHAPE_COUNT)
        excludedShapeLen = 0;

    if (objIndex[Next] == -1) {
        objIndex[Next] = index;
        objIndex[Next + 1] = bool_rand();
        return getNextBlock();
    } else {
        if (isHeld)
            isHeld = false;

        objIndex[Current] = objIndex[Next];
        objIndex[Current + 1] = objIndex[Next + 1];
        objIndex[Next] = index;
        objIndex[Next + 1] = bool_rand();
        return getShapeByIndex(objIndex[Current], objIndex[Current + 1]);
    }
}

DWORD WINAPI GameUIThreadFunc(void *args) {

    int moveState = STATE_DEFAULT;

    while (GameUIThread != NULL) {
        if (gameState == GAME_PAUSED) {
            Sleep(1000);
            continue;
        }
        moveState = moveObject(&currentBlock, DIR_DOWN, 1);

        if (moveState == STATE_CONFLICT) {
            assignObjToMap(&currentBlock);
            int filled = checkLineFilled();
            if ((totalLineErased += filled) % 4 == 0)
                level++;
            updateScore(filled);

            if (maxShapeHeight <= 1) {
                gameState = GAME_OVER;
                nScene->_draw(nScene);
                return 0;
            }
            currentBlock = getNextBlock();
            //currentBlock = getShapeByIndex(1, false);
            currentBlock.position.x = (GAMEAREA_WIDTH / 4) * 2;
            currentBlock.position.y = 0;
            nScene->objects[ID_CURRENT_BLOCK] = &currentBlock;

            nextBlock = getShapeByIndex(objIndex[Next], objIndex[Next + 1]);
            nextBlock.position.x = GAMEAREA_WIDTH + 10;
            nextBlock.position.y = 3;

            // Redraw UI when block position fixed
            clear();
            nScene->notifyUpdated(nScene, ID_GAMEAREA_BOX);
            nScene->notifyUpdated(nScene, ID_NEXTBLOCK_BOX);
            nScene->notifyUpdated(nScene, ID_NEXT_BLOCK);
        }
        nScene->notifyUpdated(nScene, ID_CURRENT_BLOCK);
        nScene->_draw(nScene);

        Sleep(max(1000 - (level * 10), 200));
    }
    return 0;
}

void game_onCreate(Scene *self, int prevId) {
    setConsoleSize((Size){22, GAMEAREA_HEIGHT + 1});

    nScene = self;

    gameState = GAME_PLAYING;
    score = 0;
    level = 1;
    totalLineErased = 0;
    maxShapeHeight = GAMEAREA_HEIGHT;
    orgBest = readHighScore();

    limitStartPos = positionOf(2, 0);
    limitEndPos   = positionOf(GAMEAREA_WIDTH + 1, 1);

    // Initialize all elements to 0
    memset(stackedBlocks, 0, sizeof(stackedBlocks));
    memset(excludedShapes, 0, sizeof(excludedShapes));
    //memset(blockBuffer, 0, sizeof(blockBuffer));
    memset(objIndex, -1, sizeof(objIndex));

    // Create and initialize objects
    currentBlock = getNextBlock();
    currentBlock.position.x = 10;
    currentBlock.position.y = 1;
    self->objects[ID_CURRENT_BLOCK] = &currentBlock;
    nScene->notifyUpdated(nScene, ID_CURRENT_BLOCK);

    gameAreaWall = newBox((Size){GAMEAREA_WIDTH + 2, GAMEAREA_HEIGHT + 1}, COLOR_WHITE);
    gameAreaWall.position.x = 0;
    gameAreaWall.position.y = 0;
    self->objects[ID_GAMEAREA_BOX] = &gameAreaWall;
    nScene->notifyUpdated(nScene, ID_GAMEAREA_BOX);

    nextBlockWall = newBox((Size){12, 6}, COLOR_WHITE);
    nextBlockWall.position.x = GAMEAREA_WIDTH + 6;
    nextBlockWall.position.y = 1;
    self->objects[ID_NEXTBLOCK_BOX] = &nextBlockWall;
    nScene->notifyUpdated(nScene, ID_NEXTBLOCK_BOX);

    nextBlock = getShapeByIndex(objIndex[Next], objIndex[Next + 1]);
    nextBlock.position.x = GAMEAREA_WIDTH + 10;
    nextBlock.position.y = 3;
    self->objects[ID_NEXT_BLOCK] = &nextBlock;
    nScene->notifyUpdated(nScene, ID_NEXT_BLOCK);

    self->obj_count = 4;

    // Show countdown
    for (int i = 0; i < 3; ++i) {
        setColor(COLOR_WHITE);
        moveto(1, 8);
        switch (i) {
            case 0:
                printf("         _____ \n"
                       "         |___ / \n"
                       "           |_ \\ \n"
                       "          ___) |\n"
                       "         |____/ \n");
                break;
            case 1:
                printf("         ____  \n"
                       "         |___ \\ \n"
                       "           __) |\n"
                       "          / __/ \n"
                       "         |_____|\n");
                break;
            case 2:
                printf("           _     \n"
                       "           / |    \n"
                       "           | |    \n"
                       "           | |    \n"
                       "           |_|    \n");
                break;
            default:
                break;
        }
        gameAreaWall.draw(&gameAreaWall);
        Sleep(1000);
    }
    moveto(1, 8);
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
    lastKey = key;
    int moveState = STATE_DEFAULT;

    if (gameState == GAME_PLAYING) {
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
                break;
            case 'c':
                holdObject(&currentBlock);
                break;
            case KEY_SPACE:
                dropObject(&currentBlock);
                break;
            case 'p':
                gameState = GAME_PAUSED;
                break;
            default: break;
        }
    } else if (gameState == GAME_PAUSED) {
        gameState = GAME_PLAYING;
        nScene->notifyUpdated(nScene, ID_GAMEAREA_BOX);
        nScene->notifyUpdated(nScene, ID_NEXTBLOCK_BOX);
        nScene->notifyUpdated(nScene, ID_NEXT_BLOCK);
        clear();
        self->_draw(self);
    } else {
        switch (key) {
            case 'q':
                getGameInstance()->showScene(getGameInstance(), SCENE_MAIN);
                moveState = STATE_WALL;
                break;
            case 'r': // R
                clear();
                //game_onCreate(self, SCENE_GAME);
                getGameInstance()->showScene(getGameInstance(), SCENE_GAME);
                break;
            default: break;
        }
    }

    if (moveState == STATE_DEFAULT) {
        self->notifyUpdated(self, ID_CURRENT_BLOCK);
        self->_draw(self);
    }
}

void printScore() {
    moveto(GAMEAREA_WIDTH + 6, 9);
    setColor(COLOR_SKYBLUE);
    printf("Best  : ");
    setColor(COLOR_WHITE);
    printf("%5d", max(orgBest, score));

    moveto(GAMEAREA_WIDTH + 6, 10);
    setColor(COLOR_SKYBLUE);
    printf("Score : ");
    setColor(COLOR_WHITE);
    printf("%5d", score);

    moveto(GAMEAREA_WIDTH + 6, 11);
    setColor(COLOR_SKYBLUE);
    printf("Level : ");
    setColor(COLOR_WHITE);
    printf("%5d", level);
}

void printDebugInfo() {
    setColor(COLOR_YELLOW);
    moveto_origin(16, 5, ORIGIN_END);
    printf("[%d %d %d]", objIndex[Current], objIndex[Next], objIndex[Hold]);
    moveto_origin(16, 4, ORIGIN_END);
    printf("maxHeight=  %02d", maxShapeHeight);
    moveto_origin(16, 3, ORIGIN_END);
    printf("lastKey=    %02d", lastKey);
    moveto_origin(16, 2, ORIGIN_END);
    printf("x, y= (%02d, %02d)", currentBlock.position.x, currentBlock.position.y);
    moveto_origin(16, 1, ORIGIN_END);
    printf("h, w= (%02d, %02d)", currentBlock.size.height, currentBlock.size.width);
}

void printGameKeys(int state) {
    setColor(COLOR_WHITE);
    switch (state) {
        case GAME_PLAYING:
            moveto(GAMEAREA_WIDTH + 6, 15);
            wprintf(L"    △");
            moveto(GAMEAREA_WIDTH + 6, 16);
            printf("   TURN");
            moveto(GAMEAREA_WIDTH + 6, 17);
            wprintf(L"◁L    R▷");
            moveto(GAMEAREA_WIDTH + 6, 18);
            printf("   DOWN");
            moveto(GAMEAREA_WIDTH + 6, 19);
            wprintf(L"    ▽");
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

    setColor(COLOR_BLACK);
    drawLine(&limitStartPos, &limitEndPos, SPACE);

    moveto(GAMEAREA_WIDTH + 9, 0);
    setColor(COLOR_WHITE);
    printf("- NEXT -");

    printScore();

    int midPos = GAMEAREA_HEIGHT / 2;
    Color color;
    switch (gameState) {
        case GAME_PLAYING:
            color = COLOR_GREEN;
            break;
        case GAME_PAUSED:
            color = COLOR_YELLOW;
            setColor(COLOR_WHITE);
            moveto(4, midPos);
            printf("<< GAME PAUSED >>");
            moveto(6, midPos + 1);
            printf("press any key");
            moveto(8, midPos + 2);
            printf("to resume");
            break;
        case GAME_OVER:
            color = COLOR_RED;
            setColor(COLOR_WHITE);
            moveto(4, midPos - 1);
            printf("                ");
            moveto(4, midPos);
            printf("<< GAME OVER! >>");
            moveto(4, midPos + 1);
            printf("                ");
            moveto(3, midPos + 2);
            printf("Press R to restart");
            moveto(3, midPos + 3);
            printf("Press Q to go home");
            break;
        default:
            break;
    }

    for (int i = 3; i < GAMEAREA_WIDTH; i += 2) {
        moveto(i, 1);
        setColor(color);
        printf("-");
        //printBlock(LINE);
        //setColor(COLOR_BLACK);
        //printBlock(SPACE);
        printf("  ");
    }

    if (DEBUG)
        printDebugInfo();
    else
        printGameKeys(gameState);
}

void game_onDestroy(Scene *self) {
    if (orgBest < score)
        writeHighScore(score);

    if (GameUIThread != NULL) {
        CloseHandle(GameUIThread);
        GameUIThread = NULL;

        size_t size = sizeof(int) * GAMEAREA_WIDTH;
        for (int j = GAMEAREA_HEIGHT; j > maxShapeHeight; --j) {
            memset(stackedBlocks[j], 0, size);
        }
    }
}

Scene game_createScene(int id) {
    Scene scene = createScene(
            id,
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
    return (0 < p.x && p.x <= GAMEAREA_WIDTH && 0 <= p.y && p.y < GAMEAREA_HEIGHT);
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
    Position p = { 0, 0 };
    for (int i = 0; i < SHAPE_SIZE; ++i) {
        copyPosition(&p, &object->shape[i]);
        p.x *= 2;
        addPosition(&p, &simulatedPos);

        if (!checkBound(p)) {
            if (p.y >= GAMEAREA_HEIGHT)
                return STATE_CONFLICT;
            else
                return STATE_WALL;
        }

        if (checkConflict && stackedBlocks[p.y - 1][p.x / 2 - 1] != SPACE)
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

void dropObject(Object *object) {
    int dist = 0;

    while (canMove(object, DIR_DOWN, ++dist) == STATFLAG_DEFAULT);

    movePosition(&(object->position), DIR_DOWN, dist - 1);
}

void holdObject(Object *object) {
    if (isHeld) return;

    if (objIndex[Hold] == -1) {
        objIndex[Hold] = objIndex[Current];
        currentBlock = getNextBlock();
    } else {
        int tmp = objIndex[Hold];
        objIndex[Hold] = objIndex[Current];
        objIndex[Current] = tmp;
        tmp = objIndex[Hold + 1];
        objIndex[Hold + 1] = objIndex[Current + 1];
        objIndex[Current + 1] = tmp;
        currentBlock = getShapeByIndex(objIndex[Current], objIndex[Current + 1]);
    }
    currentBlock.position.x = (GAMEAREA_WIDTH / 4) * 2;
    isHeld = true;
}