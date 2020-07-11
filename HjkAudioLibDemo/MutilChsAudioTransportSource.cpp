#include "MutilChsAudioTransportSource.h"

MutilChsAudioTransportSource::MutilChsAudioTransportSource()
{
}

MutilChsAudioTransportSource::~MutilChsAudioTransportSource()
{
	//setSource(nullptr);
	releaseMasterResources();
}
void MutilChsAudioTransportSource::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
}
void MutilChsAudioTransportSource::releaseResources()
{
}
/*��ʼ����
void MutilChsAudioTransportSource::setSource(PositionableAudioSource* newSource, int readAheadBufferSize, TimeSliceThread* readAheadThread, double sourceSampleRateToCorrectFor, int maxNumChannels)
{

}
*/
void MutilChsAudioTransportSource::getNextAudioBlock(const HjkAudioSourceChannelInfo& info)
{
	const ScopedLock s1(callbackLock);
	if (masterSource != nullptr && !stopped)
	{
		masterSource->getNextAudioBlock(info);
		//��ͣ����
		if (!playing)
		{
			// just stopped playing, so fade out the last block..
			/*
			for(int i = info.buffer->getNumChannels();--i>=0;)
				info.buffer->applyGainRamp(i, info.startSample, jmin(256, info.numSamples), 1.0f, 0.0f);
			if (info.numSamples > 256)
				info.buffer->clear(info.startSample + 256, info.numSamples - 256);*/
		}
	//Դ�����Ѷ��꣨�ҷ�ѭ���������ò���
		/*
		 if (positionableSource->getNextReadPosition() > positionableSource->getTotalLength() + 1
			  && ! positionableSource->isLooping())
		{
			playing = false;
			inputStreamEOF = true;
			sendChangeMessage();
		}
		*/
		stopped = !false;
		//������ͨ��Ӧ������
		/*
		for (int i = info.buffer->getNumChannels(); --i >= 0;)
			info.buffer->applyGainRamp(i, info.startSample,
				info.numSamples, lastGain, gain);
		*/
	}
	else
	{
		info.clearActiveBufferRegion();
		stopped = true;

	}
	lastGain = gain;
}

void MutilChsAudioTransportSource::releaseMasterResources()
{
	const ScopedLock sl(callbackLock);

	if (masterSource != nullptr)
		masterSource->releaseResources();

	isPrepared = false;
}
