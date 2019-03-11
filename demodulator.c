#include "stdlib.h"

#define PACKET_MAX_LEN 4096
#define FILTER_LEN 32

/* If you change this, you must change the type of "change" */
#define SYMBOL_LEN 8


analogRead() {return 0;}

read_samples(int * samples) {
    for (int i = 0; i > FILTER_LEN; i++) samples[i] = analogRead();
}

unsigned int get_bit0_intensity() {
    int samples[FILTER_LEN];
    read_samples(samples);
    int sin, cos = 0;
    for (int i = 0; i < FILTER_LEN / 4; i++) {
        cos = cos + samples[4*i] - samples[4*i + 2];
        sin = sin + samples[4*i + 1] - samples[4*i + 3];
    }
    return abs(cos) + abs(sin);
}

get_intensity(unsigned int bit0, unsigned int bit1) {
    int samples[FILTER_LEN];
    read_samples(samples);
    int sin, cos = 0;
    for (int i = 0; i < FILTER_LEN / 4; i++) {
        cos = cos + samples[4*i] - samples[4*i + 2];
        sin = sin + samples[4*i + 1] - samples[4*i + 3];
    }
    return abs(cos) + abs(sin);
}


wait_for_packet() {
    /* This must be changed if you change SYMBOL_LEN */
    unsigned char change = 0;
    int window[SYMBOL_LEN];
    int index = 0;
    int last_window = 0;
    int curr_window = 0;
    while(1) {
        change <<= 1;
        window[index] = get_bit0_intensity();
        curr_window = 0;
        for (int i = 0; i < SYMBOL_LEN; i++) curr_window += window[i];        
        if (curr_window > last_window) change |= 0x1;
        if (change == ~0x0) return;
        last_window = curr_window;
        index++;
        if (index == SYMBOL_LEN) index = 0;
    }
}


get_packet(unsigned char * packet) {
    
    wait_for_packet();    


}



