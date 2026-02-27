#pragma once

#include "math.h"
//#include "scene.h"

typedef struct {
    void* window;
    void* glContext;
    void* scene;
    i32 windowWidth;
    i32 windowHeight;
    Matrix4 projection;
    Matrix4 view;
    float time;
    float deltaTime;
} AppContext;

extern i32 InitCanis();
