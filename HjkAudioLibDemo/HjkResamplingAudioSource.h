#pragma once
#include <cmath>
#include "HjkAudioSource.h"
#include "OptionalScopedPointer.h"
#include "HjkDefs.h"
#include "hjk_memory.h"
#include "hjk_SpinLock.h"
#include "hjk_MathsFunctions.h"
#include "hjk_ CriticalSection.h"
#include "hjk_HeapBlock.h"
class HjkResamplingAudioSource :
    public HjkAudioSource
{
public:
    //为给定的输入源创建HjkResamplingAudioSource
    HjkResamplingAudioSource(HjkAudioSource* inputSource,
        bool deleteInputWhenDelered,
        int numChannels = 2);

    ~HjkResamplingAudioSource() override;
    /** Changes the resampling ratio.

       (This value can be changed at any time, even while the source is running).

       @param samplesInPerOutputSample     if set to 1.0, the input is passed through; higher
                                           values will speed it up; lower values will slow it
                                           down. The ratio must be greater than 0
   */
    void setResamplingRatio(double samplesInPerOutputSample);

    /** Returns the current resampling ratio.

        This is the value that was set by setResamplingRatio().
    */
    double getResamplingRatio() const noexcept { return ratio; }

    /** Clears any buffers and filters that the resampler is using. */
    void flushBuffers();

    //==============================================================================
    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
    void releaseResources() override;
    void getNextAudioBlock(const HjkAudioSourceChannelInfo&) override;

private:
    //==============================================================================
    OptionalScopedPointer<HjkAudioSource> input;
    double ratio = 1.0, lastRatio = 1.0;
    HjkAudioBuffer<float> buffer;
    int bufferPos = 0, sampsInBuffer = 0;
    double subSampleOffset = 0.0;
    double coefficients[6];
    SpinLock ratioLock;
    CriticalSection callbackLock;
    const int numChannels;
    HeapBlock<float*> destBuffers;
    HeapBlock<const float*> srcBuffers;

    void setFilterCoefficients(double c1, double c2, double c3, double c4, double c5, double c6);
    void createLowPass(double proportionalRate);

    struct FilterState
    {
        double x1, x2, y1, y2;
    };

    HeapBlock<FilterState> filterStates;
    void resetFilters();

    void applyFilter(float* samples, int num, FilterState& fs);

  //  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ResamplingAudioSource)
};
