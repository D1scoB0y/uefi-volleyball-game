#include <Globals.h>
#include <Logger.h>
#include <Serial.h>
#include <Keyboard.h>
#include <Screen.h>
#include <Game.h>
#include <Tsc.h>

#define START_COUNTDOWN 5
#define END_COUNTDOWN   5

EFI_STATUS
StartCountdown(UINTN Secs, CHAR16 *Fmt)
{
    EFI_EVENT TimerEvent;
    EFI_STATUS Status = ST->BootServices->CreateEvent(
        EVT_TIMER,
        TPL_CALLBACK,
        NULL,
        NULL,
        &TimerEvent
    );
    if (EFI_ERROR(Status)) {
        Printf(L"Error creating timer event: %r\r\n", Status);
        return Status;
    }

    ST->BootServices->SetTimer(
        TimerEvent,
        TimerPeriodic,
        10000000 // 1 second
    );

    for (UINTN i = Secs; i > 0; --i) {
        Printf(Fmt, i);
        ST->BootServices->WaitForEvent(
            1,
            &TimerEvent,
            NULL
        );
    }

    ST->BootServices->CloseEvent(TimerEvent);

    return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
efi_main(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable)
{
    GlobalsSet(ImageHandle, SystemTable);
    ST->ConOut->ClearScreen(ST->ConOut);

    EFI_STATUS Status;

    Printf(L"Start initializing process...\r\n");

    TscInit();

    Status = SerialInit();
    if (EFI_ERROR(Status)) {
        Printf(L"Error initializing serial!\r\n");
    }

    Status = ScreenInit();
    if (EFI_ERROR(Status)) {
        Printf(L"Screen initialization finished with errors!\r\n");
        goto exit;
    }

    KEYBOARD Kb;
    Status = KeyboardInit(&Kb);
    if (EFI_ERROR(Status)) {
        Printf(L"Keyboard initialization finished with error: %r\r\n", Status);
        goto exit;
    }

    Printf(L"All devices was initialized!\r\n");

    Status = StartCountdown(START_COUNTDOWN, L"Game will started in %d\r\n");
    if (EFI_ERROR(Status)) {
        Printf(L"Error starting run countdown!\r\n");
        goto exit;
    }

    GameRun(Kb.Keys);

    ST->ConOut->ClearScreen(ST->ConOut);

    exit:
    ScreenFree();
    KeyboardFree(&Kb);
    StartCountdown(END_COUNTDOWN, L"App will closed in %d\r\n");

    return EFI_SUCCESS;
}
