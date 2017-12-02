#!/bin/bash
#g++ -std=c++11 TestMain.cpp glad.c Display.cpp Shader.cpp Object.cpp -o TestMain -I./ -lglfw -lGLEW -lGLU -lGL -lstdc++ -ldl
g++ -std=c++11 TestMain.cpp Display.cpp Shader.cpp Object.cpp -o TestMain -I./ -lSDL2 -lSDL2main -lGLEW -lGLU -lGL -lstdc++ -ldl
