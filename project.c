#define LED_BASE 0xFF200000
#define HEX3_HEX0_BASE 0xFF200020
#define SW_BASE 0xFF200040
#define SENSOR 0xFFC040FC

volatile int hex_code[16] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F, 0x77, 0x7C, 0x39, 0x5E, 0x79, 0x71};

volatile int *switch_ptr = (int *)SW_BASE;//read the lower 4 switches
volatile int *hex_ptr_1 = (int *)HEX3_HEX0_BASE;//retrieve the value read from HEX3 to HEX0
volatile int switch_val;//to store the value retrieved from switch_ptr

int readSwitches() {
    switch_val = *(switch_ptr) &= 0x01;
    return switch_val;
}

void displayHexDigit(int digit) {
    ((int)HEX3_HEX0_BASE) = hex_code[digit];
}

float readSensor() {
    return((float)SENSOR);
}

float convertToF(float v){
    return()
}


int main() {
    int switchValue;
    
    while(true){
        //need to know what this returns
        console.log(readSensor())

        if(readSwitches()==0){
            //in Celsius 
            

        }else{
            
        }
    }
}
