#pragma once
#include <iostream>
#include <atomic>
#include <vector>
#include "hjk_HeapBlock.h"
#include "hjk_MathsFunctions.h"
#include "hjk_FloatVectorOperations.h"

template <typename Type>
class HjkAudioBuffer
{
public:
	//创建一个空的缓存，其中通道数0和长度0
	HjkAudioBuffer() noexcept
		:channels(static_cast <Type**>(preallocatedChannelSpace))
	{

	}
	//其他的创建

	~HjkAudioBuffer() = default;
//返回通道数
	int getNumChannels() const noexcept { return numChannels; }
	//获取单个缓存通道的采样数
	int getNumSamples() const noexcept
	{
		return size;
	}
    /** Changes the buffer's size or number of channels.

       This can expand or contract the buffer's length, and add or remove channels.

       If keepExistingContent is true, it will try to preserve as much of the
       old data as it can in the new buffer.

       If clearExtraSpace is true, then any extra channels or space that is
       allocated will be also be cleared. If false, then this space is left
       uninitialised.

       If avoidReallocating is true, then changing the buffer's size won't reduce the
       amount of memory that is currently allocated (but it will still increase it if
       the new size is bigger than the amount it currently has). If this is false, then
       a new allocation will be done so that the buffer uses takes up the minimum amount
       of memory that it needs.

       Note that if keepExistingContent and avoidReallocating are both true, then it will
       only avoid reallocating if neither the channel count or length in samples increase.

       If the required memory can't be allocated, this will throw a std::bad_alloc exception.
   */
   /** Returns a pointer to an array of read-only samples in one of the buffer's channels.
      For speed, this doesn't check whether the channel number is out of range,
      so be careful when using it!
      If you need to write to the data, do NOT call this method and const_cast the
      result! Instead, you must call getWritePointer so that the buffer knows you're
      planning on modifying the data.
  */
    const Type* getReadPointer(int channelNumber) const noexcept
    {
        //@hjk debug flag
 //jassert(isPositiveAndBelow(channelNumber, numChannels));
        return channels[channelNumber];
    }

    /** Returns a pointer to an array of read-only samples in one of the buffer's channels.
        For speed, this doesn't check whether the channel number or index are out of range,
        so be careful when using it!
        If you need to write to the data, do NOT call this method and const_cast the
        result! Instead, you must call getWritePointer so that the buffer knows you're
        planning on modifying the data.
    */
    const Type* getReadPointer(int channelNumber, int sampleIndex) const noexcept
    {
        //@hjk debug flag
 //jassert(isPositiveAndBelow(channelNumber, numChannels));
        //@hjk debug flag
 //jassert(isPositiveAndBelow(sampleIndex, size));
        return channels[channelNumber] + sampleIndex;
    }

    /** Returns a writeable pointer to one of the buffer's channels.
        For speed, this doesn't check whether the channel number is out of range,
        so be careful when using it!
        Note that if you're not planning on writing to the data, you should always
        use getReadPointer instead.
    */
    Type* getWritePointer(int channelNumber) noexcept
    {
        //@hjk debug flag
  // jassert(isPositiveAndBelow(channelNumber, numChannels));
        isClear = false;
        return channels[channelNumber];
    }

    /** Returns a writeable pointer to one of the buffer's channels.
        For speed, this doesn't check whether the channel number or index are out of range,
        so be careful when using it!
        Note that if you're not planning on writing to the data, you should
        use getReadPointer instead.
    */
    Type* getWritePointer(int channelNumber, int sampleIndex) noexcept
    {
        //@hjk debug flag
  //jassert(isPositiveAndBelow(channelNumber, numChannels));
     //@hjk debug flag
 //   jassert(isPositiveAndBelow(sampleIndex, size));
        isClear = false;
        return channels[channelNumber] + sampleIndex;
    }

    /** Returns an array of pointers to the channels in the buffer.

        Don't modify any of the pointers that are returned, and bear in mind that
        these will become invalid if the buffer is resized.
    */
    const Type** getArrayOfReadPointers() const noexcept { return const_cast<const Type**> (channels); }

    /** Returns an array of pointers to the channels in the buffer.

        Don't modify any of the pointers that are returned, and bear in mind that
        these will become invalid if the buffer is resized.
    */
    Type** getArrayOfWritePointers() noexcept { isClear = false; return channels; }

    //==============================================================================
    void setSize(int newNumChannels,
        int newNumSamples,
        bool keepExistingContent = false,
        bool clearExtraSpace = false,
        bool avoidReallocating = false)
    {
        //@hjk debug flag
 //jassert(newNumChannels >= 0);
         //@hjk debug flag
  //jassert(newNumSamples >= 0);

        if (newNumSamples != size || newNumChannels != numChannels)
        {
            auto allocatedSamplesPerChannel = ((size_t)newNumSamples + 3) & ~3u;
            auto channelListSize = ((static_cast<size_t> (1 + newNumChannels) * sizeof(Type*)) + 15) & ~15u;
            auto newTotalBytes = ((size_t)newNumChannels * (size_t)allocatedSamplesPerChannel * sizeof(Type))
                + channelListSize + 32;

            if (keepExistingContent)
            {
                if (avoidReallocating && newNumChannels <= numChannels && newNumSamples <= size)
                {
                    // no need to do any remapping in this case, as the channel pointers will remain correct!
                }
                else
                {
                    HeapBlock<char, true> newData;
                    newData.allocate(newTotalBytes, clearExtraSpace || isClear);

                    auto numSamplesToCopy = (size_t)jmin(newNumSamples, size);

                    auto newChannels = reinterpret_cast<Type**> (newData.get());
                    auto newChan = reinterpret_cast<Type*> (newData + channelListSize);

                    for (int j = 0; j < newNumChannels; ++j)
                    {
                        newChannels[j] = newChan;
                        newChan += allocatedSamplesPerChannel;
                    }

                    if (!isClear)
                    {
                        auto numChansToCopy = jmin(numChannels, newNumChannels);

                        for (int i = 0; i < numChansToCopy; ++i)
                            FloatVectorOperations::copy(newChannels[i], channels[i], (int)numSamplesToCopy);
                    }

                    allocatedData.swapWith(newData);
                    allocatedBytes = newTotalBytes;
                    channels = newChannels;
                }
            }
            else
            {
                if (avoidReallocating && allocatedBytes >= newTotalBytes)
                {
                    if (clearExtraSpace || isClear)
                        allocatedData.clear(newTotalBytes);
                }
                else
                {
                    allocatedBytes = newTotalBytes;
                    allocatedData.allocate(newTotalBytes, clearExtraSpace || isClear);
                    channels = reinterpret_cast<Type**> (allocatedData.get());
                }

                auto* chan = reinterpret_cast<Type*> (allocatedData + channelListSize);

                for (int i = 0; i < newNumChannels; ++i)
                {
                    channels[i] = chan;
                    chan += allocatedSamplesPerChannel;
                }
            }

            channels[newNumChannels] = nullptr;
            size = newNumSamples;
            numChannels = newNumChannels;
        }
    }
    /** Clears all the samples in all channels. */
    void clear() noexcept
    {
        if (!isClear)
        {
            for (int i = 0; i < numChannels; ++i)
                FloatVectorOperations::clear(channels[i], size);

            isClear = true;
        }
    }
    /** Clears a specified region of all the channels.

       For speed, this doesn't check whether the channel and sample number
       are in-range, so be careful!
   */
    void clear(int startSample, int numSamples) noexcept
    {
        //@hjk debug flag
 //jassert(startSample >= 0 && numSamples >= 0 && startSample + numSamples <= size);

        if (!isClear)
        {
            if (startSample == 0 && numSamples == size)
                isClear = true;

            for (int i = 0; i < numChannels; ++i)
                FloatVectorOperations::clear(channels[i] + startSample, numSamples);
        }
    }
    /** Clears a specified region of just one channel.

            For speed, this doesn't check whether the channel and sample number
            are in-range, so be careful!
        */
    void clear(int channel, int startSample, int numSamples) noexcept
    {
        //@hjk debug flag
 //jassert(isPositiveAndBelow(channel, numChannels));
        //@hjk debug flag
  // jassert(startSample >= 0 && numSamples >= 0 && startSample + numSamples <= size);

        if (!isClear)
            FloatVectorOperations::clear(channels[channel] + startSample, numSamples);
    }
private:
    //==============================================================================
    int numChannels = 0, size = 0;
    size_t allocatedBytes = 0;
    Type** channels;
    HeapBlock<char, true> allocatedData;
    Type* preallocatedChannelSpace[32];
    std::atomic<bool> isClear{ false };

    void allocateData()
    {
        static_assert (std::alignment_of<Type>::value <= std::alignment_of<std::max_align_t>::value,
            "AudioBuffer cannot hold types with alignment requirements larger than that guaranteed by malloc");
        //@hjk debug flag
         //        jassert(size >= 0);

        auto channelListSize = (size_t)(numChannels + 1) * sizeof(Type*);
        auto requiredSampleAlignment = std::alignment_of<Type>::value;
        size_t alignmentOverflow = channelListSize % requiredSampleAlignment;

        if (alignmentOverflow != 0)
            channelListSize += requiredSampleAlignment - alignmentOverflow;

        allocatedBytes = (size_t)numChannels * (size_t)size * sizeof(Type) + channelListSize + 32;
        allocatedData.malloc(allocatedBytes);
        channels = reinterpret_cast<Type**> (allocatedData.get());
        auto chan = reinterpret_cast<Type*> (allocatedData + channelListSize);

        for (int i = 0; i < numChannels; ++i)
        {
            channels[i] = chan;
            chan += size;
        }

        channels[numChannels] = nullptr;
        isClear = false;
    }

    void allocateChannels(Type* const* dataToReferTo, int offset)
    {
        //@hjk debug flag
         //        jassert(offset >= 0);

        // (try to avoid doing a malloc here, as that'll blow up things like Pro-Tools)
        if (numChannels < (int)numElementsInArray(preallocatedChannelSpace))
        {
            channels = static_cast<Type**> (preallocatedChannelSpace);
        }
        else
        {
            allocatedData.malloc(numChannels + 1, sizeof(Type*));
            channels = reinterpret_cast<Type**> (allocatedData.get());
        }

        for (int i = 0; i < numChannels; ++i)
        {
            // you have to pass in the same number of valid pointers as numChannels
          //@hjk debug flag
  //   jassert(dataToReferTo[i] != nullptr);
            channels[i] = dataToReferTo[i] + offset;
        }

        channels[numChannels] = nullptr;
        isClear = false;
    }


};
