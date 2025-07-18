#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "pico/rand.h"
#include "init.h"
#include "display.h"

#define H 0
#define L 1
#define ROWS 18
#define COLUMNS 10
#define FALL_TIME_MS 500

void core1_entry();
void move_right();
void move_left();
void move_down(uint32_t* fall_time_adr);
void random_block();
void clear_block();
void rotating();
void check_delete_rows(uint32_t* fall_time_adr);
void all_block_fall();
bool is_body(unsigned short place[2]);
void new_game(uint32_t* fall_time_adr);
bool speed_up_falling();
absolute_time_t last_button_time;
unsigned short pos[4][2] = {{0, 0},{0, 1},{1, 0},{1, 1}}; 
volatile bool tab[ROWS][COLUMNS] = {0};

int main()
{
    init_ledris();
    
    uint32_t fall_time = FALL_TIME_MS;
    new_game(&fall_time);
   
    multicore_launch_core1(core1_entry);
    
    while(true){
        if(speed_up_falling()){
            sleep_ms(fall_time*0.5);
        }else{
            sleep_ms(fall_time);
        }
        move_down(&fall_time);
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
        move_left();
        break;
    case RIGHT:
        move_right();
        break;
    case ROTATE:
        rotating();
        break;
    }
    last_button_time = get_absolute_time();
    
}

void move_left(){
    for(int i = 0; i < 4; i++){ // checking if not to far left
        if (pos[i][1] == 0){
            return;
        }
        if(tab[pos[i][0]][pos[i][1] - 1] == H){
            unsigned short place[2] = {pos[i][0] , pos[i][1] - 1};
            if (!is_body(place)){
                return;
            }
        }
    }
    clear_block();
    for(int i = 0; i < 4; i++){
        pos[i][1] = pos[i][1] - 1;
        tab[pos[i][0]][pos[i][1]] = H;
    }
}

void move_right(){
    for(int i = 0; i < 4; i++){ // checking if not to far right
        if (pos[i][1] == 9){
            return;
        }
        if(tab[pos[i][0]][pos[i][1] + 1] == H){
            unsigned short place[2] = {pos[i][0] , pos[i][1] + 1};
            if (!is_body(place)){
                return;
            }
        }
    }
    clear_block();
    for(int i = 0; i < 4; i++){ // if not on right wall moving everything right
        pos[i][1] += 1;
        tab[pos[i][0]][pos[i][1]] = H;
    } 
            
}

void move_down(uint32_t* fall_time_adr){
    bool floor = false;
    for(int i = 0; i < 4; i++){// checking if not floor
        if (pos[i][0] == 17){
            floor = true;
            break;
        }
        if (tab[pos[i][0] + 1][pos[i][1]] == H){
            unsigned short place[2] = {pos[i][0] + 1, pos[i][1]};
            if (!is_body(place)){
                floor = true;         
            };
        }
            
    }
    if(floor){
        check_delete_rows(fall_time_adr);
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
}

void clear_block(){
     for(int i = 0; i < 4; i++){ 
            tab[pos[i][0]][pos[i][1]] = L;
        }
}

void random_block(){
    unsigned short all_blocks[7][4][2] = {{{0, 4}, {0, 5}, {1, 4}, {1, 5}},//block
                                   {{1, 3}, {1, 4}, {1, 5}, {1, 6}},//long
                                   {{0, 4}, {1, 4}, {2, 4}, {2, 5}},//L
                                   {{0, 5}, {1, 5}, {2, 5}, {2, 4}},//J
                                   {{0, 6}, {0, 5}, {1, 5}, {1, 4}},//S
                                   {{0, 3}, {0, 4}, {1, 4}, {1, 5}},//Z
                                   {{0, 5}, {1, 5}, {1, 4}, {1, 6}},//T
                                  };

    int block = get_rand_32() % 7;
    for(int i = 0; i < 4; i++){
        for(int j = 0; j < 2; j++){
            pos[i][j] = all_blocks[block][i][j];
        }
    }
    
}

bool is_body(unsigned short place[2]){
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
        if(temp_pos[i][1] < 0 || temp_pos[i][1] > 9 || temp_pos[i][0] > 17){
            return;
        } 
        if(tab[temp_pos[i][0]][temp_pos[i][1]] == H){
            if(!is_body(temp_pos[i])){
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

void check_delete_rows(uint32_t* fall_time_adr){

    for(int i = ROWS - 1; i > 1; i -= 1){ //iterating through whole  table
        bool row = true;
        for(int j = 0; j < COLUMNS; j++){
            if(tab[i][j] != H){
                row = false;
                break;
            }
        }
        if(row){
            *fall_time_adr = *fall_time_adr * 0.95;
            all_block_fall(i);
            i += 1;
        }
    }

    for(int j = 0; j < COLUMNS; j++){
        if(tab[2][j] == H){
            new_game(fall_time_adr);
            return;
        }
    }
}

void all_block_fall(int row){
    for(int i = 0; i < COLUMNS; i++){
            tab[row][i] = L;
    }
    for(int i = row; i > 1; i -= 1){
        for(int j = 0; j < COLUMNS; j++){
            tab[i][j] = tab[i-1][j];
        }
    }
    
}

void new_game(uint32_t* fall_time_adr){
    for(int i = 0; i < ROWS; i++){ //setting up all the leds low
        for(int j = 0; j < COLUMNS; j++){
            tab[i][j] = L;
        }
    }
    *fall_time_adr = FALL_TIME_MS;
    clear_block();
    random_block();
    for(int i = 0; i < 4; i++){
            tab[pos[i][0]][pos[i][1]] = H;
    }
}

bool speed_up_falling(){
    if(gpio_get(LEFT) == 0 && gpio_get(RIGHT) == 0){
        return true;
    }else
    return false;
}