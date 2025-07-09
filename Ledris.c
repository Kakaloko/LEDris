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


void moving(uint direction);
void random_block();
void clear_block();
void core1_entry();
void rotating();
void check_delete_rows();
void all_block_fall();
int is_body(unsigned short place[2]);
void new_game();
absolute_time_t last_button_time;
volatile bool tab[ROWS][COLUMNS] = {0};
unsigned short pos[4][2] = {{0, 0},{0, 1},{1, 0},{1, 1}}; 

int main()
{
    init_ledris();

    new_game();
    
    multicore_launch_core1(core1_entry);


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
            check_delete_rows();
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
    unsigned short all_blocks[7][4][2] = {{{0, 4}, {0, 5}, {1, 4}, {1, 5}},//block
                                   {{1, 3}, {1, 4}, {1, 5}, {1, 6}},//long
                                   {{0, 4}, {1, 4}, {2, 4}, {2, 5}},//L
                                   {{0, 5}, {1, 5}, {2, 5}, {2, 4}},//J
                                   {{0, 6}, {0, 5}, {1, 5}, {1, 4}},//S
                                   {{0, 3}, {0, 4}, {1, 4}, {1, 5}},//Z
                                   {{0, 5}, {1, 5}, {1, 4}, {1, 6}},//T
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
        if(temp_pos[i][1] < 0 || temp_pos[i][1] > 9 || temp_pos[i][0] > 17){
            return;
        } 
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

void check_delete_rows(){

    for(int i = ROWS - 1; i > 1; i -= 1){ //iterating through whole  table
        bool row = true;
        for(int j = 0; j < COLUMNS; j++){
            if(tab[i][j] != H){
                row = false;
                break;
            }
        }
        if(row){
            all_block_fall(i);
            i += 1;
        }
    }

    for(int j = 0; j < COLUMNS; j++){
        if(tab[2][j] == H){
            new_game();
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

void new_game(){
    for(int i = 0; i < ROWS; i++){ //setting up all the leds low
        for(int j = 0; j < COLUMNS; j++){
            tab[i][j] = L;
        }
    }
    clear_block();
    random_block();
    for(int i = 0; i < 4; i++){
            tab[pos[i][0]][pos[i][1]] = H;
    }
}