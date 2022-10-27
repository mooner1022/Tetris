//
// Created by moonm on 2022-09-29.
//

#ifndef TETRIS_OBJECT_H
#define TETRIS_OBJECT_H

#include "GameEngine.h"
#include "../Console/Console.h"

#define T_SHAPE 0
#define T_LINE  1
#define T_BOX   2

#define MAX_WIDTH   4
#define MAX_HEIGHT  4

#define SHAPE_COUNT 5
#define SHAPE_SIZE  4

typedef short ObjectType;
typedef int Shape;
typedef int MappedData;

typedef struct object_t {
    ObjectType type;
    Size size;
    Position position;
    short color;
    short direction;
    Shape center;
    Position shape[SHAPE_SIZE];

    void (* draw)(object_t *self);
    void (* erase)(object_t *self);
} Object;

Position coordOfShape(int num);

void object_t_draw(object_t *self);

Object newObject(ObjectType type, Size size, short color, const Shape shape[SHAPE_SIZE], Shape center);
Object newShape(Size size, short color, const Shape shape[SHAPE_SIZE], Shape center);
Object newLine(Size size, short color);
Object newBox(Size size, short color);
void cloneObject(Object* original, Object* new);

Size parseShape(MappedData retMap[MAX_HEIGHT][MAX_WIDTH], const int shape[SHAPE_SIZE]);
void flipShape(Shape shape[SHAPE_SIZE]);
void shiftSpace(Object *object);

Object getShapeByIndex(int index, bool flip);
Object getRandomShape();

#endif