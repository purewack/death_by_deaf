#pragma once
#include <exception>
#include "RtAudio.h"

inline double data[2];
void pdprint(const char *s);
int rtaudio_process( void *outputBuffer, void *inputBuffer, unsigned int nBufferFrames,
    double streamTime, RtAudioStreamStatus status, void *userData );
    
class AudioEngine{
    private:
        RtAudio dac;
    
    public:
        AudioEngine(const char* pdPatchName);

        ~AudioEngine();
};
