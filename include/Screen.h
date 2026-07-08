#ifndef _SCREEN_H
#define _SCREEN_H

#include <Uefi.h>
#include <Protocol/GraphicsOutput.h>

typedef struct {
    EFI_GRAPHICS_OUTPUT_PROTOCOL *Gop;
    UINT32 *Buffer;
} SCREEN;

extern SCREEN Screen;

VOID
ScreenFree(VOID);

EFI_STATUS
ScreenInit(VOID);

VOID
ScreenClear();

VOID
ScreenDrawRect(
    IN INT32 X,
    IN INT32 Y,
    IN INT32 W,
    IN INT32 H,
    IN UINT32 Color
);

VOID
ScreenDrawCircle(
    IN INT32 X,
    IN INT32 Y,
    IN INT32 Radius,
    IN UINT32 Color
);

VOID
ScreenFlush(VOID);

#endif
