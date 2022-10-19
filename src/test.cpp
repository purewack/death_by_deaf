#include <stdio.h>
#include <iostream>
#include "z_libpd.h"

void pdprint(const char *s) {
    printf("%s", s);
}
int main(){
    std::cout << "test" << std::endl;
    libpd_set_printhook(pdprint);
    libpd_init();
    std::cout << "libpd" << std::endl;
    return 0;
}