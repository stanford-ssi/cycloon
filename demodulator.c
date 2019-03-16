#include "stdlib.h"
#define PACKET_MAX_LEN 2048
#define FILTER_LEN 64

typedef unsigned char symbol;
#define SYMBOL_LEN sizeof(symbol)*8

#define THRESHHOLD 8

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
  //  printf("Bit0 0 : 1 1 at the filter level %d %d : %d %d \n", cos0, sin0, cos1, sin1);
}

void wait_for_packet() {
    /* This must be changed if you change SYMBOL_LEN */
    symbol change_record = 0;
    int window[SYMBOL_LEN];
    for (int i = 0; i < SYMBOL_LEN; i++) window[i] = 0;
    int index = 0;
    int past_reading = 0;
    //int last_window = 0;
    //int curr_window = 0;
    int last_change = 0;
    int curr_change = 0;
    while(1) { 
        past_reading = window[index];
        window[index] = get_bit0_intensity();
        //curr_window = 0;
        //for (int i = 0; i < SYMBOL_LEN; i++) curr_window += window[i];        
        //curr_change = curr_window - last_window;
        
        curr_change = window[index] - past_reading;
        if (curr_change < 0 && change_record == (symbol) ~0x0) return;
        change_record <<= 1;
        if (curr_change > 0) change_record |= 0x1U;
        //printf("%d\n", curr_change);
        /* && curr_change < (last_change << 1) */ 
        //if (change_record == (symbol) ~0x0) return;
        // last_window = curr_window;
        last_change = curr_change;
        index++;
        if (index == SYMBOL_LEN) index = 0;
    }
}

/* 0 means continue; 1 means EOM; -1 means ERR */
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
        unsigned int bit0 = bit0_raw / (bit1_raw + 1);
        unsigned int bit1 = bit1_raw / (bit0_raw + 1);
       // printf("%u, %u\n", bit0, bit1);
        res <<= 1;
        if (bit1 > THRESHHOLD) res |= 0x1;
        else if (bit0 < THRESHHOLD) return -1;
    }
    //printf("%x\n", res);
    *dest = res;
    if (res == 0) return 1;
    else return 0;
}

void get_packet(unsigned char * packet) {
    while(1) {
        wait_for_packet();
        for (int i = 0; i < SYMBOL_LEN - 1; i++) get_bit0_intensity();
       // printf("packet detected\n");
        for (int i = 0; i < PACKET_MAX_LEN; i++) {
            int flag = get_byte(&packet[i]);
            //printf("%d, %x\n", flag, packet[i]);
            if (flag > 0) return;
            if (flag < 0) break;
        }
    }
}