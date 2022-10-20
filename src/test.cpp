#include <stdio.h>
#include <iostream>
#include <cstdlib>
#include "raylib.h"
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
#include "sol.hpp" 

int rtaudio_process( void *outputBuffer, void *inputBuffer, unsigned int nBufferFrames,
         double streamTime, RtAudioStreamStatus status, void *userData )
{
   float *out_buffer = (float *) outputBuffer;
   float *in_buffer = (float *) inputBuffer;
    libpd_process_float(4,in_buffer,out_buffer);
  return 0;
}

void pdprint(const char *s) {
    printf("%s", s);
}
int main(){
    std::cout << "test CMAKE" << std::endl;

    std::cout << "[script]" << std::endl;
        sol::state lua;
        lua.open_libraries(sol::lib::base, sol::lib::package);
        lua.script("print(\"[sol2]hello sol2\")");

    std::cout << "[audio]" << std::endl;
        std::cout << "libpd" << std::endl;
        libpd_set_printhook(pdprint);
        libpd_init();
        libpd_init_audio(0, 2, 44100);
        libpd_start_message(1); // one entry in list
        libpd_add_float(1.0f);
        libpd_finish_message("pd", "dsp");
        // open patch       [; pd open file folder(
        if (!libpd_openfile("wiggle.pd","."))
            return -1;
        
        RtAudio dac;
        if ( dac.getDeviceCount() < 1 ) 
            return -1;
        
        RtAudio::StreamParameters parameters;
        parameters.deviceId = dac.getDefaultOutputDevice();
        parameters.nChannels = 2;
        parameters.firstChannel = 0;
        unsigned int sampleRate = 44100;
        unsigned int bufferFrames = 256; // 256 sample frames
        double data[2] = {0,0};
        try {
            dac.openStream( &parameters, NULL, RTAUDIO_FLOAT32,
                            sampleRate, &bufferFrames, &rtaudio_process, NULL );
            dac.startStream();
            std::cout << "rtaudio start" << std::endl;
        }
        catch ( RtAudioError& e ) {
            e.printMessage();
            throw std::runtime_error("\nRTError\n"); 
        } 
    
    std::cout << "[gfx]" << std::endl;
        InitWindow(800, 450, "raylib [core] example - basic window");

        double dd = 0;
        while (!WindowShouldClose())
        {
            BeginDrawing();
                ClearBackground(BLACK);
                DrawText("DEAF", int(dd), int(dd), 20, WHITE);
                dd += 0.1;
                if (dd > 255.0) dd -=255.0;
            EndDrawing();
        }

        CloseWindow();


    std::cout << "audio" << std::endl;
        try {
            // Stop the stream
            dac.stopStream();
            std::cout << "rtaudio end" << std::endl;
        }
        catch (RtAudioError& e) {
            e.printMessage();
        }
        if ( dac.isStreamOpen() ) dac.closeStream();
    return 0;
}