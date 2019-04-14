#include "Audio.h"

HSAMPLE Audio::loadSample(const char * filename)
{
	HSAMPLE sam;
	if (sam = BASS_SampleLoad(FALSE, filename, 0, 0, 3, BASS_SAMPLE_OVER_POS))

		std::cout << "sample " << filename << " loaded!" << endl;
	else
	{
		cout << "Can't load sample";
		exit(0);
	}
	return sam;
}
void Audio::playSound(float volume) {
	HCHANNEL ch = BASS_SampleGetChannel(sound, FALSE);
	BASS_ChannelSetAttribute(ch, BASS_ATTRIB_FREQ, 0);
	BASS_ChannelSetAttribute(ch, BASS_ATTRIB_VOL, volume);
	BASS_ChannelSetAttribute(ch, BASS_ATTRIB_PAN, -1);
	if (!BASS_ChannelPlay(ch, FALSE))
		cout << "Can't play sample" << endl;
}