#include "microphone.h"
#include <iostream>

void main()
{
	Microphone mic;

	char** micArray;
	unsigned int numMics = 0;

	mic.GetNumMicrophones(&numMics);
	micArray = (char**)malloc(sizeof(char*) * numMics);
	for (int i = 0; i < numMics; i++)
		micArray[i] = (char*)malloc(sizeof(char) * 32);
	mic.GetAttatchedMicrophones(micArray);

	for (int i = 0; i < numMics; i++)
		printf("%s\n",micArray[i]);

	if (!mic.SelectMicrophone(0))
	{
		char error[512];
		mic.GetErrorString(error, 512);
		printf(error);
	}

	char** audioData = (char**)malloc(sizeof(char*) * 2);
	for (int i = 0; i < 2; i++)
		audioData[i] = (char*)malloc(sizeof(char) * BUFFER_SIZE);

	unsigned int len[2];
	int flags[2] = { 0, 0 };

	while (true)
	{
		if (!mic.Stream(audioData, len, flags))
		{
			char error[512];
			mic.GetErrorString(error, 512);
			printf(error);
		}
		else
		{
			if (flags[0] & WHDR_DONE)
				mic.PlayRecording(audioData[0], len[0], flags[0]);
		}
	}
}