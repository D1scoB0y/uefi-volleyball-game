#include <Game.h>
#include <Screen.h>
#include <Fp32.h>
#include <Logger.h>
#include <Tsc.h>

#define SCORE_PANEL_X_OFFSET        FP32_FROM_CONST(32)
#define SCORE_PANEL_Y_OFFSET        FP32_FROM_CONST(48)
#define SCORE_PANEL_BETWEEN_OFFSET  FP32_FROM_CONST(12)
#define SCORE_POINT_SIZE            FP32_FROM_CONST(24)

// #define MEASUREMENTS_COUNT  2048

// STATIC UINT64 DrawCircleTicks = 0;
// STATIC UINTN MeasurementsCount = 0;

STATIC FP32 ScreenScaleX, ScreenScaleY;

STATIC
inline
INT32
MapToScreenX(INT32 X)
{
    return Fp32ToInt32Ceil(Fp32Mul(X, ScreenScaleX));
}

STATIC
inline
INT32
MapToScreenY(INT32 Y)
{
    return Fp32ToInt32Ceil(Fp32Mul(Y, ScreenScaleY));
}

VOID
RenderInit(FP32 MapW, FP32 MapH)
{
    ScreenScaleX = Fp32Div(
        Fp32FromInt32((INT32) Screen.Gop->Mode->Info->PixelsPerScanLine),
        MapW
    );
    ScreenScaleY = Fp32Div(
        Fp32FromInt32((INT32) Screen.Gop->Mode->Info->VerticalResolution),
        MapH
    );
}

VOID
Render(GAME *G)
{
    ScreenClear();

    //
    // Draw ground
    //
    ScreenDrawRect(
        0,
        MapToScreenY(G->Map.GroundY),
        (INT32) Screen.Gop->Mode->Info->PixelsPerScanLine,
        MapToScreenY(G->Map.H - G->Map.GroundY),
        G->Map.GroundColor
    );

    //
    // Draw net
    //
    ScreenDrawRect(
        MapToScreenX(G->Map.NetX),
        MapToScreenY(G->Map.NetY),
        MapToScreenX(G->Map.NetW),
        MapToScreenY(G->Map.NetH),
        G->Map.NetColor
    );

    //
    // Draw players
    //
    ScreenDrawRect(
        MapToScreenX(G->P[0].Body.X),
        MapToScreenY(G->P[0].Body.Y),
        MapToScreenX(G->P[0].Body.W),
        MapToScreenY(G->P[0].Body.H),
        G->P[0].Body.Color
    );
    ScreenDrawRect(
        MapToScreenX(G->P[1].Body.X),
        MapToScreenY(G->P[1].Body.Y),
        MapToScreenX(G->P[1].Body.W),
        MapToScreenY(G->P[1].Body.H),
        G->P[1].Body.Color
    );

    // UINT64 DrawCircleTicksStart = Tsc();

    //
    // Draw ball
    //
    ScreenDrawCircle(
        MapToScreenX(G->Ball.X + G->Ball.W / 2),
        MapToScreenY(G->Ball.Y + G->Ball.H / 2),
        MapToScreenX(G->Ball.W / 2),
        G->Ball.Color
    );

    // DrawCircleTicks += Tsc() - DrawCircleTicksStart;

    // MeasurementsCount++;

    // if (MeasurementsCount == MEASUREMENTS_COUNT) {
    //     UINT64 DrawCircleAvgUs = ((DrawCircleTicks / MEASUREMENTS_COUNT) * 1000000ULL) / TscFreq;
    //     Debugf(L"DrawCircle: %d us\r\n", DrawCircleAvgUs);

    //     DrawCircleTicks = 0;
    //     MeasurementsCount = 0;
    // }

    //
    // Draw score
    //
    for (UINTN i = 0; i < G->LeftScore; ++i) {
        ScreenDrawRect(
            MapToScreenX(SCORE_PANEL_X_OFFSET + (SCORE_POINT_SIZE + SCORE_PANEL_BETWEEN_OFFSET) * (INT32) i),
            MapToScreenY(SCORE_PANEL_Y_OFFSET),
            MapToScreenX(SCORE_POINT_SIZE),
            MapToScreenY(SCORE_POINT_SIZE),
            G->P[0].Body.Color
        );
    }
    for (UINTN i = 0; i < G->RightScore; ++i) {
        ScreenDrawRect(
            MapToScreenX(G->Map.W - SCORE_POINT_SIZE - (SCORE_PANEL_X_OFFSET + (SCORE_POINT_SIZE + SCORE_PANEL_BETWEEN_OFFSET) * (INT32) i)),
            MapToScreenY(SCORE_PANEL_Y_OFFSET),
            MapToScreenX(SCORE_POINT_SIZE),
            MapToScreenY(SCORE_POINT_SIZE),
            G->P[1].Body.Color
        );
    }

    ScreenFlush();
}
