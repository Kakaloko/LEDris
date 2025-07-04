#include "pico\stdlib.h"
#include "init.h"



void init_ledris(){
    stdio_init_all();
    init_leds();
    init_demultiplexer();
    init_buttons();
}

void init_demultiplexer()
{
    for(uint i= 16; i < 21; i++){ //pins 16-20 for choosing rows on display
        gpio_init(i);
        gpio_set_dir(i, GPIO_OUT);
        gpio_put(i, false);
    }
    gpio_put(20, true);
}

void init_leds()
{
    for(uint i=0; i < 10; i++){ //pins 0-9 for setting each pin low or high in row
        gpio_init(i);
        gpio_set_dir(i, GPIO_OUT);
        gpio_put(i , true);
    }
}

void init_buttons(){
    for(uint i = 10; i < 14; i++){ //pins 10-13 are buttons
        gpio_init(i);
        gpio_set_dir(i, GPIO_IN);
        gpio_pull_up(i);
    }//todo irq interrupitons :""
    gpio_set_irq_enabled_with_callback(LEFT, GPIO_IRQ_EDGE_FALL, true, button_handler);
    gpio_set_irq_enabled_with_callback(RIGHT, GPIO_IRQ_EDGE_FALL, true, button_handler);
    gpio_set_irq_enabled_with_callback(ROTATE, GPIO_IRQ_EDGE_FALL, true, button_handler);
}


