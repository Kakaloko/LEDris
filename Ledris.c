#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "init.h"
#include "display.h"

#define H 0
#define L 1
void moving(uint direction);
void core1_entry();
volatile bool tab[16][10] = {0};
unsigned short pos[4][2] = {{0, 0},{0, 1},{1, 0},{1, 1}}; 

int main()
{
    init_ledris();

     for(int i = 0; i < 16; i++){ //setting up all the leds low
        for(int j = 0; j < 10; j++){
            tab[i][j] = L;
        }
    }
    
    multicore_launch_core1(core1_entry);

    for(int i = 0; i < 4; i++){
        tab[pos[i][0]][pos[i][1]] = H;
    }

    while(true){
        sleep_ms(500);
        moving(3);
    }
   
}


void core1_entry(){

    while (true) {
        loop_display_table(tab);
    }

}

void button_handler(uint gpio, uint32_t event_mask){
    switch (gpio)
    {
    case LEFT:
        moving(1);
    
    case RIGHT:
        moving(2);
    
    case ROTATE:
        break;
    }
}

void moving(uint direction){
    switch (direction)
    {
    case 1:
        for(int i = 0; i < 4; i++){ // checking if not to far left
            if (pos[i][1] == 0){
                return;
            }
        }
        for(int i = 0; i < 4; i++){ // if not on left wall moving everything left
            tab[pos[i][0]][pos[i][1]] = L;
        }
        for(int i = 0; i < 4; i++){
            pos[i][1] -= 1;
            tab[pos[i][0]][pos[i][1]] = H;
            }
        break;
    
    case 2:
        for(int i = 0; i < 4; i++){ // checking if not to far right
            if (pos[i][1] == 9){
                return;
            }
        }
        for(int i = 0; i < 4; i++){ // if not on right wall moving everything right
            tab[pos[i][0]][pos[i][1]] = L;
            }
         for(int i = 0; i < 4; i++){ // if not on right wall moving everything right
            pos[i][1] += 1;
            tab[pos[i][0]][pos[i][1]] = H;
            } 
            
        break;
    case 3:
        bool floor = false;
        for(int i = 2; i < 4; i++){ // checking if not floor
            if (pos[i][1] == 15 || tab[pos[i][0]+1][pos[i][1]] == H){
                floor = true;
            }
        }
        if(floor == true){
            pos[0][0] = 0;
            pos[1][0] = 0;
            pos[2][0] = 1;
            pos[3][0] = 1;
            for(int i = 0; i < 4; i++){
                tab[pos[i][0]][pos[i][1]] = H;
            }
            return;
        }
        for(int i = 0; i < 4; i++){ 
            tab[pos[i][0]][pos[i][1]] = L;
        }
        for(int i = 0; i < 4; i++){
            pos[i][0] += 1;
            tab[pos[i][0]][pos[i][1]] = H;
            }
        break;
    default:
        break;
    }

    }
    
