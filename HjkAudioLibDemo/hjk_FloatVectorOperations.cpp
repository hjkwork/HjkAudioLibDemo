#include "hjk_FloatVectorOperations.h"
#include "hjk_memory.h"
#include "hjk_MathsFunctions.h"


//==============================================================================
void JUCE_CALLTYPE FloatVectorOperations::clear(float* dest, int num) noexcept
{
#if JUCE_USE_VDSP_FRAMEWORK
    vDSP_vclr(dest, 1, (size_t)num);
#else
    zeromem(dest, (size_t)num * sizeof(float));
#endif
}

void JUCE_CALLTYPE FloatVectorOperations::clear(double* dest, int num) noexcept
{
#if JUCE_USE_VDSP_FRAMEWORK
    vDSP_vclrD(dest, 1, (size_t)num);
#else
    zeromem(dest, (size_t)num * sizeof(double));
#endif
}



void JUCE_CALLTYPE FloatVectorOperations::copy(float* dest, const float* src, int num) noexcept
{
    memcpy(dest, src, (size_t)num * sizeof(float));
}

void JUCE_CALLTYPE FloatVectorOperations::copy(double* dest, const double* src, int num) noexcept
{
    memcpy(dest, src, (size_t)num * sizeof(double));
}
