//
// Created by moonm on 2022-09-29.
//

#include <stdio.h>
#include <windows.h>
#include <locale.h>
#include <conio.h>
#include <time.h>
#include "../Engine/GameEngine.h"
#include "Console.h"

HANDLE getHandle() {
    return GetStdHandle(STD_OUTPUT_HANDLE);
}

void initConsole() {
    setlocale(LC_CTYPE, "");
    hideCursor();
}

void clear() {
    system("cls");
}

void setColor(Color color) {
    SetConsoleTextAttribute(getHandle(), color);
}

void moveto(int x, int y) {
    moveto_origin(x, y, ORIGIN_START);
}

void moveto_origin(int x, int y, Origin origin) {
    COORD pos;
    if (origin == ORIGIN_END) {
        Size console = getConsoleSize();
        pos.X = console.width - x;
        pos.Y = console.height - y;
    } else {
        pos.X = x;
        pos.Y = y;
    }

    SetConsoleCursorPosition(getHandle(), pos);
}

void hideCursor() {
    CONSOLE_CURSOR_INFO info;
    info.dwSize = 100;
    info.bVisible = FALSE;
    SetConsoleCursorInfo(getHandle(), &info);
}

Size getConsoleSize() {
    CONSOLE_SCREEN_BUFFER_INFO bufferInfo;
    Size size;

    GetConsoleScreenBufferInfo(getHandle(), &bufferInfo);
    size.width = (short)(bufferInfo.srWindow.Right - bufferInfo.srWindow.Left + 1);
    size.height = (short)(bufferInfo.srWindow.Bottom - bufferInfo.srWindow.Top + 1);
    return size;
}

void printBlock(short type) {
    unsigned short *ch;

    switch (type) {
        case SPACE:
            ch = L"  ";
            break;
        case DOT:
            ch = L"\u2022";
            break;
        case BLOCK_EMPTY:
            ch = L"\u25A1";
            break;
        case BLOCK_FILLED:
            ch = L"\u25A0";
            break;
        case BLOCK_WALL:
            setColor(BACKGROUND_RED | BACKGROUND_GREEN);
            ch = L"  ";
            break;
        case LINE:
            wprintf(L"─");
            return;
    }

    wprintf(L"%ls", ch);
}

void drawLine(Position* start, Position* end, short blockType) {
    int diffX, diffY;
    diffX = end->x - start->x;
    diffY = end->y - start->y;

    if (diffX >= diffY) {
        for (int x = start->x; x < end->x; ++x) {
            int y = start->y + diffY * (x - start->x) / diffX;
            moveto(x, y);
            printBlock(blockType);
        }
    } else {
        for (int y = start->y; y < end->y; ++y) {
            int x = start->x + diffX * (y - start->y) / diffY;
            moveto(x, y);
            printBlock(blockType);
        }
    }
}

unsigned short colors[] = {
        COLOR_BLUE,
        COLOR_RED,
        COLOR_YELLOW,
        COLOR_GREEN
};
Color getRandomColor() {
    srand((unsigned int)(time(NULL)));
    return colors[rand() % (sizeof(colors) / sizeof(unsigned short))];
}