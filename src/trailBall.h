#pragma once
#include "cpup/canis.h"
#include "cpup/scene.h"
#include "cpup/model.h"
#include "cpup/inputmanager.h"
#include "paddle.h"

#include <SDL3/SDL.h>
#include <stdio.h>

Entity* SpawnTrailNode(AppContext* _app, Entity* _entity);

void GhostBallStart(AppContext* _app, Entity* _entity) {
    _entity->color = Find(&(_app->scene), "Ball")->color;
    //_entity->color = InitVector4(1.0f, 1.0f, 1.0f, 1.0f);
    _entity->color.w -= 0.5f;

    _entity->transform.scale = InitVector3(32.0f, 32.0f, 1.0f);
}

void GhostBallUpdate(AppContext* _app, Entity* _entity) {

    f32 time = 0.0f;

    Entity* leftPaddle = Find(&(_app->scene), "LeftPaddle");
    Entity* rightPaddle = Find(&(_app->scene), "RightPaddle");

    // Handle post-game logic
    if(_app->scene->gameEnded || _entity->transform.scale.x <= 0.05) {
        //may crash program
        Destroy(_app, &(_app->scene), _entity->id);
        return;
    }

    Vector3 delta = Vec2ToVec3(Vec2Mul(_entity->velocity, _app->deltaTime));
    _entity->transform.position = Vec3Add(_entity->transform.position, delta);

}

void GhostBallDraw(AppContext* _app, Entity* _entity) {

    // Draw ghost ball
    Matrix4 transform = IdentityMatrix4(); // the order is important
    Mat4Translate(&transform, _entity->transform.position);
    Mat4Rotate(&transform, _entity->transform.rotation * DEG2RAD, InitVector3(0.0f, 0.0f, 1.0f));

    _entity->transform.scale = Vec3Mul(_entity->transform.scale, 0.995);
    Mat4Scale(&transform, InitVector3(_entity->transform.scale.x, _entity->transform.scale.y, _entity->transform.scale.z));


    BindShader(_entity->shaderId);

    ShaderSetFloat(_entity->shaderId, "TIME", _app->time);
    ShaderSetMatrix4(_entity->shaderId, "VIEW", _app->view);
    ShaderSetMatrix4(_entity->shaderId, "PROJECTION", _app->projection);

    ShaderSetVector4(_entity->shaderId, "COLOR", _entity->color);
    ShaderBindTexture(_entity->shaderId, _entity->image->id, "MAIN_TEXTURE", 0);
    ShaderSetMatrix4(_entity->shaderId, "TRANSFORM", transform);
    DrawModel(*_entity->model);

    UnBindShader();
}

void GhostBallOnDestroy(AppContext* _app, Entity* _entity) {

}

Entity* SpawnTrailNode(AppContext* _app, Entity* _entity) {

    Entity* ball = Spawn(&_app->scene);
    
    ball->transform.position = _entity->transform.position;
    ball->image = _entity->image;
    ball->model = _entity->model;
    ball->shaderId = _entity->shaderId;
    ball->Start = GhostBallStart;
    ball->Update = GhostBallUpdate;
    ball->Draw = GhostBallDraw;
    ball->OnDestroy = GhostBallOnDestroy;

    return ball;
}
