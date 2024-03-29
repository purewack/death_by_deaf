#pragma once
#include <vector>
#include <atomic>
#include "z_libpd.h"

#if defined(_WIN32)           
	#define NOGDI             // All GDI defines and routines
	#define NOUSER            // All USER defines and routines
    // Type required before windows.h inclusion
    typedef struct tagMSG *LPMSG;
#endif

#include "RtAudio.h"

#if defined(_WIN32)           // raylib uses these names as function parameters
	#undef near
	#undef far
#endif

#if defined(_WIN32)  
#include <stdexcept>
#include <limits>
#endif

using frametime = std::chrono::duration<int64_t, std::ratio<64,48000>>;

inline RtAudio adac;

void audio_end();
int audio_init();
int audioProcess (void *out_b, void *in_b, unsigned int nBufferFrames,
           double streamTime, RtAudioStreamStatus status, void *data );
void VAudio_bind();
inline bool audio_editor = false;