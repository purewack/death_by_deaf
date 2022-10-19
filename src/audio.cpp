#include "types.hpp"

float in01,in0,in1,pin0,pin1;
float out01;



int process ( void *out_b, void *in_b, unsigned int nBufferFrames,
           double streamTime, RtAudioStreamStatus status, void *data )
{ 	
	float* in_lr = (float*)in_b;
	float* out_lr = (float*)out_b;
	
    auto t = new ScopedTimer(&bench_dsp);
    //
    // inl = (float*)jack_port_get_buffer(input_portl, nframes);
    // inr = (float*)jack_port_get_buffer(input_portr, nframes);
    // outl = (float*)jack_port_get_buffer(output_portl, nframes);
    // outr = (float*)jack_port_get_buffer(output_portr, nframes);

    audioActionQue();
    for(int i=0; i<nBufferFrames; i++){
        // float spl0 = inl[i];
        // test_clip(&spl0);
      //   test_clip2(&spl0);
        // outl[i] = spl0;
    }
    ++audioActionQue;

    delete t;
    return 0;
}


void audio_end()
{
	if ( hw_audio.isStreamOpen() ) hw_audio.closeStream();
}
int audio_init(){
  if ( hw_audio.getDeviceCount() < 1 ) {
  		return -1;
  	}
  	RtAudio::StreamParameters iParams, oParams;
  	iParams.deviceId = hw_audio.getDefaultInputDevice(); // first available device
  	iParams.nChannels = 2;
    
  	oParams.deviceId = hw_audio.getDefaultOutputDevice(); // first available device
  	oParams.nChannels = 2;
    oParams.firstChannel = 0;
    
  	RtAudio::StreamOptions oStream;
  	oStream.flags = 0;//RTAUDIO_SCHEDULE_REALTIME | RTAUDIO_HOG_DEVICE;
  	oStream.priority = 90;
    unsigned int buf_size = 256;
    
  	try {
  		hw_audio.openStream( &oParams, &iParams, RTAUDIO_FLOAT32, 48000, &buf_size, &process, &oStream);
  	}
  	catch ( RtAudioError& e ) {
      e.printMessage();
  		return -1;
  	}
  	try {
  		hw_audio.startStream();
      max_dsp = std::chrono::duration_cast<std::chrono::nanoseconds>(frametime{1});
  	}
  	catch ( RtAudioError& e ) {
      e.printMessage();
  		return -1;
  	}
    
   return 0;
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

int AudioActionQue::enque(float p_r, void (*fp)(void)){
    return enque(frametime{ long( float(period)*p_r )},fp);
}
int AudioActionQue::enque(frametime w, void (*fp)(void)){

    if(actions_count == max_actions) return -1;
    if(que_count == max_actions) return -1;
    
    int q = que_count;
    auto o = (last + w.count());

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
                actions[i].action();
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
    clear();
}

Clip::~Clip(){
    free(_aData);
    free(_bData);
}


void Clip::operator()(float* inout){

    switch(state){
        case State::clear:
        case State::stop:
            return;
        break;

        case State::base:
            if(length >= AUDIO_CLIP_SIZE) return;
            _aData[head] = _bData[head] = *inout;
            length++;
        break;

        case State::play:
            *inout = _data[head];
        break;

        case State::dub:
            if(flags.refill == false)
            _data[head] += *inout;
            
            *inout = _data[head];
        break;
    }

    head++;
    if(head > length){
        head -= length;
    }

}

void Clip::swapStreams(){
    if(_data == _aData) {
        _data = _bData;
        _ndata = _aData;
    }
    else {
        _data = _aData;
        _ndata = _bData;
    }
}

void Clip::refill(){

    for(unsigned long i=0; i<length; i++){
        _ndata[i] = _data[i];
    }

    _undo_level++;
}

void Clip::clear(){
    state = State::clear;
    length = 0;
    head = 0;
    _data = _aData;
    _ndata = _bData;
}

void clip_stop(Clip* c){
    switch(c->state){
        case Clip::State::play:
            c->state = Clip::State::stop;
            c->head = 0;
        break;

        case Clip::State::base:
            c->clear();
        break;

        case Clip::State::dub:
            c->state = Clip::State::play;
            c->swapStreams();
            c->flags.refill = true;
        break;
    }
}

void clip_launch(Clip* c){
    switch(c->state){
        case Clip::State::clear:
            c->state = Clip::State::base;
        break;

        case Clip::State::stop:
            c->state = Clip::State::dub;
        break;

        case Clip::State::play:
            c->state = Clip::State::dub;
            c->swapStreams();
        break;

        case Clip::State::dub:
            c->flags.refill = true;
            c->state = Clip::State::play;
        break;

        case Clip::State::base:
            c->head = 0;
            c->state = Clip::State::play;
        break;
    }
}


void audio_supervisor(){
    Clip* clips[2];
    clips[0] = &test_clip;
    clips[1] = &test_clip2;

    for(int i=0; i<2; i++){
        if(clips[i]->flags.refill){
            clips[i]->refill();
            clips[i]->flags.refill = false;
        }
    }
}
