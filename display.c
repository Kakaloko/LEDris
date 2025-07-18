#include "display.h"
#include "pico\stdlib.h"

void set_leds(bool* tab){
    uint32_t mask = (1 << 0) | (1 << 1) | (1 << 2) | (1 << 3) | (1 << 4) | (1 << 5) | (1 << 6) | (1 << 7) | (1 << 8) | (1 << 9); //gpio 0 to 9

    gpio_put_masked(mask, 
        (tab[0] << 0) | (tab[1] << 1) | (tab[2] << 2) | (tab[3] << 3) | (tab[4] << 4) | (tab[5] << 5) | (tab[6] << 6) | (tab[7] << 7) | (tab[8] << 8) | (tab[9] << 9));
         // changing gpio value

}

void loop_display_table(bool tab[18][10]){
        uint32_t mask = (1 << 16) | (1 << 17) | (1 << 18) | (1 << 19); //pins 16, 17, 18, 19 are otuput for demultiplexer
        uint32_t out = (1 << 15);

        for(int i = 2; i < 18; i ++){
            gpio_put_masked(mask, out);
            set_leds(tab[i]);
            out += (1 << 16);
            sleep_ms(1);
        }

}