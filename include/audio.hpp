#pragma once
#include <vector>
#include <rtmidi/RtMidi.h>
#include <rtaudio/RtAudio.h>
#include <jack/jack.h>

#define AUDIO_CLIP_SIZE 1048576 //1MB 8*1024*1024 ~ 21s @ 48kHz
using frametime = std::chrono::duration<int64_t, std::ratio<1,48000>>;

enum MidiBytes: int{
	on = 144, //x90
	off = 128, //x80
	cc = 176, //xb0
};

inline std::vector<unsigned char> rawmidi;
inline RtMidiIn scanner;
inline int devicesCount = 0;
inline std::vector<RtMidiIn*> devices;

inline jack_options_t options = JackNoStartServer;
inline jack_status_t status;
inline jack_port_t *input_portl;
inline jack_port_t *input_portr;
inline jack_port_t *output_portl;
inline jack_port_t *output_portr;
inline jack_client_t *client;
inline jack_default_audio_sample_t *inl, *inr, *outl, *outr;
 
int audio_init();
int audioProcess (jack_nframes_t nframes, void *arg);


struct AudioAction;
struct AudioActionQue;
struct Clip;

struct Clip {
  
public:
    unsigned long id;
    enum class State:int{
        clear,
        stop,
        base,
        play,
        dub
    };
    //std::atomic<State> state;
    State state;
    
    unsigned long length;
    unsigned long head;
   
    void process(float* inout);
    
    Clip();
    ~Clip();
    
    void clear();
    void stop();
    void rec();
    void play();
    void next();
    
private:
    float *dataStream, *dataStart, *dataDub, *aData;
    
    
    friend struct AudioActionQue;
};

inline Clip test_clip, test_clip2;


struct AudioAction{
    std::function <void(void)> action;
    bool done = true;
    bool qued = true;
    frametime when = frametime{0};
};

struct AudioActionQue {
private:
    frametime ft = frametime{0};
    const int max_actions = 16;
    AudioAction actions[16];
    int free_slot = 0;
    std::mutex m;
public:
    void add(std::function<void(void)> f, frametime when);
    void confirm();
    void clear();
    void check();
};
inline AudioActionQue audioActionQue;
