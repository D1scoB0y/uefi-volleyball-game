#ifndef _GAME_H
#define _GAME_H

#include <Fp32.h>
#include <Keyboard.h>

typedef struct {
    FP32 X, Y, W, H, VelX, VelY;
    UINT32 Color;
} BODY;

typedef struct {
    BODY Body;
    BOOLEAN OnGround, Left, Up, Right;
} PLAYER;

typedef struct {
    FP32 H, W, GroundY, NetY, NetX, NetW, NetH;
    UINT32 GroundColor, NetColor;
} MAP;

typedef struct {
    MAP Map;
    BODY Ball;
    PLAYER P[2];
    UINT8 LeftScore, RightScore;
    BOOLEAN PassRight;
} GAME;

VOID
GameRun(BOOLEAN *Keys);

#endif
