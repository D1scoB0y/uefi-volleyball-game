#include <Formatter.h>
#include <Console.h>
#include <Serial.h>

STATIC
UINTN
Utf16ToAscii(
    CONST CHAR16 *Src,
    CHAR8 *Dst,
    UINTN DstSize
)
{
    UINTN i = 0;

    while (
        Src[i] &&
        i < DstSize - 1
    ) {
        CHAR16 Ch = Src[i];

        if (Ch <= 0x7F)
            Dst[i] = (CHAR8)Ch;
        else
            Dst[i] = '?';

        i++;
    }

    Dst[i] = '\0';

    return i+1;
}

__attribute__((ms_abi))
VOID
Printf(CONST CHAR16 *Fmt, ...)
{
    VA_LIST Args;
    VA_START(Args, Fmt);

    CHAR16 Buffer[512];
    Format(Buffer, 512, Fmt, Args);

    ConsoleWrite(Buffer);

    VA_END(Args);
}

__attribute__((ms_abi))
VOID
Debugf(CONST CHAR16 *Fmt, ...)
{
    VA_LIST Args;
    VA_START(Args, Fmt);

    CHAR16 Buffer[512];
    Format(Buffer, 512, Fmt, Args);

    CHAR8 AsciiBuffer[512];

    UINTN Len = Utf16ToAscii(
        Buffer,
        AsciiBuffer,
        sizeof(Buffer)
    );

    SerialWrite(AsciiBuffer, Len);

    VA_END(Args);
}
