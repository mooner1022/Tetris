cmake_minimum_required(VERSION 3.23)
project(Tetris C)

set(CMAKE_C_STANDARD 99)

add_executable(Tetris main.c KeyBinding.h Console/Console.h Console/Console.c Engine/Object.h Engine/Object.c Engine/GameEngine.h Engine/GameEngine.c Scene/MainScene.h Scene/MainScene.c Scene/GameScene.h Scene/GameScene.c GlobalValues.h)
