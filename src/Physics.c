#include <Game.h>
#include <Fp32.h>

#define MAP_GRAVITY             FP32_ONE
#define MAP_GROUND_FRICTION     FP32_ONE

#define PLAYER_X_ACCEL          FP32_FROM_CONST(3)
#define PLAYER_MAX_X_VEL        FP32_FROM_CONST(12)
#define PLAYER_JUMP_IMPULSE     -FP32_FROM_CONST(20)

#define BALL_RESTITUTION        (FP32_ONE - FP32_ONE / 4)

STATIC
VOID
UpdatePlayer(PLAYER *P)
{
    if (P->Left && !P->Right) {
        INT32 NewVelX = P->Body.VelX - PLAYER_X_ACCEL;
        P->Body.VelX = NewVelX < -PLAYER_MAX_X_VEL ? -PLAYER_MAX_X_VEL : NewVelX;
    }

    if (P->Right && !P->Left) {
        INT32 NewVelX = P->Body.VelX + PLAYER_X_ACCEL;
        P->Body.VelX = NewVelX > PLAYER_MAX_X_VEL ? PLAYER_MAX_X_VEL : NewVelX;
    }

    if (!P->Left && !P->Right && P->OnGround) {
        if (P->Body.VelX < -MAP_GROUND_FRICTION) {
            P->Body.VelX += MAP_GROUND_FRICTION;
        } else if (P->Body.VelX > MAP_GROUND_FRICTION) {
            P->Body.VelX -= MAP_GROUND_FRICTION;
        } else {
            P->Body.VelX = 0;
        }
    }

    P->Body.VelY += MAP_GRAVITY;

    if (P->Up && P->OnGround)
        P->Body.VelY = PLAYER_JUMP_IMPULSE;

    P->Body.X += P->Body.VelX;
    P->Body.Y += P->Body.VelY;
}

STATIC
VOID
ResolvePlayerMap(PLAYER *P, MAP *M)
{
    //
    // Resolve Y
    //
    if (P->Body.Y + P->Body.H >= M->GroundY) {
        P->Body.Y = M->GroundY - P->Body.H;
        P->Body.VelY = 0;
        P->OnGround = TRUE;
    } else {
        P->OnGround = FALSE;
    }

    //
    // Resolve X
    //
    if (P->Body.X <= 0) {
        P->Body.X = 0;
        P->Body.VelX = 0;
    } else if (P->Body.X + P->Body.W >= M->W) {
        P->Body.X = M->W - P->Body.W;
        P->Body.VelX = 0;
    }

    //
    // Resolve net
    //
    if (
        !(P->Body.X < M->NetX + M->NetW &&
          P->Body.X + P->Body.W > M->NetX &&
          P->Body.Y < M->NetY + M->NetH &&
          P->Body.Y + P->Body.H > M->NetY)
    ) return;

    INT32 PlayerCenterX = P->Body.X + P->Body.W / 2;
    INT32 NetCenterX = M->NetX + M->NetW / 2;

    P->Body.VelX = 0;

    if (PlayerCenterX >= NetCenterX)
        P->Body.X = M->NetX + M->NetW;
    else
        P->Body.X = M->NetX - P->Body.W;
}

STATIC
VOID
UpdateBall(BODY *B)
{
    B->VelY += MAP_GRAVITY;

    B->X += B->VelX;
    B->Y += B->VelY;
}

STATIC
VOID
ResolveBallMap(BODY *B, MAP *M)
{
    //
    // Resolve Y
    //
    if (B->Y <= 0) {
        B->Y = 0;
        B->VelY = Fp32Mul(-B->VelY, BALL_RESTITUTION);
    } else if (B->Y + B->H >= M->GroundY) {
        B->Y = M->GroundY - B->H;
        B->VelY = Fp32Mul(-B->VelY, BALL_RESTITUTION);
    }

    //
    // Resolve X
    //
    if (B->X <= 0) {
        B->X = 0;
        B->VelX = Fp32Mul(-B->VelX, BALL_RESTITUTION);
    } else if (B->X + B->W >= M->W) {
        B->X = M->W - B->W;
        B->VelX = Fp32Mul(-B->VelX, BALL_RESTITUTION);
    }

    //
    // Resolve net
    //
    if (
        !(B->X < M->NetX + M->NetW &&
          B->X + B->W > M->NetX &&
          B->Y < M->NetY + M->NetH &&
          B->Y + B->H > M->NetY)
    ) return;

    INT32 BallCenterX = B->X + B->W / 2;
    INT32 BallCenterY = B->Y + B->H / 2;

    INT32 NetCenterX = M->NetX + M->NetW / 2;
    INT32 NetCenterY = M->NetY + M->NetH / 2;

    INT32 Dx = BallCenterX - NetCenterX;
    INT32 Dy = BallCenterY - NetCenterY;

    INT32 OverlapX = (B->W + M->NetW) / 2 - ABS(Dx);
    INT32 OverlapY = (B->H + M->NetH) / 2 - ABS(Dy);

    if (OverlapX < OverlapY) {
        if (Dx > 0)
            B->X += OverlapX;
        else
            B->X -= OverlapX;

        B->VelX = Fp32Mul(-B->VelX, BALL_RESTITUTION);
    } else {
        B->Y -= OverlapY;
        B->VelY = Fp32Mul(-B->VelY, BALL_RESTITUTION);
    }
}

STATIC
VOID
ResolveBallPlayer(BODY *B, PLAYER *P)
{
    if (
        !(P->Body.X < B->X + B->W &&
          P->Body.X + P->Body.W > B->X &&
          P->Body.Y < B->Y + B->H &&
          P->Body.Y + P->Body.H > B->Y)
    ) return;

    INT32 PlayerCenterX = P->Body.X + P->Body.W / 2;
    INT32 PlayerCenterY = P->Body.Y + P->Body.H / 2;

    INT32 BallCenterX = B->X + B->W / 2;
    INT32 BallCenterY = B->Y + B->H / 2;

    INT32 Dx = BallCenterX - PlayerCenterX;
    INT32 Dy = BallCenterY - PlayerCenterY;

    INT32 OverlapX = (P->Body.W + B->W) / 2 - ABS(Dx);
    INT32 OverlapY = (P->Body.H + B->H) / 2 - ABS(Dy);

    if (OverlapX < OverlapY) {
        if (Dx > 0) {
            B->X += OverlapX;
            B->VelX = Fp32Mul(ABS(B->VelX), BALL_RESTITUTION);
        } else {
            B->X -= OverlapX;
            B->VelX = Fp32Mul(-ABS(B->VelX), BALL_RESTITUTION);
        }

        B->VelX += P->Body.VelX / 2;
    } else {
        if (Dy < 0) {
            B->Y -= OverlapY;

            if (B->VelY > 0)
                B->VelY = Fp32Mul(-B->VelY, BALL_RESTITUTION);
            else
                B->VelY += Fp32FromInt32(4);

            B->VelY += P->Body.VelY / 2;
            B->VelX += P->Body.VelX;
        } else {
            B->Y += OverlapY;
            B->VelY = ABS(B->VelY);
            B->VelY += Fp32FromInt32(4);
        }
    }
}

VOID
PhysicsStep(GAME *G)
{
    UpdatePlayer(&G->P[0]);
    ResolvePlayerMap(&G->P[0], &G->Map);

    UpdatePlayer(&G->P[1]);
    ResolvePlayerMap(&G->P[1], &G->Map);

    UpdateBall(&G->Ball);
    ResolveBallMap(&G->Ball, &G->Map);

    ResolveBallPlayer(&G->Ball, &G->P[0]);
    ResolveBallPlayer(&G->Ball, &G->P[1]);
}
