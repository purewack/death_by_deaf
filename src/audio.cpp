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

    audioActionQue();
    for(int i=0; i<nframes; i++){
        float spl0 = inl[i];
        test_clip(&spl0);
        test_clip2(&spl0);
        outl[i] = spl0;
    }
    ++audioActionQue;

    delete t;
    return 0;
}


void audio_end()
{
    jack_client_close (client);
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


void AudioActionQue::q_test(void* data){
    LOG("test");
    LOG((int*)data);
}

void AudioActionQue::q_rec(void* data){
    clip_rec(&test_clip);
    test_clip.update();
}


void AudioActionQue::clear(){

    actions_count = 0;    
    que_count = 0;
    for(int i=0; i<max_actions; i++){
        actions[i].pending = false;
    }

}

void AudioActionQue::cancel(int i){
	if(i<0 or i>max_actions) return;
	actions[i].pending = false;
	if(actions_count) actions_count--;
}
void AudioActionQue::cancel(std::string with_name){
	for(int i=0; i<max_actions; i++){
		if(actions[i].name == with_name){		
			actions[i].pending = false;
			if(actions_count) actions_count--;
			break;
		}
	}
}

void AudioActionQue::unque(int q){
	if(q<0 or q>max_actions) return;
	que[q].pending = false;
	if(que_count) que_count--;
}

void AudioActionQue::unque(std::string with_name){
	for(int i=0; i<max_actions; i++){
		if(que[i].name == with_name){		
			que[i].pending = false;
			if(que_count) que_count--;
			break;
		}
	}
}

int AudioActionQue::enque(float p_r, void (*fp)(void*), void* arg){
    return enque(
		frametime{ long( float(period)*p_r )},
        fp, arg
    );
}
int AudioActionQue::enque(frametime w, void (*fp)(void*), void* arg){

    if(actions_count == max_actions) return -1;
    if(que_count == max_actions) return -1;
    
    int q = que_count;
    auto o = (last + w.count());

	if(que[q].action_data) free(que[q].action_data);
    que[q].action_data = arg;
    que[q].action = fp;

    que[q].offset.store(o);
	que[q].pending = true;
    que_count = q + 1;
    
    return que_count;
}

frametime AudioActionQue::confirm(){
    if(que_count == 0) return frametime{0};

    int j = 0;
    for(int i=0; i<max_actions; i++){
        if(actions[i].pending == false and que[j].pending){
            actions[i].action_data = que[j].action_data;
            actions[i].action = que[j].action;
            actions[i].offset.store(que[j].offset.load());
            actions[i].pending = true;
	    	que[j].pending = false;
            j++;
            if(++actions_count == max_actions or --que_count == 0) break;
        }
    }
    
    return frametime{tick.load()};
}


void AudioActionQue::operator()(){
    
    if(actions_count == 0){
        return;
    };
    
    for(int i=0; i<max_actions; i++){
        if(actions[i].pending){
            if(tick >= actions[i].offset) {
                actions[i].action(actions[i].action_data);
                actions_count--;
                actions[i].pending = false;
            }
        }
    }
}

void AudioActionQue::operator++(){
    if(period){
        tick += 1;
        if(tick > period+last){
            last.store(tick.load());
            period_ticks++;
        }
        period_ratio = float(tick-last) / float(period);
    }
}



Clip::Clip(){
    id = (unsigned long)this;
	_aData  =  (float*)calloc(AUDIO_CLIP_SIZE,sizeof(float));
	_bData  =  (float*)calloc(AUDIO_CLIP_SIZE,sizeof(float)); 
	_cData  =  (float*)calloc(AUDIO_CLIP_SIZE,sizeof(float)); 
    clear();
}

Clip::~Clip(){
    free(_aData);
    free(_bData);
    free(_cData);
}


void Clip::operator()(float* inout){

    switch(state){
        case State::clear:
        case State::stop:
            return;
        break;

        case State::base:
            _aData[head] = _bData[head] = *inout;
            if(length < AUDIO_CLIP_SIZE) {
                length++;
            }
        break;

        case State::play:
            *inout = _data[head];
        break;

        case State::dub:
            _cData[head] += *inout;
            *inout = _data[head] + _cData[head];
        break;

        case State::merge:
            *inout = _data[head] + _cData[head];
        break;
    }

    head++;
    if(head > length){
        head -= length;
    }

}


void Clip::clear(){
    n_state = State::clear;
    n_length = 0;
    n_head = 0;
    _data = _aData;
}

void Clip::update(){
    if(n_swap_data){
        if(_data == _aData)
            _data = _bData;
        else
            _data = _aData;

    }
    if(n_head) head = n_head;
    if(n_length) length = n_length;
    if(n_state != State::none) state = n_state;

    n_swap_data = false;
    n_head = 0;
    n_length = 0;
    n_state = State::none;
}

void Clip::merge(){
    float *destData, *srcData, *dubData;
    if(_data == _aData){
        destData = _bData;
        srcData = _aData;
    }
    else{
        destData = _aData;
        srcData = _aData;
    }
    dubData = _cData;

    for(unsigned long i=0; i<length; i++){
        destData[i] = srcData[i] + dubData[i];
        srcData[i] = destData[i];
        dubData[i] = 0;
    }

    _undo_level++;
}

void clip_stop(Clip* c){
    switch(c->state){
        case Clip::State::play:
            c->n_state = Clip::State::stop;
            c->n_head = 0;
        break;

        case Clip::State::base:
            c->clear();
        break;

        case Clip::State::dub:
            c->n_state = Clip::State::play;
        break;
    }
}

void clip_rec(Clip* c){
    switch(c->state){
        case Clip::State::clear:
            c->n_state = Clip::State::base;
        break;

        case Clip::State::stop:
            c->n_state = Clip::State::dub;
        break;

        case Clip::State::play:
            c->n_state = Clip::State::dub;
        break;

        case Clip::State::dub:
            c->n_state = Clip::State::merge;
        break;

        case Clip::State::base:
            c->n_head = 0;
            c->n_state = Clip::State::play;
        break;
    }
}

void clip_play(Clip* c){
    switch(c->state){
        case Clip::State::stop:
            c->n_head = 0;
            c->n_state = Clip::State::play;
        break;
    }
}

void audio_supervisor(){
    // Clip* clips[2];
 //    clips[0] = &test_clip;
 //    clips[1] = &test_clip2;
 //
 //    for(int i=0; i<2; i++){
 //        if(clips[i]->state == Clip::State::merge){
 //            clips[i]->merge();
 //            clips[i]->n_swap_data = true;
 //            clips[i]->n_state = Clip::State::play;
 //            clips[i]->update();
 //        }
 //    }
}
