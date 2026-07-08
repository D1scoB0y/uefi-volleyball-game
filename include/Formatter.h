#ifndef _FORMATTER_H
#define _FORMATTER_H

#include <Uefi.h>

UINTN
Format(
    CHAR16 *Buffer,
    UINTN BufferSize,
    CONST CHAR16 *Fmt,
    VA_LIST Args
);

#endif
