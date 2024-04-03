#include <stdint.h>
#define KEY_BASE              0xFF200050
#define SW_BASE               0xFF200040
#define HEX3_HEX0_BASE        0xFF200020
#define HEX5_HEX4_BASE        0xFF200030
#define   MPCORE_PRIV_TIMER   0xFFFEC600

// volatile so they survive compilation
volatile int *switch_ptr = (int *)SW_BASE;//read the lower 4 switches
volatile int *key_ptr = (int *)KEY_BASE;//read the 4 push buttons
volatile int *hex_ptr_1 = (int *)HEX3_HEX0_BASE;//retrieve the value read from HEX3 to HEX0
volatile int *hex_ptr_2 = (int *)HEX5_HEX4_BASE;//retrieve the value read from HEX5 to HEX4
volatile int switch_val;//to store the value retrieved from switch_ptr
volatile int key_val;//to store the value retrieved from key_ptr

//define structure for manipulating the A9 private timer:
typedef struct
{
    uint32_t load;//first 4 bytes store the value you count with
    uint32_t count;//second 4 bytes store the current value reached
    uint32_t control;//third 4 bytes store the control settings (set last 2 bits to ...11 for on or ...10 for off)
    //set 2nd-last byte to store the frequency of the clock tics
    uint32_t status;//fourth 4 bytes store the status information (set to 0 or 1)
} PrivateTimer;

PrivateTimer *timer = (PrivateTimer *)MPCORE_PRIV_TIMER;//instantiate timer

//Function to configure timer
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

//hex representation for 0 to 9, for the BCD display
void DisplayHex(int value)//assume storing hundredths of seconds
{
    unsigned char bcd_code[10]  = {
        0x3F, 0x06, 0x5B, 0x4F,
        0x66, 0x6D, 0x7D, 0x07,
        0x7F, 0x67
    };//store hex representations for 0 to 9

    value = value%600000;//wrap back to zero if timer exceeds displayable amount

	//divide to hundredths of seconds, tenths of seconds, seconds, tens of seconds, minutes, and tens of minutes
    //after this, concatenate into strings that represent the display values desired
    //minutes string calculations
    *hex_ptr_2 = bcd_code[(value/6000)/10]*0x100//tens minutes
    + bcd_code[(value/6000)%10];//single minutes
    //seconds string calculations
    *hex_ptr_1 = bcd_code[((value%6000)/100)/10]*0x1000000//tens seconds
    + bcd_code[((value%6000)/100)%10]*0x10000//single seconds
    + bcd_code[(value%100)/10]*0x100//tenths seconds
    + bcd_code[(value%100)%10];//hundredths seconds
}

int ReadKeys(void)//read the 4 push buttons and interpret the instruction
{
    key_val = *(key_ptr) &= 0x0F;//convert retrieved value from the lowest 4 push buttons
    switch(key_val){
        case 0b1://start
            return 1;
            break;
        case 0b10://stop
            return 2;
            break;
        case 0b100://lap
            return 3;
            break;
        case 0b1000://clear
            return 4;
            break;
        default://ignore
            return 0;
            break;
    }
}

int ReadSwitch(void)// read the lowest switch and interpret the instructions
{
    switch_val = *(switch_ptr) &= 0x01;//convert retrieved value from the lowest switch
    return switch_val;//return converted value
}
	
int main(void) {
    int timerValue = 0;//timer value
    int state = 2;//state starts as stopped
    int lapVal = 0;//store the lap value
    volatile int prevState;//track whether to keep timer ticking after storing a lap
    InitTimer(2300000);//initialize the timer to complete every hundredth of a second

    //manipulate timer as indicated by the user
    while (1) 
	{
        //pass on the value if a valid value was read from a push button
        if (state==1 || state==2) prevState = state;//otherwise prevState remains same as before
        if(ReadKeys() > 0) state = ReadKeys();//otherwise state remains same as before
        
        //Display request
        switch(ReadSwitch()){
            case 0://display current timer value
                DisplayHex(timerValue);
                break;
            case 1://display stored lap value
                DisplayHex(lapVal);
                break;
        }

        //Counting request
        if(CheckTimer()==1){//update every 100th of a second, if needed
            switch(state){
                case 1://start
                    timerValue++;//increment counter
                    break;
                case 2://stop
                    break;//do nothing, so counter doesn't increment
                case 3://lap
                    //overwrite lap value
                    lapVal = timerValue;//resume incrementing
                    //if previous state was counting, resume, otherwise stop
                    if(prevState==1) timerValue++;
                    break;
                case 4://clear
                    timerValue = 0;//remain at time zero until resuming counter
                    lapVal = 0;
                    state = 2;//stop timer after clearing
                    break;
                default://force timer to stop if state somehow gets invalid values
                    state = 2;
                    break;
            }
        }
	else state = prevState; 
    }
}