#include <stdio.h>
#include "pico/stdlib.h"

void init_demultiplexer();
void init_leds();
void set_leds();
void loop_demultiplexer();
void init_buttons();

int main()
{
    stdio_init_all();
    init_demultiplexer();
    init_leds();
    init_buttons();

    
    int tab[16][10] = {0};
    for(int i = 0; i < 16; i++){
        for(int j = 0; j < 10; j++){
            tab[i][j] = (i + j) % 2;
        }
    }



    while (true) {
        loop_demultiplexer(tab);
    }
}


void init_demultiplexer()
{
    for(uint i= 16; i < 21; i++){
        gpio_init(i);
        gpio_set_dir(i, true);
        gpio_put(i, false);
    }
    gpio_put(20, true);
}

void init_leds()
{
    for(uint i=0; i < 10; i++){
        gpio_init(i);
        gpio_set_dir(i, true);
        gpio_put(i , true);
    }
}

void init_buttons(){
    for(int i = 10; i < 14; i++){
        gpio_init(i);
        gpio_set_dir(i, 0);
        gpio_pull_up(i);
    }//todo irq interrupitons :""
}


void set_leds(int* tab){
    uint32_t mask = (1 << 0) | (1 << 1) | (1 << 2) | (1 << 3) | (1 << 4) | (1 << 5) | (1 << 6) | (1 << 7) | (1 << 8) | (1 << 9); //gpio 0 to 9

    gpio_put_masked(mask, 
        (tab[0] << 0) | (tab[1] << 1) | (tab[2] << 2) | (tab[3] << 3) | (tab[4] << 4) | (tab[5] << 5) | (tab[6] << 6) | (tab[7] << 7) | (tab[8] << 8) | (tab[9] << 9));
         // changing gpio value

}

void loop_demultiplexer(int tab[16][10]){
        uint32_t mask = (1 << 16) | (1 << 17) | (1 << 18) | (1 << 19); //pins 16, 17, 18, 19 are otuput for demultiplexer
        uint32_t out = (1 << 15);

        for(int i =0; i < 16; i ++){
            gpio_put_masked(mask, out);
            set_leds(tab[i]);
            out += (1 << 16);
            sleep_ms(1);
        }

}