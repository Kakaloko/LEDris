#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "pico/rand.h"
#include "init.h"
#include "display.h"

#define H 0
#define L 1

void moving(uint direction);
void random_block();
void clear_block();
void core1_entry();
void rotating();
int is_body(unsigned short place[2]);
absolute_time_t last_button_time;
volatile bool tab[18][10] = {0};
unsigned short pos[4][2] = {{0, 0},{0, 1},{1, 0},{1, 1}}; 

int main()
{
    init_ledris();

     for(int i = 0; i < 18; i++){ //setting up all the leds low
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
    if (absolute_time_diff_us(last_button_time, get_absolute_time()) < 150000){
        return; // if not enought time from last push do nothing
    }
    switch (gpio)
    {
    case LEFT:
        moving(LEFT);
        break;
    case RIGHT:
        moving(RIGHT);
        break;
    case ROTATE:
        rotating();
        break;
    }
    last_button_time = get_absolute_time();
    
}

void moving(uint direction){ //change it to move_left/ rigt itd not switch case less readable
    switch (direction)
    {
    case LEFT:
        for(int i = 0; i < 4; i++){ // checking if not to far left
            if (pos[i][1] == 0){
                return;
            }
            if(tab[pos[i][0]][pos[i][1] - 1] == H){
                unsigned short place[2] = {pos[i][0] , pos[i][1] - 1};
                if (is_body(place) == false){
                    return;
                }
            }
        }
    
        clear_block();
            
        for(int i = 0; i < 4; i++){
            pos[i][1] = pos[i][1] - 1;
            tab[pos[i][0]][pos[i][1]] = H;
            }
        break;
    
    case RIGHT:
        for(int i = 0; i < 4; i++){ // checking if not to far right
            if (pos[i][1] == 9){
                return;
            }
            if(tab[pos[i][0]][pos[i][1] + 1] == H){
                unsigned short place[2] = {pos[i][0] , pos[i][1] + 1};
                if (is_body(place) == false){
                    return;
                }
            }
        }
        clear_block();
        for(int i = 0; i < 4; i++){ // if not on right wall moving everything right
            pos[i][1] += 1;
            tab[pos[i][0]][pos[i][1]] = H;
            } 
            
        break;
    case 3:
        bool floor = false;
        for(int i = 0; i < 4; i++){// checking if not floor
            if (pos[i][0] == 17){
                floor = true;
                break;
            }
            if (tab[pos[i][0] + 1][pos[i][1]] == H){
                unsigned short place[2] = {pos[i][0] + 1, pos[i][1]};
                if (is_body(place)== false){
                    floor = true;
                };
            }
            
        }
        if(floor == true){
            random_block();
            for(int i = 0; i < 4; i++){
                tab[pos[i][0]][pos[i][1]] = H;
            }
            return;
        }
        clear_block();
        for(int i = 0; i < 4; i++){
            pos[i][0] += 1;
            tab[pos[i][0]][pos[i][1]] = H;
            }
        break;
    default:
        break;
    }
    }

void clear_block(){
     for(int i = 0; i < 4; i++){ 
            tab[pos[i][0]][pos[i][1]] = L;
        }
}

void random_block(){
    unsigned short all_blocks[7][4][2] = {{{0, 0}, {0, 1}, {1, 0}, {1, 1}},//block
                                   {{1, 0}, {1, 1}, {1, 2}, {1, 3}},//long
                                   {{0, 0}, {1, 0}, {2, 0}, {2, 1}},//L
                                   {{0, 1}, {1, 1}, {2, 0}, {2, 1}},//J
                                   {{0, 2}, {0, 1}, {1, 0}, {1, 1}},//S
                                   {{0, 0}, {0, 1}, {1, 1}, {1, 2}},//Z
                                   {{0, 1}, {1, 1}, {1, 0}, {1, 2}},//T
                                  };

    int block = get_rand_32() % 7;
    printf("%d", block);
    for(int i = 0; i < 4; i++){
        for(int j = 0; j < 2; j++){
            pos[i][j] = all_blocks[block][i][j];
        }
    }
    
}

int is_body(unsigned short place[2]){
    for(int i = 0; i < 4; i++){
        if(place[0] == pos[i][0] && place[1] == pos[i][1]){
            return true;
        }
    }
    return false;
}

void rotating(){
    short temp_pos[4][2] = {0};
    for(int i = 0; i < 4; i++){
        temp_pos[i][0] = pos[i][1] - pos[1][1] + pos[1][0]; // doing matrix rotation 
        temp_pos[i][1] = -1 * (pos[i][0] - pos[1][0]) + pos[1][1]; 
        if(tab[temp_pos[i][0]][temp_pos[i][1]] == H){
            if(is_body(temp_pos[i]) == false){
                    return;
                }
        }
    }
    clear_block();
    for(int i = 0; i < 4; i++){
        pos[i][0] = temp_pos[i][0];
        pos[i][1] = temp_pos[i][1];
        tab[pos[i][0]][pos[i][1]] = H;
    }
}