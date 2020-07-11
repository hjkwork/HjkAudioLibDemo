// HjkAudioLibDemo.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//
#pragma once
#include <iostream>
#include <process.h>
#include "HjkAudioSource.h"
#include "HjkAudioBuffer.h"
#include "hjk_MathsFunctions.h"
#include "hjk_ CriticalSection.h"
int currentBufferSizeSamples = 0;
double currentSampleRate = 0;
int chsIn = 0, chsOut = 0;
bool stop = true;
bool isOpen_ = false, isStarted = false;
CriticalSection startStopLock;
CriticalSection audioCallbackLock, midiCallbackLock;
CriticalSection readLock;
HjkAudioBuffer<float> tempBuffer;
HjkAudioSource* source = nullptr;
double sampleRate = 0;
int bufferSize = 0;
float* channels[128];
float* outputChans[128];
const float* inputChans[128];
//注意初始化中的参数，可以由外部给出
void initSource();
void run(void*);
/*
void start(AudioIODeviceCallback* call) 
{
    if (isOpen_ && call != nullptr && !isStarted)
    {
        if (!isThreadRunning())
        {
            // something's gone wrong and the thread's stopped..
            isOpen_ = false;
            return;
        }

        call->audioDeviceAboutToStart(this);

        const ScopedLock sl(startStopLock);
        callback = call;
        isStarted = true;
    }
}

void stop() 
{
    if (isStarted)
    {
        auto* callbackLocal = callback;

        {
            const ScopedLock sl(startStopLock);
            isStarted = false;
        }

        if (callbackLocal != nullptr)
            callbackLocal->audioDeviceStopped();
    }
}
*/
int main()
{
    std::cout << "Hello World!\n";
    initSource();
    //启动线程
    _beginthread(run, 0, NULL);
    while (1);

}
void setSource(HjkAudioSource* newSource)
{
    if (source != newSource)
    {
        auto* oldSource = source;

        if (newSource != nullptr && bufferSize > 0 && sampleRate > 0)
            newSource->prepareToPlay(bufferSize, sampleRate);

        {
            const ScopedLock sl(readLock);
            source = newSource;
        }

        if (oldSource != nullptr)
            oldSource->releaseResources();
    }
}
void initSource()
{
    //currentBufferSizeSamples
    //currentSampleRate etc.
    chsIn = 2;
    chsOut = 2;
    stop = false;
    isStarted = true;
}
void AudioSourcePlayerAudioDeviceIOCallback(const float** inputChannelData,
    int totalNumInputChannels,
    float** outputChannelData,
    int totalNumOutputChannels,
    int numSamples)
{
    // these should have been prepared by audioDeviceAboutToStart()...
    //@hjk debug flag
         //       jassert(sampleRate > 0 && bufferSize > 0);

    const ScopedLock sl(readLock);
    //初始化的时候需要
    if (source != nullptr)
    {
        int numActiveChans = 0, numInputs = 0, numOutputs = 0;

        // messy stuff needed to compact the channels down into an array
        // of non-zero pointers..
        for (int i = 0; i < totalNumInputChannels; ++i)
        {
            if (inputChannelData[i] != nullptr)
            {
                inputChans[numInputs++] = inputChannelData[i];
                
                if (numInputs >= numElementsInArray(inputChans))
                    break;                   
            }
        }

        for (int i = 0; i < totalNumOutputChannels; ++i)
        {
            if (outputChannelData[i] != nullptr)
            {
                outputChans[numOutputs++] = outputChannelData[i];

                if (numOutputs >= numElementsInArray(outputChans))
                    break;                    
            }
        }

        if (numInputs > numOutputs)
        {
            // if there aren't enough output channels for the number of
            // inputs, we need to create some temporary extra ones (can't
            // use the input data in case it gets written to)
            tempBuffer.setSize(numInputs - numOutputs, numSamples,
                false, false, true);

            for (int i = 0; i < numOutputs; ++i)
            {
                channels[numActiveChans] = outputChans[i];
                memcpy(channels[numActiveChans], inputChans[i], (size_t)numSamples * sizeof(float));
                ++numActiveChans;
            }

            for (int i = numOutputs; i < numInputs; ++i)
            {
                channels[numActiveChans] = tempBuffer.getWritePointer(i - numOutputs);
                memcpy(channels[numActiveChans], inputChans[i], (size_t)numSamples * sizeof(float));
                ++numActiveChans;
            }
        }
        else
        {
            for (int i = 0; i < numInputs; ++i)
            {
                channels[numActiveChans] = outputChans[i];
                memcpy(channels[numActiveChans], inputChans[i], (size_t)numSamples * sizeof(float));
                ++numActiveChans;
            }

            for (int i = numInputs; i < numOutputs; ++i)
            {
                channels[numActiveChans] = outputChans[i];
                zeromem(channels[numActiveChans], (size_t)numSamples * sizeof(float));
                ++numActiveChans;
            }
        }

        HjkAudioBuffer<float> buffer(channels, numActiveChans, numSamples);

        HjkAudioSourceChannelInfo info(&buffer, 0, numSamples);
        source->getNextAudioBlock(info);
        /*调整增益
        for (int i = info.buffer->getNumChannels(); --i >= 0;)
            buffer.applyGainRamp(i, info.startSample, info.numSamples, lastGain, gain);

        lastGain = gain;
        */
    }
    else
    {
        for (int i = 0; i < totalNumOutputChannels; ++i)
            if (outputChannelData[i] != nullptr)
                zeromem(outputChannelData[i], (size_t)numSamples * sizeof(float));
    }
}
//该函数来源于AudioDeviceManager，在此测试用，完整的工程中，应该定义该类作为usb设备的管理
void AudioDeviceManagerAudioDeviceIOCallbackInt(const float** inputChannelData,
    int numInputChannels,
    float** outputChannelData,
    int numOutputChannels,
    int numSamples)
{
    const ScopedLock sl(audioCallbackLock);
    /*@hjk debug
    inputLevelGetter->updateLevel(inputChannelData, numInputChannels, numSamples);
    outputLevelGetter->updateLevel(const_cast<const float**> (outputChannelData), numOutputChannels, numSamples);
    */
    //if (callbacks.size() > 0)
    if(1)
    {
       // AudioProcessLoadMeasurer::ScopedTimer timer(loadMeasurer);

        tempBuffer.setSize(jmax(1, numOutputChannels), jmax(1, numSamples), false, false, true);

        AudioSourcePlayerAudioDeviceIOCallback(inputChannelData, numInputChannels,outputChannelData, numOutputChannels, numSamples);

        auto** tempChans = tempBuffer.getArrayOfWritePointers();
  /*
        for (int i = callbacks.size(); --i > 0;)
        {
            callbacks.getUnchecked(i)->audioDeviceIOCallback(inputChannelData, numInputChannels,
                tempChans, numOutputChannels, numSamples);

            for (int chan = 0; chan < numOutputChannels; ++chan)
            {
                if (auto* src = tempChans[chan])
                    if (auto* dst = outputChannelData[chan])
                        for (int j = 0; j < numSamples; ++j)
                            dst[j] += src[j];
            }
        }
        */
    }
    else
    {
        for (int i = 0; i < numOutputChannels; ++i)
            zeromem(outputChannelData[i], (size_t)numSamples * sizeof(float));
    }
    /*
    if (testSound != nullptr)
    {
        auto numSamps = jmin(numSamples, testSound->getNumSamples() - testSoundPosition);
        auto* src = testSound->getReadPointer(0, testSoundPosition);

        for (int i = 0; i < numOutputChannels; ++i)
            for (int j = 0; j < numSamps; ++j)
                outputChannelData[i][j] += src[j];

        testSoundPosition += numSamps;

        if (testSoundPosition >= testSound->getNumSamples())
            testSound.reset();
    }
    */
}
void run(void*)
{
    //1.设定优先级
    //setMMThreadPriority();
    //设定缓存大小
    auto bufferSize = currentBufferSizeSamples;
    //设定输入通道数
    auto numInputBuffers = chsIn;
    //设定输出通道数
    auto numOutputBuffers = chsOut;
    //定义采样率是否改变
    bool sampleRateHasChanged = false;
    //定义输入和输出,浮点型
    HjkAudioBuffer<float>ins(numInputBuffers, bufferSize + 32);
    HjkAudioBuffer<float>outs(numOutputBuffers, bufferSize + 32);
    //定义输入和输出的缓冲区指针
    auto inputBuffers = ins.getArrayOfWritePointers();
    auto outputBuffers = outs.getArrayOfWritePointers();
    //清空输入和输出
    ins.clear();
    outs.clear();

    while (!stop)
    {
        //输入和输出的激活判断
        //if(inputdevice == nullptr)
        //if(outputdevice == nullptr){//采样率改变等判断}
        //满足条件则开始回调
        if (1)//
        {
            const ScopedTryLock s1(startStopLock);
            if (s1.isLocked() && isStarted)
                AudioDeviceManagerAudioDeviceIOCallbackInt(const_cast<const float**> (inputBuffers), numInputBuffers,
                outputBuffers, numOutputBuffers, bufferSize);
            else
                outs.clear();


        }
        //如果输出准备好，（设备、缓存可读等）则开始传输入下一个数组
        if (1)
        {
            //输出为一块缓存区，则该步骤不是必要的
            //采样率是否改变？
            if (/*外部值探查*/0)
            {
                sampleRateHasChanged = true;
                //sampleRateChangedByOutput = true;
            }
        }
    }

}

