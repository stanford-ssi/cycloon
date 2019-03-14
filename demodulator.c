#include "stdlib.h"

#define PACKET_MAX_LEN 256
#define FILTER_LEN 64

/* If you change this, you must change the type of "change" */
#define SYMBOL_LEN 8
#define THRESHHOLD 3

int analogRead();

void read_samples(int * samples) {
    for (int i = 0; i < FILTER_LEN; i++) samples[i] = analogRead();
}

unsigned int get_bit0_intensity() {
    int s[FILTER_LEN];
    read_samples(s);
    int sin0 = 0, cos0 = 0;
    for (int i = 0; i < FILTER_LEN >> 3; i++) {
        int idx = i << 3;
        cos0 = cos0 + s[idx] + s[idx+1] - s[idx + 3] - s[idx + 4] - s[idx + 5] + s[idx + 7];
        sin0 = sin0 + s[idx+1] + s[idx+2] + s[idx + 3] - s[idx + 5] - s[idx + 6] - s[idx + 7];
    }
    int intensity = abs(cos0) + abs(sin0);
    return intensity;
}

void get_intensity(unsigned int * bit0, unsigned int * bit1) {
    int s[FILTER_LEN];
    read_samples(s);
    int sin0 = 0, cos0 = 0, sin1 = 0, cos1 = 0;
    for (int i = 0; i < FILTER_LEN >> 3; i++) {
        int idx = i << 3;
        cos0 = cos0 + s[idx] + s[idx+1] - s[idx + 3] - s[idx + 4] - s[idx + 5] + s[idx + 7];
        sin0 = sin0 + s[idx+1] + s[idx+2] + s[idx + 3] - s[idx + 5] - s[idx + 6] - s[idx + 7];
        cos1 = cos1 + s[idx] - s[idx+2] + s[idx+4] - s[idx+6];
        sin1 = sin1 + s[idx+1] - s[idx+3] + s[idx+5] - s[idx+7]; 
    }
    *bit0 = abs(cos0) + abs(sin0);
    *bit1 = abs(cos1) + abs(sin1);
    printf("Bit0:1 at the filter level %u %u\n", *bit0, *bit1);
}

void wait_for_packet() {
    /* This must be changed if you change SYMBOL_LEN */
    unsigned char change = 0;
    int window[SYMBOL_LEN];
    for (int i = 0; i < SYMBOL_LEN; i++) window[i] = 0;
    int index = 0;
    int last_window = 0;
    int curr_window = 0;
    while(1) {
        change <<= 1;
        window[index] = get_bit0_intensity();
        curr_window = 0;
        for (int i = 0; i < SYMBOL_LEN; i++) curr_window += window[i];        
        if (curr_window > last_window) change |= 0x1;
        if (change == (unsigned char) ~0x0) return;
        last_window = curr_window;
        index++;
        if (index == SYMBOL_LEN) index = 0;
    }
}

/* 0 means continue; 1 means ERR; -1 means EOM */
int get_byte(unsigned char * dest) {
    
    unsigned char res = 0;
    for (int i = 0; i < 8; i++) {
        int window0[SYMBOL_LEN], window1[SYMBOL_LEN];
        for (int i = 0; i < SYMBOL_LEN; i++)
            get_intensity(&window0[i], &window1[i]);
        unsigned int bit0_raw = 0, bit1_raw = 0;
        for (int i = 0; i < SYMBOL_LEN; i++) {
            bit0_raw += window0[i];
            bit1_raw += window1[i];
        }
        unsigned int bit0 = bit0_raw / bit1_raw;
        unsigned int bit1 = bit1_raw / bit0_raw;
        printf("%u, : %u\n", bit0_raw, bit1_raw);
        res <<= 1;
        if (bit1 > THRESHHOLD) res |= 0x1;
        else if (bit0 < THRESHHOLD) return 1;
    }
    if (res == 0) return -1;
    *dest = res;
    return 0;
}

void get_packet(unsigned char * packet) {
    while(1) {
        wait_for_packet();
        for (int i = 0; i < PACKET_MAX_LEN; i++) {
            int flag = get_byte(&packet[i]);
            if (flag == -1) break;
            if (flag == 1) return;
        }
    }
}