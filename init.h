#define RIGHT 12
#define LEFT 11
#define ROTATE 10

void init_ledris();
void init_demultiplexer();
void init_leds();
void init_buttons();
void button_handler(uint gpio, uint32_t event_mask);
