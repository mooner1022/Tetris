//
// Created by moonm on 2022-09-29.
//

#include <string.h>
#include <time.h>
#include <stdlib.h>
#include "../Console/Console.h"
#include "Object.h"

// [0~3] = Absolute position of mData, [4] = isFlippable(0: false, 1: true), [5~6] = width, height of shape, [7] = Color of shape, [8] = center pos
/*
 * [0  1  2  3  ┐ Used for default shapes
 *  4  5  6  7  ┘
 *  8  9 10 11  ┐ Used for rotated shapes
 * 12 13 14 15] ┘
 */
Shape shapes[SHAPE_COUNT][SHAPE_SIZE + 5] = {
        {0, 1, 4, 5, 0, 2, 2, COLOR_YELLOW , 0}, // 2x2
        {0, 1, 2, 3, 0, 4, 1, COLOR_SKYBLUE, 2}, // 1x4
        {0, 1, 5, 6, 1, 3, 2, COLOR_RED    , 5}, // z block
        {0, 4, 5, 6, 1, 3, 2, COLOR_BLUE   , 5}, // L block
        {1, 4, 5, 6, 0, 3, 2, COLOR_MAGENTA, 5}  // T block
};

void movetoPos(Position *position) {
    moveto(position->x, position->y);
}

Position coordOfShape(int num) {
    Position pos;
    pos.x = num % 4;
    pos.y = num / 4;

    if (num < 0)
        pos.x *= -1;
    return pos;
}

/* draw() and erase() mapping for each types */

// Shape
void object_t_draw_shape(object_t *self) {
    setColor(self->color);

    Position coord;
    for (int i = 0; i < SHAPE_SIZE; ++i) {
        coord = self->shape[i];

        moveto(self->position.x + (coord.x * 2), self->position.y + coord.y);
        printBlock(BLOCK_EMPTY);
    }
}

void object_t_erase_shape(object_t *self) {
    setColor(COLOR_BLACK);

    Position coord;
    for (int i = 0; i < SHAPE_SIZE; ++i) {
        coord = self->shape[i];

        moveto(self->position.x + (coord.x * 2), self->position.y + coord.y);
        printBlock(SPACE);
    }
}

// Line
void object_t_draw_line(object_t *self) {
    setColor(self->color);
    movetoPos(&self->position);

    for (int i = 0; i < self->size.height; ++i) {
        for (int j = 0; j < self->size.width; ++j) {
            printBlock(BLOCK_FILLED);
        }
        moveto(self->position.x, (i + 1) + self->position.y);
    }
}

void object_t_erase_line(object_t *self) {
    setColor(COLOR_BLACK);
    movetoPos(&self->position);

    for (int i = 0; i < self->size.height; ++i) {
        for (int j = 0; j < self->size.width; ++j) {
            printBlock(SPACE);
        }
        moveto(self->position.x, (i + 1) + self->position.y);
    }
}

// Box
void object_t_draw_box(object_t *self) {
    setColor(self->color);
    movetoPos(&self->position);

    /*
     * p0   p1
     *
     * p2   p3
     */
    Position pos1 = positionOf(self->position.x + self->size.width, self->position.y);
    Position pos2 = positionOf(self->position.x, self->position.y + self->size.height - 1);
    Position pos3 = positionOf(self->position.x + self->size.width + 1, self->position.y + self->size.height);

    drawLine(&self->position, &pos1, BLOCK_WALL);
    drawLine(&self->position, &pos2, BLOCK_WALL);
    drawLine(&pos1, &pos3, BLOCK_WALL);
    drawLine(&pos2, &pos3, BLOCK_WALL);
}

void object_t_erase_box(object_t *self) {
    setColor(COLOR_BLACK);
    movetoPos(&self->position);

    Position pos1 = positionOf(self->position.x + self->size.width, self->position.y);
    Position pos2 = positionOf(self->position.x, self->position.y + self->size.height - 1);
    Position pos3 = positionOf(self->position.x + self->size.width + 1, self->position.y + self->size.height);

    drawLine(&self->position, &pos1, SPACE);
    drawLine(&self->position, &pos2, SPACE);
    drawLine(&pos1, &pos3, SPACE);
    drawLine(&pos2, &pos3, SPACE);
}

void object_t_draw(object_t *self) {
    switch (self->type) {
        case T_SHAPE:
            object_t_draw_shape(self);
            break;
        case T_LINE:
            object_t_draw_line(self);
            break;
        case T_BOX:
            object_t_draw_box(self);
            break;
    }
}

void object_t_erase(object_t *self) {
    switch (self->type) {
        case T_SHAPE:
            object_t_erase_shape(self);
            break;
        case T_LINE:
            object_t_erase_line(self);
            break;
        case T_BOX:
            object_t_erase_box(self);
            break;
    }
}

Object newObject(ObjectType type, Size size, short color, const Shape shape[SHAPE_SIZE + 1], Shape center) {
    Object nShape;
    nShape.type = type;
    nShape.size = size;
    nShape.color = color;

    Position nPosition;
    nPosition.x = 0;
    nPosition.y = 0;
    nShape.position = nPosition;

    if (type == T_SHAPE) {
        size_t memSize = sizeof(Position);
        for (int i = 0; i < SHAPE_SIZE; ++i) {
            Position sPos = coordOfShape(shape[i]);
            memcpy_s(&nShape.shape[i], memSize, &sPos, memSize);
        }
        nShape.center = center;
    }

    nShape.draw = object_t_draw;
    nShape.erase = object_t_erase;

    return nShape;
}

Object newShape(Size size, short color, const Shape shape[SHAPE_SIZE], Shape center) {
    return newObject(T_SHAPE, size, color, shape, center);
}

Object newLine(Size size, short color) {
    Shape dummy[SHAPE_SIZE] = { 0 };
    return newObject(T_LINE, size, color, dummy, 0);
}

Object newBox(Size size, short color) {
    Shape dummy[SHAPE_SIZE] = { 0 };
    return newObject(T_BOX, size, color, dummy, 0);
}

void cloneObject(Object* original, Object* new) {
    memcpy_s(new, sizeof(Object), original, sizeof(Object));
}

/*
Size parseShape(MappedData retMap[MAX_HEIGHT][MAX_WIDTH], const int shape[SHAPE_SIZE]) {
    //int retMap[MAX_WIDTH][MAX_HEIGHT] = { 0 };
    int x, y, maxX = 0, maxY = 0;

    // 초기화
    for (int i = 0; i < MAX_HEIGHT; ++i) {
        for (int j = 0; j < MAX_WIDTH; ++j) {
            retMap[i][j] = 0;
        }
    }

    for (int i = 0; i < SHAPE_SIZE; ++i) {
        x = shape[i] % 4;
        y = shape[i] / 4;

        if (maxY < y)
            maxY = y;
        if (maxX < x)
            maxX = x;

        retMap[y][x] = 1;
    }

    return sizeOf((short)(maxX + 1), (short)(maxY + 1));
}
 */

void flipShape(Shape shape[SHAPE_SIZE]) {

    for (int i = 0; i < SHAPE_SIZE; ++i) {
        int div = shape[i] / 4;
        int tmp = shape[i] % 4;
        shape[i] = abs(tmp - 3) + (4 * div) - 1;
    }
}

/*
 *  Unused
int recursiveShift(Shape shape[SHAPE_SIZE], int index) {
    bool shouldShiftX = true;
    bool shouldShiftY = true;

    for (int i = 0; i < SHAPE_SIZE; ++i) {
        int cursor = shape[i];
        if (cursor % 4 == 0) {
            shouldShiftX = false;
            if (!shouldShiftY)
                break;
        }
        if (cursor / 4 == 0) {
            shouldShiftY = false;
            if (!shouldShiftX)
                break;
        }
    }

    if (shouldShiftX) {
        for (int i = 0; i < SHAPE_SIZE; ++i) {
            shape[i]--;
        }
    }
    if (shouldShiftY) {
        for (int i = 0; i < SHAPE_SIZE; ++i) {
            int tmp = shape[i];
            shape[i] = ((tmp / 4) - 1) + (tmp % 4);
        }
    }

    if (!shouldShiftX && !shouldShiftY)
        return index;
    else
        return recursiveShift(shape, ++index);
}

void shiftSpace(Object *object) {
    recursiveShift(object->shape, 0);
}
 */

void cloneShape(Shape clonedShape[SHAPE_SIZE], const Shape orgShape[SHAPE_SIZE]) {
    for (int i = 0; i < SHAPE_SIZE; ++i) {
        clonedShape[i] = orgShape[i];
    }
}

Object getShapeByIndex(int index, bool flip) {
    Shape selected[SHAPE_SIZE + 5];
    size_t memSize = (SHAPE_SIZE + 5) * sizeof(Shape);
    memcpy_s(selected, memSize, shapes[index], memSize);

    Shape shape[SHAPE_SIZE];
    cloneShape(shape, selected);

    Size size;
    size.width  = (short)selected[5];
    size.height = (short)selected[6];

    if (flip && selected[4] == 1)
        flipShape(shape);

    return newShape(size, (short)selected[7], shape, selected[8]);
}

Object getRandomShape() {
    srand((unsigned int)(time(NULL)));
    int randNum = rand() % SHAPE_COUNT;

    return getShapeByIndex(randNum, rand() % 2 == 1);
}