#include <stdint.h>
#define SW_BASE               0xFF200040
#define HEX3_HEX0_BASE        0xFF200020
#define HEX5_HEX4_BASE        0xFF200030
#define MPCORE_PRIV_TIMER     0xFFFEC600


volatile int *switch_ptr = (int *)SW_BASE;//read the lower 4 switches
volatile int *key_ptr = (int *)KEY_BASE;//read the 4 push buttons
volatile int *hex_ptr_1 = (int *)HEX3_HEX0_BASE;//retrieve the value read from HEX3 to HEX0
volatile int *hex_ptr_2 = (int *)HEX5_HEX4_BASE;//retrieve the value read from HEX5 to HEX4
volatile int switch_val;//to store the value retrieved from switch_ptr
volatile int key_val;//to store the value retrieved from key_ptr

typedef struct{
    uint32_t load;//first 4 bytes store the value you count with
    uint32_t count;//second 4 bytes store the current value reached
    uint32_t control;//third 4 bytes store the control settings (set last 2 bits to ...11 for on or ...10 for off)
    //set 2nd-last byte to store the frequency of the clock tics
    uint32_t status;//fourth 4 bytes store the status information (set to 0 or 1)
} PrivateTimer;

PrivateTimer *timer = (PrivateTImer *)MPCORE_PRIV_TIMER

