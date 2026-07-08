#ifndef _FP32_H
#define _FP32_H

#include <Uefi.h>

#define FP32_SHIFT          16
#define FP32_ONE            (1 << FP32_SHIFT)

#define FP32_FROM_CONST(X)  ((FP32) ((X) << FP32_SHIFT))

typedef INT32 FP32;
typedef INT64 FP64;

STATIC
inline
FP32
Fp32FromInt32(INT32 V)
{
    return FP32_FROM_CONST(V);
}

STATIC
inline
FP32
Fp32Mul(FP32 A, FP32 B)
{
    return (FP32) (((FP64) A * B) >> FP32_SHIFT);
}

STATIC
inline
FP32
Fp32Div(FP32 A, FP32 B)
{
    return (FP32) (((FP64) A << FP32_SHIFT) / B);
}

STATIC
inline
INT32
Fp32ToInt32Ceil(FP32 V)
{
    return (INT32) (((FP64) V + (0b00000001 << (FP32_SHIFT - 1))) >> FP32_SHIFT);
}

#endif
