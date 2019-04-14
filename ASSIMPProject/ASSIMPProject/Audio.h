#pragma once
//std library
#include <iostream>
//bass audio library
#include "Bass.h"

using namespace std;

class Audio {
public:
	//Audio object holds the filename of the sound and loads it into an object
	Audio(const char* filename)
	{	
		char* f = (char*)filename;
		sound = loadSample(f);

	}
	~Audio();
	//simple functionality from bass to manipulate and play sounds
	HSAMPLE getSample() { return sound; }
	HSAMPLE loadSample(const char * filename);

	void playSound(float volume);

private:
	HSAMPLE sound;
};