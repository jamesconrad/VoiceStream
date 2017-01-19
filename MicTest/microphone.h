#pragma once

#include <windows.h>
#include <mmsystem.h>
#include <vector>

//half a second of buffer
#define BUFFER_SIZE 8000 * 2 * 2 / 4

class Microphone
{
public:
	//All functions that have a return will return true or false; if true the function sucessfully completed
	//if false the function failed, error information will be provided via GetErrorString; follows opengl GetError rules
	Microphone();
	~Microphone();

	//@param1 : char* err - c string error message, reallocated on call
	//@param2 : unsigned int maxLen - maximum lenght of err
	void GetErrorString(char* err, unsigned int maxLen);

	//returns the uint error
	unsigned int GetError();

	//@param1 : unsigned int* numMics - number of mic devices found, overwritten on call
	bool GetNumMicrophones(unsigned int* numMics);

	//@param1 : char** micArray - c string array of device names, max length is 32 chars, micArray must be prealloc'd, overwritten on call
	bool GetAttatchedMicrophones(char** micArray);

	//@param1 : unsigned int mic - index of devices to be used for recording
	bool SelectMicrophone(unsigned int mic);
		
	//@param1 : char* data - char pointer to location of the audio buffer
	//@param2 : unsigned int* len - length of recorded audio, overwritten on call
	bool Stream(char** returnData, unsigned int* len);


	void PlayRecording(char* sound);

	enum MICERROR
	{
		NO_ERR = 0, STREAM_RECORDING_STARTED = 1, NO_MIC_DETECTED = 2, INVALID_MIC_SELECTION, WAVE_IN_MMRESULT
	};

private:
	MICERROR error;
	std::vector<WAVEINCAPS> devices;
	HWAVEIN waveHandle;
	WAVEFORMATEX waveFormat;
	WAVEHDR waveHeader[2];
	MMRESULT mmResult;
	HWAVEOUT speakerHandle[2];
	unsigned int selectedDevice;
	bool recording;
	char buffer[2][BUFFER_SIZE];
};