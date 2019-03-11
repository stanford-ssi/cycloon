#include "demodulator.c"
#include "stdio.h"
#include "stdlib.h"

int main(int argc, char** argv) {
    unsigned char * packet = malloc(PACKET_MAX_LEN);

    get_packet(packet);
    printf("%s", packet);
    return 0;

}