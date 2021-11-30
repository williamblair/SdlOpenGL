#!/bin/bash
#g++ -std=c++11 TestMain.cpp glad.c Display.cpp Shader.cpp Object.cpp -o TestMain -I./ -lglfw -lGLEW -lGLU -lGL -lstdc++ -ldl
g++ -std=c++11 main.cpp Display.cpp Shader.cpp Object.cpp -o main -I./ -lSDL2 -lGLEW -lGLU -lGL -lstdc++ -ldl
