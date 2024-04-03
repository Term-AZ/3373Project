#include <stdint.h>
#include <stdbool.h>
#define SW_BASE               0xFF200040
#define HEX3_HEX0_BASE        0xFF200020
#define HEX5_HEX4_BASE        0xFF200030
#define MPCORE_PRIV_TIMER     0xFFFEC600
#define KEY_BASE              0xFF200050


volatile int *switch_ptr = (int *)SW_BASE;//read the lower 4 switches
volatile int *key_ptr = (int *)KEY_BASE;//read the 4 push buttons
volatile int *hex_ptr_1 = (int *)HEX3_HEX0_BASE;//retrieve the value read from HEX3 to HEX0
volatile int *hex_ptr_2 = (int *)HEX5_HEX4_BASE;//retrieve the value read from HEX5 to HEX4
volatile int switch_val;//to store the value retrieved from switch_ptr
volatile int key_val;//to store the value retrieved from key_ptr

int p1Score=0;
int p2Score=0;
int timerValue =0;
bool prevState=false;
bool curState = false;

unsigned char bcd_code[11]  = {
        0x3F, 0x06, 0x5B, 0x4F,
        0x66, 0x6D, 0x7D, 0x07,
        0x7F, 0x67, 0x36
    };

typedef struct{
    uint32_t load;//first 4 bytes store the value you count with
    uint32_t count;//second 4 bytes store the current value reached
    uint32_t control;//third 4 bytes store the control settings (set last 2 bits to ...11 for on or ...10 for off)
    //set 2nd-last byte to store the frequency of the clock tics
    uint32_t status;//fourth 4 bytes store the status information (set to 0 or 1)
} PrivateTimer;

PrivateTimer *timer = (PrivateTimer *)MPCORE_PRIV_TIMER;

void InitTimer(int size){
    timer->load = size;//time of 1 hour in hundredths of seconds
    timer->control=0b11;//start as on, tick every clock cycle, auto-cycle back
    timer->status=1;//reset the status if it was already flagged as "done"
}

//Function to check if timer is done, and restore if it is
int CheckTimer(void){
    if(timer->status){//if status flag gets set
        timer->status=1;//unset it
        return 1;//equivalent to true
    }
    else return 0;//equivalent to false
}

void DisplayHex1(int value, int tValue){
    *hex_ptr_1 = bcd_code[value]*0x1
    + bcd_code[10]*0x100
    + bcd_code[tValue]*0x10000
    + bcd_code[0]*0x1000000;
}

void DisplayHex2(int value){
    *hex_ptr_2 = bcd_code[10]*0x1
    + bcd_code[value]*0x100;
}

unsigned char GetCharCode(int value){
    unsigned char bcd_code[11]  = {
        0x3F, 0x06, 0x5B, 0x4F,
        0x66, 0x6D, 0x7D, 0x07,
        0x7F, 0x67, 0x36
    };
    return bcd_code[value];
}

void InitDisplay(void){
    DisplayHex1(0, 0);
    DisplayHex2(0);
}

int ReadKeys(void)//read the 4 push buttons and interpret the instruction
{
    key_val = *(key_ptr) &= 0x0F;//convert retrieved value from the lowest 4 push buttons
    switch(key_val){
        case 0b1://start
            return 1;
            break;
        case 0b10://stop
            return 0;
            break;
        // case 0b100://lap
        //     return 3;
        //     break;
        // case 0b1000://clear
        //     return 4;
        //     break;
        // default://ignore
        //     return 0;
        //     break;
    }
}

int ReadPlayer1(void){
    
}

int ReadPlayer2(void){

}

void ResetGame(void){
    p1Score=0;
    p2Score=0;
    timerValue=0;
}

int ReadSwitch(bool f)// read the lowest switch and interpret the instructions
{
    if(f){
        switch_val = *(switch_ptr)&=0x0F;//convert retrieved value 
    }else{
        switch_val = *(switch_ptr)&=0xF0 >>4;//convert retrieved value 
    }
    return switch_val;//return converted value
}

void CheckRoundWinner(){
    int p1 = ReadSwitch(true);
    int p2 = ReadSwitch(false);

    if(p1>p2){
        p1Score++;
    }else{
        p2Score++;
    }
    DisplayHex1(p1, 0);
    DisplayHex2(p2);
}

int main(void){
    
    ResetGame();
    InitDisplay();

    InitTimer(230000000);

    while(1){

        

        if(ReadKeys()==1){
            if(!curState){
                curState=true;//game running
            }else{

            }
        }

        if(curState){
            if(CheckTimer()==1){
                timerValue++;
                if(timerValue==9){
                    CheckRoundWinner();
                    break;
                }

                DisplayHex1(0,9-timerValue);
            }
        }
    }
}