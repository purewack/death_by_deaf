#include "types.hpp"

void AudioActionQue::q_test(){
    LOG("test");
}
void AudioActionQue::q_launch(){
    clip_launch(&test_clip);
}
void AudioActionQue::q_stop(){
    clip_stop(&test_clip);
}