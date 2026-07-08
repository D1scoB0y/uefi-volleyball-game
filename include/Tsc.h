#ifndef _TSC_H
#define _TSC_H

#include <Uefi.h>

extern UINT64 TscFreq; // hz

UINT64
Tsc();

VOID
TscInit();

#endif
