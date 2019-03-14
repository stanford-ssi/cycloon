#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "demodulator.c"

static FILE* fp;

int main(int argc, char** argv) {
    fp = fopen(argv[1], "r");
    unsigned char packet[PACKET_MAX_LEN];
    //printf("%s", packet);
    
    get_packet(packet);
    printf("%s\n", packet);
    return 0;

}

int analogRead() {
    char buf[35];
    if (!fgets(buf, 35, fp)) exit(0);
    int res = atoi(buf);
    return res;
}