#include <Uefi.h>

STATIC CONST CHAR16 *ErrorMessages[] = {
    L"The operation completed successfully.",
    L"The image failed to load.",
    L"The parameter was incorrect.",
    L"The operation is not supported.",
    L"The buffer was not the proper size for the request.",
    L"The buffer was not large enough to hold the requested data. The required buffer size is returned in the appropriate parameter when this error occurs.",
    L"There is no data pending upon return.",
    L"The physical device reported an error while attempting the operation.",
    L"The device can not be written to.",
    L"The resource has run out.",
    L"An inconsistency was detected on the file system causing the operation to fail.",
    L"There is no more space on the file system.",
    L"The device does not contain any medium to perform the operation.",
    L"The medium in the device has changed since the last access.",
    L"The item was not found.",
    L"Access was denied.",
    L"The server was not found or did not respond to the request.",
    L"A mapping to the device does not exist.",
    L"A timeout time expired.",
    L"The protocol has not been started.",
    L"The protocol has already been started.",
    L"The operation was aborted.",
    L"An ICMP error occurred during the network operation.",
    L"A TFTP error occurred during the network operation.",
    L"A protocol error occurred during the network operation.",
    L"A function encountered an internal version that was incompatible with a version requested by the caller.",
    L"The function was not performed due to a security violation.",
    L"A CRC error was detected.",
    L"The beginning or end of media was reached.",
    L"Unknown error (29).",                                      // 29
    L"Unknown error (30).",                                      // 30
    L"The end of the file was reached.",
    L"The language specified was invalid.",
    L"The security status of the data is unknown or compromised and the data must be updated or replaced to restore a valid security status.",
    L"There is an address conflict address allocation.",
    L"A HTTP error occurred during the network operation.",
};

STATIC
CONST CHAR16*
StatusToString(EFI_STATUS Status)
{
    return ErrorMessages[Status & ~MAX_BIT];
}

STATIC
VOID
AppendChar(
    CHAR16 **Out,
    UINTN *Remaining,
    CHAR16 Ch
)
{
    if (*Remaining > 1) {
        **Out = Ch;
        (*Out)++;
        (*Remaining)--;
    }
}

STATIC
VOID
AppendString(
    CHAR16 **Out,
    UINTN *Remaining,
    CONST CHAR16 *Str
)
{
    while (*Str)
        AppendChar(Out, Remaining, *Str++);
}

STATIC
VOID
AppendUint(
    CHAR16 **Out,
    UINTN *Remaining,
    UINTN Value
)
{
    CHAR16 Buffer[32];
    UINTN Pos = 31;
    BOOLEAN Negative = FALSE;

    Buffer[31] = L'\0';

    if (Value < 0) {
        Negative = TRUE;
        Value = -Value;
    }

    do {
        Buffer[--Pos] = L'0' + (Value % 10);
        Value /= 10;
    } while (Value);

    if (Negative)
        Buffer[--Pos] = L'-';

    AppendString(
        Out,
        Remaining,
        &Buffer[Pos]
    );
}

STATIC
VOID
AppendHex(
    CHAR16 **Out,
    UINTN *Remaining,
    UINTN Value
)
{
    CHAR16 Buffer[2 * sizeof(UINTN) + 1];
    UINTN Pos = ARRAY_SIZE(Buffer) - 1;

    Buffer[Pos] = L'\0';

    do {
        UINTN Digit = Value & 0xF;

        Buffer[--Pos] =
            (Digit < 10)
                ? (L'0' + (CHAR16) Digit)
                : (L'a' + (CHAR16) Digit - 10);

        Value >>= 4;
    } while (Value);

    AppendString(
        Out,
        Remaining,
        &Buffer[Pos]
    );
}

UINTN
Format(
    CHAR16 *Buffer,
    UINTN BufferSize,
    CONST CHAR16 *Fmt,
    VA_LIST Args
)
{
    CHAR16 *Out = Buffer;
    UINTN Remaining = BufferSize;

    while (*Fmt) {

        if (*Fmt != L'%') {
            AppendChar(
                &Out,
                &Remaining,
                *Fmt++
            );
            continue;
        }

        Fmt++;

        switch (*Fmt) {

        case L'd':
        {
            UINTN Value = VA_ARG(Args, UINTN);

            AppendUint(
                &Out,
                &Remaining,
                Value
            );

            break;
        }

        case L's':
        {
            CONST CHAR16 *Str =
                VA_ARG(Args, CONST CHAR16 *);

            AppendString(
                &Out,
                &Remaining,
                Str ? Str : L"(null)"
            );

            break;
        }

        case L'r':
        {
            EFI_STATUS Status =
                VA_ARG(Args, EFI_STATUS);

            AppendString(
                &Out,
                &Remaining,
                StatusToString(Status)
            );

            break;
        }

        case L'x':
        {
            UINTN Value = VA_ARG(Args, UINTN);

            AppendHex(
                &Out,
                &Remaining,
                Value
            );

            break;
        }

        case L'%':
        {
            AppendChar(
                &Out,
                &Remaining,
                L'%'
            );

            break;
        }

        default:
        {
            AppendChar(
                &Out,
                &Remaining,
                L'%'
            );

            AppendChar(
                &Out,
                &Remaining,
                *Fmt
            );

            break;
        }
        }

        Fmt++;
    }

    if (BufferSize)
        *Out = L'\0';

    return (UINTN) (Out - Buffer);
}
