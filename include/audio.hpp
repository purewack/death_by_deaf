#pragma once
#include <vector>
#include <atomic>
#include <rtmidi/RtMidi.h>
#include <rtaudio/RtAudio.h>
#include <jack/jack.h>
#include <jack/ringbuffer.h>

#define AUDIO_CLIP_SIZE 8388608 //8MB 8*1024*1024*4bytes ~ 43s @ 48kHz

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
 
void audio_end();
int audio_init();
int audioProcess (jack_nframes_t nframes, void *arg);
void audio_supervisor();

struct AudioAction;
struct AudioActionQue;
struct Clip;
struct Track;

struct Clip {
  
public:
    
    unsigned long id;
    enum class State:int{
        none = -1,
        clear = 0,
        stop,
        base,
        play,
        dub
    };
     
    std::atomic<State> state;
    std::atomic<unsigned long> length;
    std::atomic<unsigned long> head;
    struct {
        std::atomic<bool> refill;
    } flags;
    
    void operator()(float* inout);
    Clip();
    ~Clip();
    
    void clear();
    void refill();
    void undo();
    void swapStreams();
private:
    float *_aData, *_bData, *_data, *_ndata;
    unsigned int _undo_level;
};
    
void clip_stop(Clip* c);
void clip_launch(Clip* c);

inline Clip test_clip, test_clip2;


struct AudioAction{
    std::atomic<bool> pending = false;
    std::atomic<long> offset = 0;
    void (*action)(void);
	std::string name = "default";
};

struct AudioActionQue {

    std::atomic<long> tick = 0;
    std::atomic<long> last = 0;
    std::atomic<long> period = 0;
    std::atomic<float> period_ratio = 0.f;
    std::atomic<int> period_ticks = 0;
    
    const int max_actions = 16;
    AudioAction actions[16];
    AudioAction que[16];
    std::atomic<int> actions_count = 0;
    std::atomic<int> que_count = 0;

    int enque(frametime when, void (*func)(void));
    int enque(float ratio, void (*func)(void));
    void unque(int q);
    void cancel(int i);
	void unque(std::string with_name);
    void cancel(std::string with_name);
    void clear();
    frametime confirm();
    void operator()();
    void operator++();
    
    static void q_test();
    static void q_launch();
    static void q_stop();
};
inline AudioActionQue audioActionQue;
