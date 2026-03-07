#pragma once
#include "cpup/canis.h"
#include "cpup/math.h"
#include "cpup/scene.h"
#include "cpup/model.h"
#include "cpup/io.h"
#include "cpup/inputmanager.h"
#include <stdio.h>

typedef struct {
    int var;
    Vector2 bounceVelocity;
} Paddle;

void PaddleStart(AppContext* _app, Entity* _entity) {
    if(_entity->name == "LeftPaddle")
        _entity->color = InitVector4(1.0f, 0.1f, 0.1f, 1.0f);
    else
        _entity->color = InitVector4(0.1f, 0.2f, 1.0f, 1.0f);

    _entity->transform.rotation = 0.0f;
    _entity->transform.scale = InitVector3(32.0f, 128.0f, 1.0f);

}

void PaddleUpdate(AppContext* _app, Entity* _entity) {

    if (GetKey(_app, SDL_SCANCODE_S) && _entity->name == "LeftPaddle")
    {
        _entity->velocity = InitVector2(0.0f, -1.0f * 150.0f);

        Vector3 nextPos = Vec3Add(_entity->transform.position, Vec2ToVec3(Vec2Mul(_entity->velocity, _app->deltaTime)));
        
        if(nextPos.y > 0.0f + _entity->transform.scale.y / 2)
            _entity->transform.position = nextPos;
    }

    else if (GetKey(_app, SDL_SCANCODE_W) && _entity->name == "LeftPaddle")
    {
        _entity->velocity = InitVector2(0.0f, 1.0f * 150.0f);

        Vector3 nextPos = Vec3Add(_entity->transform.position, Vec2ToVec3(Vec2Mul(_entity->velocity, _app->deltaTime)));
        
        if(nextPos.y < 600.0f - _entity->transform.scale.y / 2)
            _entity->transform.position = nextPos;
    }

    if (GetKey(_app, SDL_SCANCODE_DOWN) && _entity->name == "RightPaddle")
    {
        //printf("%f", _entity->transform.position.y);

        _entity->velocity = InitVector2(0.0f, -1.0f * 150.0f);

        //_entity->transform.position = Vec3Add(_entity->transform.position, Vec2ToVec3(Vec2Mul(_entity->velocity, _app->deltaTime)));

        Vector3 nextPos = Vec3Add(_entity->transform.position, Vec2ToVec3(Vec2Mul(_entity->velocity, _app->deltaTime)));
        
        if(nextPos.y > 0.0f + _entity->transform.scale.y / 2)
            _entity->transform.position = nextPos;
        
    }

    else if (GetKey(_app, SDL_SCANCODE_UP) && _entity->name == "RightPaddle")
    {
        _entity->velocity = InitVector2(0.0f, 1.0f * 150.0f);

        Vector3 nextPos = Vec3Add(_entity->transform.position, Vec2ToVec3(Vec2Mul(_entity->velocity, _app->deltaTime)));
        
        if(nextPos.y < 600.0f - _entity->transform.scale.y / 2)
            _entity->transform.position = nextPos;
    }

    // Decay bounce for visual effect
    Paddle* paddle = (Paddle*)_entity->data;
    paddle->bounceVelocity = Vec2Mul(paddle->bounceVelocity, 0.9f);
}

void PaddleDraw(AppContext* _app, Entity* _entity) {
    Matrix4 transform = IdentityMatrix4(); // the order is important
    Mat4Translate(&transform, _entity->transform.position);
    Mat4Rotate(&transform, _entity->transform.rotation * DEG2RAD, InitVector3(0.0f, 0.0f, 1.0f));
    Mat4Scale(&transform, InitVector3(_entity->transform.scale.x, _entity->transform.scale.y, _entity->transform.scale.z));

    BindShader(_entity->shaderId);

    ShaderSetFloat(_entity->shaderId, "TIME", _app->time);
    ShaderSetMatrix4(_entity->shaderId, "VIEW", _app->view);
    ShaderSetMatrix4(_entity->shaderId, "PROJECTION", _app->projection);

    Paddle* paddle = (Paddle*)_entity->data;
    ShaderSetFloat(_entity->shaderId, "BOUNCE_X", paddle->bounceVelocity.x / _entity->transform.scale.x);
    ShaderSetVector4(_entity->shaderId, "COLOR", _entity->color);
    ShaderBindTexture(_entity->shaderId, _entity->image->id, "MAIN_TEXTURE", 0);
    ShaderSetMatrix4(_entity->shaderId, "TRANSFORM", transform);
    DrawModel(*_entity->model);

    UnBindShader();
}

void PaddleOnDestroy(AppContext* _app, Entity* _entity) {

}
