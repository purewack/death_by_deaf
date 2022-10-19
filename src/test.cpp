#include <stdio.h>
#include <iostream>
#include "z_libpd.h"
#include "RtAudio.h"

void pdprint(const char *s) {
    printf("%s", s);
}
int main(){
    std::cout << "test CMAKE" << std::endl;

    std::cout << "libpd" << std::endl;
    libpd_set_printhook(pdprint);
    libpd_init();

    std::cout << "rtaudio" << std::endl;
    std::cout << "\nRtAudio Version " << RtAudio::getVersion() << std::endl;
    return 0;
}