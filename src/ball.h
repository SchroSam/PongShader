#pragma once
#include "cpup/canis.h"
#include "cpup/scene.h"
#include "cpup/model.h"
#include "cpup/inputmanager.h"
#include "paddle.h"

#include <SDL3/SDL.h>
#include <stdio.h>

typedef struct {
    int leftScore;
    int rightScore;
    Vector3 trail[100];
} Ball;

Entity* SpawnBall(AppContext* _app, Entity* _entity);

void EndGame(AppContext* _app, Entity* _entity);

static f32 gameEndStartTime = 0.0f;

// winner == true means left player wins, winner == false means right player wins
static bool winner = false;

void BallStart(AppContext* _app, Entity* _entity) {
    _entity->color = InitVector4(1.0f, 1.0f, 1.0f, 1.0f);

    if(gameEndStartTime != 0.0f){

        if(winner == false)
            _entity->color = InitVector4(1.0f, 0.1f, 0.1f, 1.0f);
        else
            _entity->color = InitVector4(0.1f, 0.2f, 1.0f, 1.0f);

    }

        _entity->transform.scale = InitVector3(32.0f, 32.0f, 1.0f);

    Ball* ball = (Ball*)_entity->data;
    // ball->trailIndex = 0;
    // ball->trailUpdateCounter = 0;
    // for(int i = 0; i < 10; i++) {
    //     ball->trail[i] = InitVector3(-1000.0f, -1000.0f, 0.0f);
    // }
}

void BallUpdate(AppContext* _app, Entity* _entity) {

    f32 time = 0.0f;

    Ball* ball = (Ball*)_entity->data;

    Entity* leftPaddle = Find(&(_app->scene), "LeftPaddle");
    Entity* rightPaddle = Find(&(_app->scene), "RightPaddle");

    //printf("%f\n", _entity->velocity.x);

    //if (GetKeyDown(_app, SDL_SCANCODE_P))
    //{
    //    SpawnBall(_app, _entity);
    //}

    if (Vec2EqualsZero(_entity->velocity) && GetKeyDown(_app, SDL_SCANCODE_SPACE))
    {
        i32 startingDirection = rand() % 4;

        static Vector2 directions[4] = {
            (Vector2){0.72f, 0.72f},
            (Vector2){0.72f, -0.72f},
            (Vector2){-0.72f, 0.72f},
            (Vector2){-0.72f, -0.72f},
        };

        _entity->velocity = Vec2Mul(directions[startingDirection], 150.0f);
    }

    if(_app->scene->gameEnded == false) // only check for collisions if the game is still going
    {

        // check if ball is heading below the screen
        if (_entity->transform.position.y - _entity->transform.scale.y * 0.5f <= 0.0f && _entity->velocity.y < 0.0f)
            _entity->velocity.y *= -1.0f; 
        
        // check if ball is heading above the screen
        if (_entity->transform.position.y + _entity->transform.scale.y * 0.5f >= _app->windowHeight && _entity->velocity.y > 0.0f)
            _entity->velocity.y *= -1.0f; 


        // Left Paddle collision
        if(_entity->transform.position.x - _entity->transform.scale.x * 0.5f <= leftPaddle->transform.position.x + leftPaddle->transform.scale.x * 0.5f && 
        _entity->transform.position.x + _entity->transform.scale.x * 0.5f > leftPaddle->transform.position.x  - leftPaddle->transform.scale.x * 0.5f &&
        _entity->transform.position.y - _entity->transform.scale.x * 0.5f <= leftPaddle->transform.position.y + leftPaddle->transform.scale.y * 0.5f && 
        _entity->transform.position.y + _entity->transform.scale.x * 0.5f > leftPaddle->transform.position.y  - leftPaddle->transform.scale.y * 0.5f)
        {
            _entity->velocity.x = 0.72f * 150;
            _entity->color = leftPaddle->color;
            ((Paddle*)leftPaddle->data)->bounceVelocity = InitVector2(10.0f, 0.0f);
        }

        // Right Paddle collision
        if(_entity->transform.position.x + _entity->transform.scale.x * 0.5f > rightPaddle->transform.position.x  - rightPaddle->transform.scale.x * 0.5f && 
        _entity->transform.position.x - _entity->transform.scale.x * 0.5f <= rightPaddle->transform.position.x + rightPaddle->transform.scale.x * 0.5f &&
        _entity->transform.position.y + _entity->transform.scale.x * 0.5f > rightPaddle->transform.position.y  - rightPaddle->transform.scale.y * 0.5f && 
        _entity->transform.position.y - _entity->transform.scale.x * 0.5f <= rightPaddle->transform.position.y + rightPaddle->transform.scale.y * 0.5f)
        {
            _entity->velocity.x = -0.72f * 150;
            _entity->color = rightPaddle->color;
            ((Paddle*)rightPaddle->data)->bounceVelocity = InitVector2(-10.0f, 0.0f);
        }

        // Off screen left
        if(_entity->transform.position.x + _entity->transform.scale.x * 0.5f <= 0.0f)
        {
            _entity->transform.position = InitVector3(_app->windowWidth * 0.5f, _app->windowHeight * 0.5f, 0.0f);
            _entity->velocity = InitVector2(0.0f, 0.0f);
            _entity->color = InitVector4(1.0f, 1.0f, 1.0f, 1.0f);

            char title[50];
            sprintf(title, "Left Score - %d : Right Score - %d", ball->leftScore, ++ball->rightScore);
            SetWindowTitle(_app, title);
        }

        // Off screen right
        if(_entity->transform.position.x - _entity->transform.scale.x * 0.5f >= _app->windowWidth)
        {
            _entity->transform.position = InitVector3(_app->windowWidth * 0.5f, _app->windowHeight * 0.5f, 0.0f);
            _entity->velocity = InitVector2(0.0f, 0.0f);
            _entity->color = InitVector4(1.0f, 1.0f, 1.0f, 1.0f);

            char title[50];
            sprintf(title, "Left Score - %d : Right Score - %d", ++ball->leftScore, ball->rightScore);
            SetWindowTitle(_app, title);
        }

        if(ball->leftScore >= 5 && _app->scene->gameEnded == false){
            winner = false;
            EndGame(_app, _entity);
        }

        else if(ball->rightScore >= 5 && _app->scene->gameEnded == false){
            winner = true;
            EndGame(_app, _entity);
        }
    }

    // Handle post-game logic
    if(_app->scene->gameEnded) {
        if(_app->time - gameEndStartTime >= 3.0f) {
            exit(0);
        }
        if(vec_count(&(_app->scene->entities)) < 3000) {
            SpawnBall(_app, _entity);
        }
    }

    Vector3 delta = Vec2ToVec3(Vec2Mul(_entity->velocity, _app->deltaTime));
    _entity->transform.position = Vec3Add(_entity->transform.position, delta);

    // // Update trail if moving
    // if (Vec2Magnitude(_entity->velocity) > 0.0f) {
    //     ball->trailUpdateCounter++;
    //     if (ball->trailUpdateCounter >= 3) {
    //         ball->trailUpdateCounter = 0;
    //         ball->trail[ball->trailIndex] = _entity->transform.position;
    //         ball->trailIndex = (ball->trailIndex + 1) % 10;
    //     }
    // }
}


void EndGame(AppContext* _app, Entity* _entity){
    _app->scene->gameEnded = true;
    gameEndStartTime = _app->time;
}

void BallDraw(AppContext* _app, Entity* _entity) {
    Ball* ball = (Ball*)_entity->data;

    // Draw trail
    // for(int i = 6; i < 11; i++) {
    //     int idx = (ball->trailIndex - i + 100) % 10;
    //     if (ball->trail[idx].x < -500.0f) continue; // skip invalid
    //     Matrix4 transform = IdentityMatrix4();
    //     Mat4Translate(&transform, ball->trail[idx]);
    //     Mat4Rotate(&transform, _entity->transform.rotation * DEG2RAD, InitVector3(0.0f, 0.0f, 1.0f));
    //     Mat4Scale(&transform, Vec3Mul(InitVector3(_entity->transform.scale.x, _entity->transform.scale.y, _entity->transform.scale.z), 1.0f - (float)(i-5) / 10.0f));

    //     BindShader(_entity->shaderId);

    //     ShaderSetFloat(_entity->shaderId, "TIME", _app->time);
    //     ShaderSetMatrix4(_entity->shaderId, "VIEW", _app->view);
    //     ShaderSetMatrix4(_entity->shaderId, "PROJECTION", _app->projection);

    //     float alpha = 1.0f - (float)i / 10.0f;
    //     ShaderSetVector4(_entity->shaderId, "COLOR", InitVector4(_entity->color.x, _entity->color.y, _entity->color.z, alpha));
    //     ShaderBindTexture(_entity->shaderId, _entity->image->id, "MAIN_TEXTURE", 0);
    //     ShaderSetMatrix4(_entity->shaderId, "TRANSFORM", transform);
    //     DrawModel(*_entity->model);

    //     UnBindShader();
    // }

    // Draw main ball
    Matrix4 transform = IdentityMatrix4(); // the order is important
    Mat4Translate(&transform, _entity->transform.position);
    Mat4Rotate(&transform, _entity->transform.rotation * DEG2RAD, InitVector3(0.0f, 0.0f, 1.0f));
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

void BallOnDestroy(AppContext* _app, Entity* _entity) {

}

Entity* SpawnBall(AppContext* _app, Entity* _entity) {

    Entity* ball = Spawn(&_app->scene);
    
    ball->data = calloc(1, sizeof(Ball));
    ball->image = _entity->image;
    ball->model = _entity->model;
    ball->shaderId = _entity->shaderId;
    ball->Start = BallStart;
    ball->Update = BallUpdate;
    ball->Draw = BallDraw;
    ball->OnDestroy = BallOnDestroy;

    static int ballCount = 0;

    if(++ballCount == 1)
        ball->transform.position = InitVector3(_app->windowWidth * 0.5f, _app->windowHeight * 0.5f, 0.0f);
    else {
        ball->transform.position = InitVector3(rand() % _app->windowWidth, _app->windowHeight + (rand() % 400), 0.0f);
        ball->velocity = InitVector2(0.0f, -0.72f * ((rand() % 151) + 150));

        if(winner == false)
            ball->color = Find(&(_app->scene), "LeftPaddle")->color;
        else
            ball->color = Find(&(_app->scene), "RightPaddle")->color;
    }

    return ball;
}
