#pragma once
#include "HjkAudioSource.h"
#include "hjk_ CriticalSection.h"
class MutilChsAudioTransportSource :
    public HjkAudioSource
{
public:
    MutilChsAudioTransportSource();
    ~MutilChsAudioTransportSource();
    void setSource(
    //    PositionableAudioSource* newSource,
        int readAheadBufferSize = 0,
    //    TimeSliceThread* readAheadThread = nullptr,
        double sourceSampleRateToCorrectFor = 0.0,
        int maxNumChannels = 2);
    void prepareToPlay(int samplesPerBlockExpected,double sampleRate) override;
    void releaseResources()override;
    void getNextAudioBlock(const HjkAudioSourceChannelInfo&)override;
private:
    HjkAudioSource* masterSource = nullptr;
    CriticalSection callbackLock;
    float gain = 1.0f, lastGain = 1.0f;
    std::atomic<bool> playing{ false }, stopped{ true };
    double sampleRate = 44100.0, sourceSampleRate = 0;
    int blockSize = 128, readAheadBufferSize = 0;
    bool isPrepared = false, inputStreamEOF = false;

    void releaseMasterResources();
};

