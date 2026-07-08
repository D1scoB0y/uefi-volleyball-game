#include <Globals.h>
#include <Render.h>
#include <Game.h>
#include <Fp32.h>
#include <Tsc.h>
#include <Keyboard.h>
#include <Physics.h>
#include <Logger.h>

#define MAP_W               1280
#define MAP_H               960

#define BACKGROUND_COLOR    0x00EEEEEE

#define GROUND_H            120
#define GROUND_Y            (MAP_H - GROUND_H)
#define GROUND_COLOR        0x00444444

#define NET_W               40
#define NET_H               200
#define NET_X               (MAP_W - NET_W) / 2
#define NET_Y               (MAP_H - GROUND_H - NET_H)
#define NET_COLOR           0x00000000

#define PLAYER_W            90
#define PLAYER_H            130
#define PLAYER1_COLOR       0x00FF0000
#define PLAYER2_COLOR       0x000000FF

#define BALL_SIZE           52
#define BALL_COLOR          0x00999999

#define PHYSICS_DT_US       20000 // 20 ms

// #define MEASUREMENTS_COUNT  2048

// STATIC UINT64 FrameTicks = 0, PhysicsTicks = 0, RenderTicks = 0;
// STATIC UINTN MeasurementsCount = 0;

STATIC
VOID
GameInit(GAME *G, BOOLEAN PassRight, UINT8 LeftScore, UINT8 RightScore)
{
    MAP M = {
        .W = Fp32FromInt32(MAP_W),
        .H = Fp32FromInt32(MAP_H),

        .GroundY = Fp32FromInt32(GROUND_Y),
        .GroundColor = GROUND_COLOR,

        .NetX = Fp32FromInt32(NET_X),
        .NetY = Fp32FromInt32(NET_Y),
        .NetW = Fp32FromInt32(NET_W),
        .NetH = Fp32FromInt32(NET_H),
        .NetColor = NET_COLOR,
    };

    PLAYER P1 = {
        .Body = {
            .X = Fp32FromInt32(250),
            .Y = Fp32FromInt32(GROUND_Y - PLAYER_H),
            .W = Fp32FromInt32(PLAYER_W),
            .H = Fp32FromInt32(PLAYER_H),
            .VelX = 0,
            .VelY = 0,
            .Color = PLAYER1_COLOR,
        },
        .OnGround = FALSE,
    };

    PLAYER P2 = {
        .Body = {
            .X = Fp32FromInt32(MAP_W - PLAYER_W - 250),
            .Y = Fp32FromInt32(GROUND_Y - PLAYER_H),
            .W = Fp32FromInt32(PLAYER_W),
            .H = Fp32FromInt32(PLAYER_H),
            .VelX = 0,
            .VelY = 0,
            .Color = PLAYER2_COLOR,
        },
        .OnGround = FALSE,
    };

    BODY B = {
        .X = Fp32FromInt32((MAP_W - BALL_SIZE) / 2),
        .Y = Fp32FromInt32(150),
        .W = Fp32FromInt32(BALL_SIZE),
        .H = Fp32FromInt32(BALL_SIZE),
        .VelX = Fp32FromInt32(5 * (PassRight ? 1 : -1)),
        .VelY = Fp32FromInt32(-12),
        .Color = BALL_COLOR,
    };

    G->Map = M;
    G->P[0] = P1;
    G->P[1] = P2;
    G->Ball = B;
    G->PassRight = PassRight;
    G->LeftScore = LeftScore;
    G->RightScore = RightScore;
}

STATIC
VOID
ProcessGoal(GAME *G)
{
    UINT8 LeftScore = G->LeftScore, RightScore = G->RightScore;
    BOOLEAN PassRight;

    if (G->Ball.X + G->Ball.W / 2 < Fp32FromInt32(MAP_W / 2)) {
        RightScore++;
        PassRight = TRUE;
    } else {
        LeftScore++;
        PassRight = FALSE;
    }

    if (LeftScore > 10 || RightScore > 10) {
        LeftScore = 0;
        RightScore = 0;
        PassRight = FALSE;
    }

    GameInit(G, PassRight, LeftScore, RightScore);
}

STATIC
VOID
ReadPlayersInput(BOOLEAN *Keys, PLAYER *P1, PLAYER *P2)
{
    P1->Left  = Keys[KEY_A];
    P1->Right = Keys[KEY_D];
    P1->Up    = Keys[KEY_W];

    P2->Left  = Keys[KEY_LEFT];
    P2->Right = Keys[KEY_RIGHT];
    P2->Up    = Keys[KEY_UP];
}

VOID
GameRun(BOOLEAN *Keys)
{
    GAME G;
    GameInit(&G, FALSE, 0, 0);

    RenderInit(G.Map.W, G.Map.H);

    UINT64 Accumulator = 0;
    UINT64 Prev = Tsc();

    while (1) {
        // UINT64 FrameTicksStart = Tsc();

        if (Keys[KEY_ESC]) {
            break;
        }

        ReadPlayersInput(Keys, &G.P[0], &G.P[1]);

        // UINT64 PhysicsTicksStart = Tsc();

        UINT64 Now = Tsc();

        UINT64 DeltaUs = ((Now - Prev) * 1000000ULL) / TscFreq;
        Prev = Now;

        Accumulator += DeltaUs;

        while (Accumulator >= PHYSICS_DT_US) {
            Accumulator -= PHYSICS_DT_US;

            PhysicsStep(&G);

            if (G.Ball.Y + G.Ball.H >= G.Map.GroundY) {
                ProcessGoal(&G);
                Accumulator = 0;
                break;
            }
        }

        // PhysicsTicks += Tsc() - PhysicsTicksStart;

        // UINT64 RenderTicksStart = Tsc();

        Render(&G);

        // RenderTicks += Tsc() - RenderTicksStart;

        // FrameTicks += Tsc() - FrameTicksStart;

        // MeasurementsCount++;

        // if (MeasurementsCount == MEASUREMENTS_COUNT) {
            // UINT64 FrameAvgUs = ((FrameTicks / MEASUREMENTS_COUNT) * 1000000ULL) / TscFreq;
            // UINT64 PhysicsAvgUs = ((PhysicsTicks / MEASUREMENTS_COUNT) * 1000000ULL) / TscFreq;
            // UINT64 RenderAvgUs = ((RenderTicks / MEASUREMENTS_COUNT) * 1000000ULL) / TscFreq;

            // Debugf(L"Frame: %d us\r\n", FrameAvgUs);
            // Debugf(L"Physics: %d us\r\n", PhysicsAvgUs);
            // Debugf(L"Render: %d us\r\n", RenderAvgUs);

            // FrameTicks = 0;
            // PhysicsTicks = 0;
            // RenderTicks = 0;
            // MeasurementsCount = 0;
        // }
    }
}
