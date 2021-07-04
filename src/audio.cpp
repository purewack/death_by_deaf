#include "types.hpp"

float in01,in0,in1,pin0,pin1;
float out01;


int audioProcess (jack_nframes_t nframes, void *arg)
{
    auto t = new ScopedTimer(&bench_dsp);

    inl = (float*)jack_port_get_buffer(input_portl, nframes);
    inr = (float*)jack_port_get_buffer(input_portr, nframes);
    outl = (float*)jack_port_get_buffer(output_portl, nframes);
    outr = (float*)jack_port_get_buffer(output_portr, nframes);

    // static const float freq_delta = 440.f/48000.f;
 //    static float phase;
 //    for(int i=0; i<nframes; i++){
 //        inl[i] = 0.5f*std::sin(2.0f*3.1415f * phase);
 //        inr[i] = inl[i];
 //        phase += freq_delta;
 //        if (phase > 1.0f) phase -= 1.0f;
 //    }

    audioActionQue.check();
    for(int i=0; i<nframes; i++){
        float spl0 = inl[i];
        test_clip.process(&spl0);
        test_clip2.process(&spl0);
        outl[i] = spl0;
    }

    delete t;
    return 0;
}



int audio_init(){
    
    client = jack_client_open("motif",JackNoStartServer,&status,NULL);
    if (client == NULL) {
        fprintf (stderr, "jack_client_open() failed, "
            "status = 0x%2.0x\n", status);
        if (status & JackServerFailed) {
            fprintf (stderr, "Unable to connect to JACK server\n");
        }
        return -1;
    }
    if (status & JackNameNotUnique) {
        jack_get_client_name(client);
        fprintf (stderr, "unique name assigned\n");
    }

    float bufs = jack_get_buffer_size(client);
    float srate = jack_get_sample_rate(client);
    float time_avail = bufs/srate;
    time_avail *= 1000.f; //milli
    time_avail *= 1000.f; //micro
    time_avail *= 1000.f; //nano
    int t = int(time_avail);
    max_dsp = std::chrono::nanoseconds{t};

    /////////

    const char **ports;
    jack_set_process_callback (client, audioProcess, 0);
    //jack_on_shutdown (client, jackEnd, 0);
    /* create three ports */

    input_portl = jack_port_register (client, "inputA",
                     JACK_DEFAULT_AUDIO_TYPE,
                     JackPortIsInput, 0);
    input_portr = jack_port_register (client, "inputB",
                     JACK_DEFAULT_AUDIO_TYPE,
                     JackPortIsInput, 0);
    output_portl = jack_port_register (client, "outputA",
                      JACK_DEFAULT_AUDIO_TYPE,
                      JackPortIsOutput, 0);
    output_portr = jack_port_register (client, "outputB",
                      JACK_DEFAULT_AUDIO_TYPE,
                      JackPortIsOutput, 0);


    if (jack_activate (client)) {
        fprintf (stderr, "cannot activate client");
        return -2;
    }
    ports = jack_get_ports (client, NULL, NULL,
                JackPortIsPhysical|JackPortIsOutput);
    if (ports == NULL) {
        fprintf(stderr, "no physical capture ports\n");
        return -3;
    }
    if (jack_connect (client, ports[0], jack_port_name (input_portl))) {
        fprintf (stderr, "cannot connect input ports\n");
    }
    if (jack_connect (client, ports[1], jack_port_name (input_portr))) {
        fprintf (stderr, "cannot connect input ports\n");
    }
    free (ports);
    ports = jack_get_ports (client, NULL, NULL,
                JackPortIsPhysical|JackPortIsInput);
    if (ports == NULL) {
        fprintf(stderr, "no physical playback ports\n");
        return -4;
    }

    if (jack_connect (client, jack_port_name (output_portl), ports[0])) {
        fprintf (stderr, "cannot connect output ports\n");
    }
    if (jack_connect (client, jack_port_name (output_portr), ports[1])) {
        fprintf (stderr, "cannot connect output ports\n");
    }

    free (ports);
   return 0;
}


void AudioActionQue::check(){
    m.lock();
    for(int i=0; i<max_actions; i++){
        if(not actions[i].done and not actions[i].qued){
            if(ft >= actions[i].when) {
                actions[i].action();
                actions[i].done = true;
            }
        }
    }
    ft += frametime{1};
    m.unlock();
}

void AudioActionQue::add(std::function<void(void)> f, frametime w){
    
    m.lock();
    for(int i=0; i<max_actions; i++){
        if(actions[i].done){
            free_slot = i;
            break;
        }
    }    
    m.unlock();
    
    AudioAction a;
    a.qued = true;
    a.done = false;
    a.action = f;
    a.when = w;
    
    m.lock();
    actions[free_slot] = a;
    m.unlock();
}
void AudioActionQue::confirm(){
    m.lock();
    for(int i=0; i<max_actions; i++){
        
        if(actions[i].qued){
            actions[i].when += ft;
            actions[i].qued = false;
        }
    }
    m.unlock();
}


Clip::Clip(){
    id = (unsigned long)this;
	dataStream  =  (float*)calloc(AUDIO_CLIP_SIZE,sizeof(float)); 
	dataStart   =  (float*)calloc(AUDIO_CLIP_SIZE,sizeof(float)); 
	dataDub     =  (float*)calloc(AUDIO_CLIP_SIZE,sizeof(float)); 
    aData = dataStream;
    clear();
}

Clip::~Clip(){
    free(dataStream);
    free(dataStart);
    free(dataDub);
}


void Clip::process(float* inout){
    switch(state){
        case State::clear:
        case State::stop:
            return;
        break;
        
        case State::base:
            aData[head++] = *inout;
            length += 1;
        break;
        
        case State::play:
            *inout = aData[head++];
        break;
    }
    
    if(head > length){
        head -= length;
    }
}


void Clip::clear(){
    state = State::clear;
    length = 0;
    head = 0;
}

void Clip::stop(){
    switch(state){
        case State::clear:
        case State::stop:
            return;
        break;

        case State::play:
            state = State::stop;
            head = 0;
        break;

        case State::base:
             clear();
        break;
    }
}

void Clip::rec(){
    switch(state){
        case State::clear:
            state = State::base;
        break;

        case State::stop:
            state = State::base;
        break;

        case State::play:

        break;

        case State::base:
            head = 0;
            state = State::play;
        break;
    }
}

void Clip::play(){
    switch(state){
        case State::clear:

        break;

        case State::stop:
        if(length){
            head = 0;
            state = State::play;
        }
        break;

        case State::play:

        break;

        case State::base:

        break;
    }
}