#include "types.hpp"

float in01,in0,in1,pin0,pin1;
float out01;


int rtaudio_process( void *outputBuffer, void *inputBuffer, unsigned int nBufferFrames,
         double streamTime, RtAudioStreamStatus status, void *userData )
{
    auto t = new ScopedTimer(&bench_dsp);
    float *out_buffer = (float *) outputBuffer;
    float *in_buffer = (float *) inputBuffer;
    libpd_process_float(4,in_buffer,out_buffer);
    delete t;
    return 0;
}
void pdprint(const char *s) {
	std::cout << "[libpd]" << s << std::endl;
}

void audio_end()
{
	std::cout << "audio" << std::endl;
        try {
            adac.stopStream();
   		std::cout << "rtaudio end" << std::endl;
        }
        catch (RtAudioError& e) {
            e.printMessage();
        }
        if ( adac.isStreamOpen() ) adac.closeStream();
}
int audio_init(){

    std::cout << "[audio]" << std::endl;
        std::cout << "libpd" << std::endl;
            libpd_set_printhook(pdprint);
            libpd_init();
            libpd_init_audio(0, 2, 44100);
            libpd_start_message(1); 
            libpd_add_float(1.0f);
            libpd_finish_message("pd", "dsp");
            if (!libpd_openfile("main.pd","./sound"))
                return -1;
            
            if ( adac.getDeviceCount() < 1 ) 
                return -1;
            
            RtAudio::StreamParameters parameters;
            parameters.deviceId = adac.getDefaultOutputDevice();
            parameters.nChannels = 2;
            parameters.firstChannel = 0;
            unsigned int sampleRate = 44100;
            unsigned int bufferFrames = 256; 
            double data[2] = {0,0};
            try {
                adac.openStream( &parameters, NULL, RTAUDIO_FLOAT32,
                                sampleRate, &bufferFrames, &rtaudio_process, NULL );
                adac.startStream();
                max_dsp = std::chrono::duration_cast<std::chrono::nanoseconds>(frametime{1});
                std::cout << "rtaudio start" << std::endl;
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
    id = 0;//(unsigned long)this;
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
