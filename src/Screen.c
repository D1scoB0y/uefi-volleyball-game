#include <Globals.h>
#include <Logger.h>
#include <Screen.h>

#define SCREEN_CLEAR_BYTE 0xEE

SCREEN Screen;

VOID
ScreenFree(VOID)
{
    ST->BootServices->FreePool(Screen.Buffer);

    Screen.Buffer = NULL;
    Screen.Gop = NULL;
}

EFI_STATUS
ScreenInit(VOID) {
    EFI_GUID GopGuid = EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID;
    EFI_GRAPHICS_OUTPUT_PROTOCOL *Gop;

    EFI_STATUS Status = ST->BootServices->LocateProtocol(
        &GopGuid,
        NULL,
        (VOID**) &Gop
    );
    if (EFI_ERROR(Status)) {
        Printf(L"Error locating GOP!\r\n");
        return Status;
    }

    Printf(
        L"Sreen resolution is %dx%d\r\n",
        Gop->Mode->Info->HorizontalResolution,
        Gop->Mode->Info->VerticalResolution
    );

    UINT32 *Buffer;
    Status = ST->BootServices->AllocatePool(
        EfiLoaderData,
        Gop->Mode->FrameBufferSize,
        (VOID**) &Buffer
    );
    if (EFI_ERROR(Status)) {
        Printf(L"Error allocating buffer!\r\n");
        return Status;
    }

    Screen.Gop = Gop;
    Screen.Buffer = Buffer;

    return EFI_SUCCESS;
}

VOID
ScreenClear(VOID)
{
    ST->BootServices->SetMem(
        Screen.Buffer,
        Screen.Gop->Mode->FrameBufferSize,
        SCREEN_CLEAR_BYTE
    );
}

VOID
ScreenDrawRect(
    IN INT32 X,
    IN INT32 Y,
    IN INT32 W,
    IN INT32 H,
    IN UINT32 Color
)
{
    if (
        X + W > (INT32) Screen.Gop->Mode->Info->HorizontalResolution ||
        Y + H > (INT32) Screen.Gop->Mode->Info->VerticalResolution
    ) return;

    UINT32 Stride = Screen.Gop->Mode->Info->PixelsPerScanLine;

    for (UINTN i = 0; i < (UINTN) H; ++i) {
        for (UINTN j = 0; j < (UINTN) W; ++j) {
            Screen.Buffer
                [((UINTN) Y + i) * Stride + (UINTN) X + j] =
                Color;
        }
    }
}

static inline VOID
DrawSpan(
    INT32 Y,
    INT32 X0,
    INT32 X1,
    UINT32 Color
)
{
    const INT32 Width = (INT32)Screen.Gop->Mode->Info->HorizontalResolution;
    const INT32 Height = (INT32)Screen.Gop->Mode->Info->VerticalResolution;

    if ((UINT32)Y >= (UINT32)Height)
        return;

    if (X0 < 0)
        X0 = 0;

    if (X1 >= Width)
        X1 = Width - 1;

    if (X0 > X1)
        return;

    UINT32 *Dst = Screen.Buffer +
                  (UINTN)Y * Screen.Gop->Mode->Info->PixelsPerScanLine +
                  X0;

    for (INT32 X = X0; X <= X1; ++X)
        *Dst++ = Color;
}

VOID
ScreenDrawCircle(
    IN INT32 X, // center
    IN INT32 Y, // center
    IN INT32 Radius,
    IN UINT32 Color
)
{
    if (Radius <= 0)
            return;

        INT32 Dx = Radius;
        INT32 Dy = 0;
        INT32 Err = 1 - Radius;

        while (Dx >= Dy)
        {
            DrawSpan(Y + Dy, X - Dx, X + Dx, Color);
            DrawSpan(Y - Dy, X - Dx, X + Dx, Color);

            if (Dx != Dy)
            {
                DrawSpan(Y + Dx, X - Dy, X + Dy, Color);
                DrawSpan(Y - Dx, X - Dy, X + Dy, Color);
            }

            ++Dy;

            if (Err < 0)
            {
                Err += (Dy << 1) + 1;
            }
            else
            {
                --Dx;
                Err += ((Dy - Dx) << 1) + 1;
            }
        }
}

VOID
ScreenFlush(VOID)
{
    Screen.Gop->Blt(
        Screen.Gop,
        (VOID*) Screen.Buffer,
        EfiBltBufferToVideo,
        0,
        0,
        0,
        0,
        Screen.Gop->Mode->Info->PixelsPerScanLine,
        Screen.Gop->Mode->Info->VerticalResolution,
        0
    );
}
