#ifndef _LOGGER_H
#define _LOGGER_H

#include <Uefi.h>

VOID
Printf(CONST CHAR16 *Fmt, ...);

VOID
Debugf(CONST CHAR16 *Fmt, ...);

#endif
