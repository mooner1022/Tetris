//
// Created by moonm on 2022-09-29.
//

#ifndef TETRIS_CONSOLE_H
#define TETRIS_CONSOLE_H

#include "../Engine/Object.h"

#define ORIGIN_START 0
#define ORIGIN_END   1

#define COLOR_BLACK   0x0
#define COLOR_BLUE    0x9
#define COLOR_SKYBLUE 0xb
#define COLOR_RED     0x4
#define COLOR_GREEN   0xa
#define COLOR_YELLOW  (COLOR_RED | COLOR_GREEN)
#define COLOR_MAGENTA 0xd
#define COLOR_WHITE   0xf

#define SPACE        0
#define BLOCK_EMPTY  1
#define BLOCK_FILLED 2
#define BLOCK_WALL   3
#define DOT          4
#define LINE         5

typedef unsigned short Color;
typedef short Origin;

void initConsole();
void clear();
void setColor(Color color);
void moveto(int x, int y);
void moveto_origin(int x, int y, Origin origin);
void hideCursor();
Size getConsoleSize();

void printBlock(short type);
void drawLine(Position* start, Position* end, short blockType);
Color getRandomColor();

#endif
