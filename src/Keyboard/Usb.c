#include <Globals.h>
#include <Keyboard.h>
#include <Logger.h>

#define USB_BOOT_PROTOCOL 0

typedef struct {
    EFI_HANDLE Handle;

    EFI_USB_IO_PROTOCOL *UsbIo;
    EFI_USB_ENDPOINT_DESCRIPTOR Endpoint;
    EFI_USB_INTERFACE_DESCRIPTOR Interface;

    BOOLEAN IsControllerDisconnected;
    BOOLEAN IsCallbackRegistered;
} USB_KEYBOARD_INFO;

STATIC USB_KEYBOARD_INFO UsbKeyboardInfo;

STATIC
EFI_STATUS
SendUsbRequest(
    IN EFI_USB_IO_PROTOCOL *UsbIo,
    IN UINT8 RequestType,
    IN UINT16 Value,
    IN UINT16 Index
)
{
    UINT32 Result;
    EFI_STATUS Status;
    EFI_USB_DEVICE_REQUEST Request;

    Request.RequestType = USB_HID_CLASS_SET_REQ_TYPE;
    Request.Request = RequestType;
    Request.Value = Value;
    Request.Index = Index;
    Request.Length = 0;

    Status = UsbIo->UsbControlTransfer (
        UsbIo,
        &Request,
        EfiUsbNoData,
        0,
        NULL,
        0,
        &Result
    );

    return Status;
}

STATIC
EFI_STATUS
EFIAPI
Callback(
    VOID *Data,
    UINTN DataLength,
    VOID *Context,
    UINT32 Status
)
{
    if (Status != EFI_USB_NOERROR) {
        Printf(L"Status = %r != EFI_USB_NOERROR\r\n", Status);
        return EFI_SUCCESS;
    }

    if (DataLength < 8) {
        Printf(L"DataLength = %d < 8\r\n", DataLength);
        return EFI_SUCCESS;
    }

    UINT8 *Keys = Context;
    UINT8 *Report = Data;

    for (UINTN i = 0; i < KEY_COUNT; ++i)
        Keys[i] = 0;

    for (UINTN i = 2; i < 8; ++i) {
        switch (Report[i]) {
        case 0x29:
            Keys[KEY_ESC] = TRUE;
            break;

        case 0x04:
            Keys[KEY_A] = TRUE;
            break;

        case 0x1A:
            Keys[KEY_W] = TRUE;
            break;

        case 0x07:
            Keys[KEY_D] = TRUE;
            break;

        case 0x50:
            Keys[KEY_LEFT] = TRUE;
            break;

        case 0x52:
            Keys[KEY_UP] = TRUE;
            break;

        case 0x4F:
            Keys[KEY_RIGHT] = TRUE;
            break;
        }
    }

    return EFI_SUCCESS;
}

VOID
UsbKeyboardInfoFree(VOID)
{
    if (UsbKeyboardInfo.IsCallbackRegistered) {
        UsbKeyboardInfo.UsbIo->UsbAsyncInterruptTransfer(
            UsbKeyboardInfo.UsbIo,
            UsbKeyboardInfo.Endpoint.EndpointAddress,
            FALSE,
            0,
            0,
            NULL,
            NULL
        );
    }

    if (UsbKeyboardInfo.IsControllerDisconnected) {
        ST->BootServices->ConnectController(
            UsbKeyboardInfo.Handle,
            NULL,
            NULL,
            FALSE
        );
    }

    UsbKeyboardInfo.Handle = NULL;
    UsbKeyboardInfo.UsbIo = NULL;
    UsbKeyboardInfo.IsControllerDisconnected = FALSE;
    UsbKeyboardInfo.IsCallbackRegistered = FALSE;
}

EFI_STATUS
UsbKeyboardInit(KEYBOARD *Kb)
{
    EFI_STATUS Status;

    //
    // Locating all handles with USB_IO_PROTOCOL
    //
    EFI_GUID UsbIoGuid = EFI_USB_IO_PROTOCOL_GUID;
    EFI_HANDLE *Handles;
    UINTN Count;

    Status = ST->BootServices->LocateHandleBuffer(
        ByProtocol,
        &UsbIoGuid,
        NULL,
        &Count,
        &Handles
    );
    if (EFI_ERROR(Status)) {
        Printf(L"Error locating handles with USB_IO_PROTOCOL: %r\r\n", Status);
        return Status;
    }

    //
    // Looking for keyboard handle
    //
    BOOLEAN Found = FALSE;

    for (UINTN i = 0; i < Count; ++i) {
        EFI_USB_IO_PROTOCOL *UsbIo;
        EFI_USB_INTERFACE_DESCRIPTOR Interface;

        Status = ST->BootServices->OpenProtocol(
            Handles[i],
            &UsbIoGuid,
            (VOID**) &UsbIo,
            IH,
            NULL,
            EFI_OPEN_PROTOCOL_GET_PROTOCOL
        );
        if (EFI_ERROR(Status)) continue;

        Status = UsbIo->UsbGetInterfaceDescriptor(
            UsbIo,
            &Interface
        );
        if (EFI_ERROR(Status)) continue;

        if (
            Interface.InterfaceClass    == 3 &&
            Interface.InterfaceSubClass == 1 &&
            Interface.InterfaceProtocol == 1
        )
        {
            UsbKeyboardInfo.Handle = Handles[i];
            UsbKeyboardInfo.UsbIo = UsbIo;
            UsbKeyboardInfo.Interface = Interface;
            Found = TRUE;
            break;
        }
    }

    ST->BootServices->FreePool(Handles);

    if (!Found) {
        Printf(L"Keyboard handle was not found!\r\n");
        return EFI_NOT_FOUND;
    }

    //
    // Disconnecting controller
    //
    Status = ST->BootServices->DisconnectController(
        UsbKeyboardInfo.Handle,
        NULL,
        NULL
    );
    if (EFI_ERROR(Status)) {
        Printf(L"Error disconnecting controller: %r\r\n", Status);
        return Status;
    }

    UsbKeyboardInfo.IsControllerDisconnected = TRUE;

    //
    // Looking for IN endpoint
    //
    EFI_USB_ENDPOINT_DESCRIPTOR Endpoint;

    Found = FALSE;

    for (UINT8 i = 0; i < UsbKeyboardInfo.Interface.NumEndpoints; i++) {
        Status = UsbKeyboardInfo.UsbIo->UsbGetEndpointDescriptor(
            UsbKeyboardInfo.UsbIo,
            i,
            &Endpoint
        );
        if (EFI_ERROR(Status)) continue;

        if ((Endpoint.EndpointAddress & 0x80) != 0 &&
            (Endpoint.Attributes & 0x03) == 0x03) {
            Found = TRUE;
            break;
        }
    }

    if (!Found) {
        Printf(L"IN endpoint was not found!\r\n");
        Status = EFI_NOT_FOUND;
        goto cleanup;
    }

    UsbKeyboardInfo.Endpoint = Endpoint;

    //
    // Setting BOOT_PROTOCOL
    //
    Status = SendUsbRequest(
        UsbKeyboardInfo.UsbIo,
        EFI_USB_SET_PROTOCOL_REQUEST,
        USB_BOOT_PROTOCOL,
        UsbKeyboardInfo.Interface.InterfaceNumber
    );
    if (EFI_ERROR(Status)) {
        Printf(L"Error setting boot protocol: %r\r\n", Status);
        goto cleanup;
    }

    //
    // Setting idle
    //
    Status = SendUsbRequest(
        UsbKeyboardInfo.UsbIo,
        EFI_USB_SET_IDLE_REQUEST,
        (0 << 8) | 0,
        UsbKeyboardInfo.Interface.InterfaceNumber
    );
    if (EFI_ERROR(Status)) {
        Printf(L"Error setting idle: %r\r\n", Status);
        goto cleanup;
    }

    //
    // Keyboard callback registration
    //
    for (UINTN i = 0; i < KEY_COUNT; ++i)
        Kb->Keys[i] = 0;

    Status = UsbKeyboardInfo.UsbIo->UsbAsyncInterruptTransfer(
        UsbKeyboardInfo.UsbIo,
        UsbKeyboardInfo.Endpoint.EndpointAddress,
        TRUE,
        UsbKeyboardInfo.Endpoint.Interval,
        UsbKeyboardInfo.Endpoint.MaxPacketSize,
        Callback,
        Kb->Keys
    );
    if (EFI_ERROR(Status)) {
        Printf(L"Error registrating async callback: %r\r\n", Status);
        goto cleanup;
    }

    UsbKeyboardInfo.IsCallbackRegistered = TRUE;
    Kb->Type = KEYBOARD_TYPE_USB;

    return EFI_SUCCESS;

    cleanup:
    KeyboardFree(Kb);

    return Status;
}
