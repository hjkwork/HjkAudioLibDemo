#pragma once
#include "HjkAudioBuffer.h"
struct HjkAudioSourceChannelInfo
{
    /** Creates an uninitialised AudioSourceChannelInfo. */
    HjkAudioSourceChannelInfo() = default;

    /** Creates an AudioSourceChannelInfo. */
    HjkAudioSourceChannelInfo(HjkAudioBuffer<float> * bufferToUse,
        int startSampleOffset, int numSamplesToUse) noexcept
        : buffer(bufferToUse),
        startSample(startSampleOffset),
        numSamples(numSamplesToUse)
    {
    }

    /** Creates an AudioSourceChannelInfo that uses the whole of a buffer.
        Note that the buffer provided must not be deleted while the
        AudioSourceChannelInfo is still using it.
    */
    explicit HjkAudioSourceChannelInfo(HjkAudioBuffer<float> & bufferToUse) noexcept
        : buffer(&bufferToUse),
        startSample(0),
        numSamples(bufferToUse.getNumSamples())
    {
    }

    /** The destination buffer to fill with audio data.

        When the AudioSource::getNextAudioBlock() method is called, the active section
        of this buffer should be filled with whatever output the source produces.

        Only the samples specified by the startSample and numSamples members of this structure
        should be affected by the call.

        The contents of the buffer when it is passed to the AudioSource::getNextAudioBlock()
        method can be treated as the input if the source is performing some kind of filter operation,
        but should be cleared if this is not the case - the clearActiveBufferRegion() is
        a handy way of doing this.

        The number of channels in the buffer could be anything, so the AudioSource
        must cope with this in whatever way is appropriate for its function.
    */
    HjkAudioBuffer<float>* buffer;

    /** The first sample in the buffer from which the callback is expected
        to write data. */
    int startSample;

    /** The number of samples in the buffer which the callback is expected to
        fill with data. */
    int numSamples;

    /** Convenient method to clear the buffer if the source is not producing any data. */
    void clearActiveBufferRegion() const
    {
        if (buffer != nullptr)
            buffer->clear(startSample, numSamples);
    }
};
class HjkAudioSource
{
protected:
	HjkAudioSource() = default;
public:
	virtual ~HjkAudioSource() = default;
/*
//准备
*/
	virtual void prepareToPlay(int samplesPerBlockExpected,
		double sampleRate) = 0;
	/*
	//释放
	*/
	virtual void releaseResources() = 0;
	
	/*
	//获取下一块
	*/
	virtual void getNextAudioBlock(const HjkAudioSourceChannelInfo& bufferToFill) = 0;
};