#include <stdio.h>
#include <iostream>
#include <cstdlib>
#include "z_libpd.h"
#include "RtAudio.h"
#include "sol.hpp"
#include "raylib.h"
// Two-channel sawtooth wave generator.
int rtaudio_process( void *outputBuffer, void *inputBuffer, unsigned int nBufferFrames,
         double streamTime, RtAudioStreamStatus status, void *userData )
{
//   unsigned int i, j;
//   double *buffer = (double *) outputBuffer;
//   double *lastValues = (double *) userData;
//   if ( status )
//     std::cout << "Stream underflow detected!" << std::endl;
//   // Write interleaved audio data.
//   for ( i=0; i<nBufferFrames; i++ ) {
//     for ( j=0; j<2; j++ ) {
//       *buffer++ = lastValues[j];
//       lastValues[j] += 0.005 * (j+1+(j*0.1));
//       if ( lastValues[j] >= 1.0 ) lastValues[j] -= 2.0;
//     }
//   }
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

    std::cout << "rtaudio" << std::endl;
        RtAudio dac;
        if ( dac.getDeviceCount() < 1 ) {
            std::cout << "\nNo audio devices found!\n";
            exit( 0 );
        }
        RtAudio::StreamParameters parameters;
        parameters.deviceId = dac.getDefaultOutputDevice();
        parameters.nChannels = 2;
        parameters.firstChannel = 0;
        unsigned int sampleRate = 44100;
        unsigned int bufferFrames = 256; // 256 sample frames
        double data[2] = {0, 0};
        try {
            dac.openStream( &parameters, NULL, RTAUDIO_FLOAT32,
                            sampleRate, &bufferFrames, &rtaudio_process, NULL );
            dac.startStream();
        }
        catch ( RtAudioError& e ) {
            e.printMessage();
            exit( 0 );
        }
        


    std::cout << "sol2" << std::endl;
        sol::state lua;
        lua.open_libraries(sol::lib::base, sol::lib::package);
        lua.script("print(\"hello sol2\")");

    //raylib
        InitWindow(800, 450, "raylib [core] example - basic window");

        while (!WindowShouldClose())
        {
            BeginDrawing();
                ClearBackground(RAYWHITE);
                DrawText("raylib", 190, 200, 20, LIGHTGRAY);
            EndDrawing();
        }

        CloseWindow();
        try {
            // Stop the stream
            dac.stopStream();
        }
        catch (RtAudioError& e) {
            e.printMessage();
        }
        if ( dac.isStreamOpen() ) dac.closeStream();

    return 0;
}