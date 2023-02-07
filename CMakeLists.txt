cmake_minimum_required(VERSION 3.23)
project(Tetris C)

set(CMAKE_C_STANDARD 99)

add_executable(Tetris main.c KeyBinding.h Console/Console.h Console/Console.c Engine/Object.h Engine/Object.c Engine/GameEngine.h Engine/GameEngine.c Scenes/MainScene.h Scenes/MainScene.c Scenes/GameScene.h Scenes/GameScene.c GlobalValues.h Engine/Scene/Scene.c Engine/Scene/Scene.h Scenes/InfoScene.h Scenes/InfoScene.c Engine/Random.h Engine/Random.c)
