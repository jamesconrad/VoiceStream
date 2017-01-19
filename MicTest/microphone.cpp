#pragma comment(lib, "winmm.lib")

#include "microphone.h"


Microphone::Microphone()
{
	error = NO_ERR;
	recording = false;
	waveHandle = NULL;

	waveFormat.wFormatTag = WAVE_FORMAT_PCM;
	waveFormat.nChannels = 2;
	waveFormat.nSamplesPerSec = 8000;//8.0 or 11.025 or 22.05 or 44.1 (Khz)
	waveFormat.wBitsPerSample = 16; //8 or 16
	waveFormat.nBlockAlign = waveFormat.nChannels * waveFormat.wBitsPerSample / 8;
	waveFormat.nAvgBytesPerSec = waveFormat.nSamplesPerSec * waveFormat.nBlockAlign;
	waveFormat.cbSize = 0;

	for (int i = 0; i < 2; i++)
	{
		waveHeader[i].lpData = buffer[i];
		waveHeader[i].dwBufferLength = BUFFER_SIZE;
		waveHeader[i].dwUser = 0;
		waveHeader[i].dwFlags = NULL;
		waveHeader[i].dwLoops = 0;
	}

	waveOutOpen(&speakerHandle[0], WAVE_MAPPER, &waveFormat, NULL, NULL, CALLBACK_NULL | WAVE_FORMAT_DIRECT);
	waveOutOpen(&speakerHandle[1], WAVE_MAPPER, &waveFormat, NULL, NULL, CALLBACK_NULL | WAVE_FORMAT_DIRECT);
}

Microphone::~Microphone()
{
	for (int i = 0; i < 2; i++)
		waveInUnprepareHeader(waveHandle, &waveHeader[i], sizeof(waveHeader[i]));

	if (waveHandle)
		waveInClose(waveHandle);
}

bool Microphone::GetNumMicrophones(unsigned int* numMics)
{
	unsigned int micCount = waveInGetNumDevs();
	if (micCount == 0)
	{
		error = NO_MIC_DETECTED;
		return false;
	}
	devices.resize(micCount);
	*numMics = micCount;
}

bool Microphone::GetAttatchedMicrophones(char** micArray)
{
	for (int i = 0, s = devices.size(); i < s; i++)
	{
		WAVEINCAPS* dev = new WAVEINCAPS;
		waveInGetDevCaps(i, dev, sizeof(WAVEINCAPS));
		devices[i] = *dev;
		memcpy(micArray[i], dev->szPname, sizeof(char) * 32);
	}

	return true;
}

bool Microphone::SelectMicrophone(unsigned int mic)
{
	if (mic >= devices.size())
	{
		error = INVALID_MIC_SELECTION;
		return false;
	}

	if (waveHandle != NULL) //device is open
	{
		if (recording)
			waveInStop(waveHandle);
		waveInClose(waveHandle);
	}

	MMRESULT mmres;

	mmres = waveInOpen(&waveHandle, mic, &waveFormat, NULL, NULL, CALLBACK_NULL | WAVE_FORMAT_DIRECT);
	if (mmres != MMSYSERR_NOERROR)
	{
		mmResult = mmres;
		error = WAVE_IN_MMRESULT;
		return false;
	}

	for (int i = 0; i < 2; i++)
	{
		mmres = waveInPrepareHeader(waveHandle, &waveHeader[i], sizeof(waveHeader));
		if (mmres != MMSYSERR_NOERROR)
		{
			mmResult = mmres;
			error = WAVE_IN_MMRESULT;
			return false;
		}

		mmres = waveInAddBuffer(waveHandle, &waveHeader[i], sizeof(char) * BUFFER_SIZE);
		if (mmres != MMSYSERR_NOERROR)
		{
			mmResult = mmres;
			error = WAVE_IN_MMRESULT;
			return false;
		}
	}

	selectedDevice = mic;
	return true;
}

bool Microphone::Stream(char** returnData, unsigned int* len)
{
	if (!recording)
	{
		MMRESULT mmres = waveInStart(waveHandle);
		if (mmres != MMSYSERR_NOERROR)
		{
			mmResult = mmres;
			error = WAVE_IN_MMRESULT;
			return false;
		}

		recording = true;
		error = STREAM_RECORDING_STARTED;
		return false;
	}

	for (int i = 0; i < 1; i++)
	{
		if (waveHeader[i].dwFlags & WHDR_DONE)
		{
			waveOutWrite(speakerHandle[0], &waveHeader[0], sizeof(waveHeader[0]));
			len[i] = waveHeader[i].dwBytesRecorded;
			memcpy(returnData[i], waveHeader[i].lpData, waveHeader[i].dwBytesRecorded);
			waveHeader[i].dwBytesRecorded = 0;
			waveHeader[i].dwFlags = 0;
			waveInPrepareHeader(waveHandle, &waveHeader[i], sizeof(waveHeader[i]));
			waveInAddBuffer(waveHandle, &waveHeader[i], sizeof(waveHeader[i]));

		}
	}
	return true;
}

unsigned int Microphone::GetError()
{
	return error;
}

void Microphone::GetErrorString(char* errorMsg, unsigned int maxLen)
{
	if (error == WAVE_IN_MMRESULT)
	{
		waveInGetErrorText(mmResult, errorMsg, maxLen);
	}
	else
	{
		memcpy(errorMsg, "UNIMPLEMENTED\0", 14);
	}
}

void Microphone::PlayRecording(char* sound)
{

}
