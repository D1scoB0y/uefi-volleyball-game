#include <Globals.h>

#define TSC_FREQ_MEASUREMENT_STALL_US 100000
#define TSC_FREQ_MEASUREMENTS_COUNT 10

UINT64 TscFreq; // hz

UINT64
Tsc()
{
    UINT32 Lo;
    UINT32 Hi;

    __asm__ volatile (
        "rdtsc"
        : "=a"(Lo), "=d"(Hi)
    );

    return ((UINT64) Hi << 32) | Lo;
}

VOID
TscInit()
{
    UINT64 Sum = 0;

    for (UINTN i = 0; i < TSC_FREQ_MEASUREMENTS_COUNT; ++i) {
        UINT64 Start = Tsc();

        ST->BootServices->Stall(TSC_FREQ_MEASUREMENT_STALL_US);

        UINT64 End = Tsc();

        Sum += (End - Start);
    }

    TscFreq = (Sum / TSC_FREQ_MEASUREMENTS_COUNT) * (1000000 / TSC_FREQ_MEASUREMENT_STALL_US);
}
