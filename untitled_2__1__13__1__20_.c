#include <math.h>

/* This files provides address values that exist in the system */

#define BOARD                 "DE10-Standard"

/* Memory */
#define DDR_BASE              0x00000000
#define DDR_END               0x3FFFFFFF
#define A9_ONCHIP_BASE        0xFFFF0000
#define A9_ONCHIP_END         0xFFFFFFFF
#define SDRAM_BASE            0xC0000000
#define SDRAM_END             0xC3FFFFFF
#define FPGA_PIXEL_BUF_BASE   0xC8000000
#define FPGA_PIXEL_BUF_END    0xC803FFFF
#define FPGA_CHAR_BASE        0xC9000000
#define FPGA_CHAR_END         0xC9001FFF

/* Cyclone V FPGA devices */
#define LED_BASE	      0xFF200000
#define LEDR_BASE             0xFF200000
#define HEX3_HEX0_BASE        0xFF200020
#define HEX5_HEX4_BASE        0xFF200030
#define SW_BASE               0xFF200040
#define KEY_BASE              0xFF200050
#define JP1_BASE              0xFF200060
#define JP2_BASE              0xFF200070
#define PS2_BASE              0xFF200100
#define PS2_DUAL_BASE         0xFF200108
#define JTAG_UART_BASE        0xFF201000
#define JTAG_UART_2_BASE      0xFF201008
#define IrDA_BASE             0xFF201020
#define TIMER_BASE            0xFF202000
#define TIMER_2_BASE          0xFF202020
#define AV_CONFIG_BASE        0xFF203000
#define RGB_RESAMPLER_BASE    0xFF203010
#define PIXEL_BUF_CTRL_BASE   0xFF203020
#define CHAR_BUF_CTRL_BASE    0xFF203030
#define AUDIO_BASE            0xFF203040
#define VIDEO_IN_BASE         0xFF203060
#define EDGE_DETECT_CTRL_BASE 0xFF203070
#define ADC_BASE              0xFF204000

/* Cyclone V HPS devices */
#define HPS_GPIO0_BASE        0xFF708000
#define HPS_GPIO1_BASE        0xFF709000
#define HPS_GPIO2_BASE        0xFF70A000
#define I2C0_BASE             0xFFC04000
#define I2C1_BASE             0xFFC05000
#define I2C2_BASE             0xFFC06000
#define I2C3_BASE             0xFFC07000
#define HPS_TIMER0_BASE       0xFFC08000
#define HPS_TIMER1_BASE       0xFFC09000
#define HPS_TIMER2_BASE       0xFFD00000
#define HPS_TIMER3_BASE       0xFFD01000
#define HPS_RSTMGR	      0xFFD05000
#define HPS_RSTMGR_PREMODRST  0xFFD05014
#define FPGA_BRIDGE           0xFFD0501C

#define PIN_MUX		      0xFFD08400
#define CLK_MGR		      0xFFD04000

#define SPIM0_BASE	      0xFFF00000
#define SPIM0_SR	      0xFFF00028
#define SPIM0_DR	      0xFFF00060
/* ARM A9 MPCORE devices */
#define   PERIPH_BASE         0xFFFEC000    // base address of peripheral devices
#define   MPCORE_PRIV_TIMER   0xFFFEC600    // PERIPH_BASE + 0x0600

/* Interrupt controller (GIC) CPU interface(s) */
#define MPCORE_GIC_CPUIF      0xFFFEC100    // PERIPH_BASE + 0x100
#define ICCICR                0x00          // offset to CPU interface control reg
#define ICCPMR                0x04          // offset to interrupt priority mask reg
#define ICCIAR                0x0C          // offset to interrupt acknowledge reg
#define ICCEOIR               0x10          // offset to end of interrupt reg
/* Interrupt controller (GIC) distributor interface(s) */
#define MPCORE_GIC_DIST       0xFFFED000    // PERIPH_BASE + 0x1000
#define ICDDCR                0x00          // offset to distributor control reg
#define ICDISER               0x100         // offset to interrupt set-enable regs
#define ICDICER               0x180         // offset to interrupt clear-enable regs
#define ICDIPTR               0x800         // offset to interrupt processor targets regs
#define ICDICFR               0xC00         // offset to interrupt configuration regs

	
//code borrowed from tutorial slides
#define TIMER_1_BASE 0xFF202000
typedef struct  _interval_timer{
	int status;
	int control;
	int low_period;
	int high_period;
	int low_counter;
	int high_counter;
} interval_timer;
volatile  interval_timer* const  timer_1= (interval_timer *) TIMER_1_BASE;
int  interval = 1000000000; //0.1s intervals for reading and temperature changing

volatile int* const adc_0 = (int*)(0xFF204000); //write 1 to read from adc_1
volatile int* const adc_1 = (int*)(0xFF204004); //read from this ADC
//can't mimic ADC by writing value to the channels; voltage values written to a variable
volatile int* const switches = (int*)(0xFF200040);
volatile int* const buttons = (int*)(0xFF200050);

volatile int temp; //make sure to initialize value
volatile int voltage;
volatile double tempD;
volatile double voltageD;
volatile int counterMax;
volatile int stop = 1;
volatile int counter = 0;
volatile int slowBoiler = 0;
volatile int fastBoiler = 0;

typedef struct _ADC_Simulator{
	int voltage;
	int temp; //make sure to initialize value
	double tempD;
	double voltageD;
} ADC_Sim;

volatile ADC_Sim ADCSim;
// variables for I/O
volatile int * LED_ptr = (int *)LED_BASE;
volatile int * toggle  = (int *) SW_BASE;
volatile int * button = (int *) KEY_BASE;
volatile int * segment = (int *)HEX3_HEX0_BASE;
volatile int * segment2 = (int *)HEX5_HEX4_BASE;

// variables for 7-segment displays
int WORD_TIMER = 500;
int letter = 0;
int clear = 1;
int currentMode = 0;

// variables for temperature
//test variables will be overwritten
int celsius = 120;
int fahnrenheit = 15;
int temperature[3];

//variables for words displays
int word_hello = 1;
int word_coffee = 0;
int word_tea = 0;
int word_water = 0;
int word_fast = 0;
int word_slow = 0;
int start = 0;

int displayDigit(int value)
{
    if (value == 1) {
            return 6;
    }
    if (value == 2) {
            return 91;
    }
    if (value == 3) {
            return 79;
    }
    if (value == 4) {
            return 102;
    }
    if (value == 5) {
            return 109;
    }
    if (value == 6) {
            return 125;
    }
    if (value == 7) {
            return 7;
    }
    if (value == 8) {
            return 127;
    }
    if (value == 9) {
            return 111;
    }
    if (value == 0) {
            return 63;
    }
    else{
        return 0;
    }
}

int wordList(int v, char word) { //stores words for 7-segment display.

    if (word == 'h'){ // displays HELLO
        if (v == 0) {
            return 118; // H
        }
        if (v == 1){
            return 121; // E
        }
        if (v == 2){
            return 56; // L
        }
        if (v == 3){
            return 56; // L
        }
        if (v == 4){
            return 63; // O
        }
        else{
            return 0; // no more letters
        }
    }
    if (word == 'c'){ // displays COFFEE
        if (v == 0) {
            return 57; // C
        }
        if (v == 1){
            return 63; // O
        }
        if (v == 2){
            return 113; // F
        }
        if (v == 3){
            return 113; // F
        }
        if (v == 4){
            return 121; // E
        }
        if (v == 5){
            return 121; // E
        }
        else{
            return 0; // no more letters
        }
    }
    if (word == 't'){ // displays TEA
        if (v == 0) {
            return 120; // t
        }
        if (v == 1){
            return 121; // E
        }
        if (v == 2){
            return 119; // A
        }
        else{
            return 0; // no more letters
        }
    }
    if (word == 'w'){ // displays DEFAULT (water)
        if (v == 0) {
            return 94; // D
        }
        if (v == 1){
            return 121; // E
        }
        if (v == 2){
            return 113; // F
        }
        if (v == 3){
            return 119; // A
        }
        if (v == 4){
            return 62; // U
        }
        if (v == 5){
            return 56; // L
        }
        if (v == 6){
            return 120; // T
        }
        else{
            return 0; // no more letters
        }
    }
    if (word == 'f'){ // displays FAST
        if (v == 0) {
            return 113; // F
        }
        if (v == 1){
            return 119; // A
        }
        if (v == 2){
            return 109; // S
        }
        if (v == 3){
            return 120; // T
        }
        else{
            return 0; // no more letters
        }
    }
    if (word == 's'){ //displays SLO
        if (v == 0){
            return 109; // S
        }
        if (v == 1){
            return 56; // L
        }
        if (v == 2){
            return 63; // O
        }
        else {
            return 0;
        }

    }
    

}

int moveWord(char word){ // displays a word in 7 seg.
    if(WORD_TIMER == 0){
        
        if( (*segment) != 0 || (*segment2) != 0){
            (*segment2) = ((*segment2) << 8) + ((*segment) >> 24);
            (*segment) = ((*segment) << 8) + wordList(letter,word);

            letter++;
            
        }
        if ((*segment) == 0 && (*segment2) == 0) { // If nothing is displayed on segments.
            if (letter){ // if word was already shown
                letter = 0;
                return 0;
            }
            (*segment) = ((*segment) << 8) + wordList(letter,word);
            letter++;
        }
        WORD_TIMER = 500;
    }
    WORD_TIMER--;
    return 1;
}

void clearDisplay(){ // clears display to show words.
    if(clear){
        (*segment) = 0;
        (*segment2) = 0;
        letter = 0;
        word_hello = 0;
        word_coffee = 0;
        word_tea = 0;
        word_water = 0;
        word_fast = 0;
        word_slow = 0;
        clear = 0;
    }
}

void clearToggle(){
    word_coffee = 0;
    word_tea = 0;
    word_water = 0;
}

void showTemp(){ // force display to show temperature
    word_hello = 0;
    word_coffee = 0;
    word_tea = 0;
    word_water = 0;
    word_fast = 0;
    word_slow = 0;
    clear = 1;
}


void displayTemperature(int temp,char type){ // shows the temperature on the display
    temperature[0] = temp/100;
    temperature[1] = (temp%100)/10;
    temperature[2] = temp%10;

    (*segment2) = displayDigit(temperature[0]);
    if (type == 'c'){
        (*segment) = (displayDigit(temperature[1])<<24) + (displayDigit(temperature[2])<<16) + (99<<8) + 57;
    }
    else{
        (*segment) = (displayDigit(temperature[1])<<24) + (displayDigit(temperature[2])<<16) + (99<<8) + 113;
    }
}

int slow_Boil(){
	//counterMax uses the values read through the ADC
	int counterMax = 6000; //10min water boil default
	if ((0b0000001100&&(*switches))&1){ //coffee
		counterMax = 5800;
	}
	else if ((0b0000000100&&(*switches))&1){ //tea
		counterMax = 5200;
	}
	//assuming +7.5C per min, +0.125C/s, 0.1s intervals
	counterMax += (25-temp)/(0.0125);
	return 0;
}

int fast_Boil(){
	//counterMax uses the values read through the ADC
	int counterMax = 3000; //5min water boil default
	if ((0b0000001100&&(*switches))&1){ //coffee
		counterMax = 2600;
	}
	else if ((0b0000000100&&(*switches))&1){ //tea
		counterMax = 1400;
	}
	//assuming +15C per min, +0.25C/s, 0.1s intervals
	counterMax += (25-temp)/(0.025);
	return 0;
}

int vToTemp(int adc){
	if (adc){
		//y = -25.44ln(x) + 266.97
		ADCSim.voltageD = (double)(ADCSim.voltage);
		ADCSim.tempD = -25.44*(log(ADCSim.voltageD)) + 266.97;
		ADCSim.temp = (int)(floor(ADCSim.tempD));
	}
	else{
		//y = -25.44ln(x) + 266.97
		voltageD = (double)(voltage);
		tempD = -25.44*(log(voltageD)) + 266.97;
		temp = (int)(floor(tempD));
	}
	return 0;
}

int tempToV(int adc){
	if (adc){
		//e^((x - 266.97)/(-25.44)) = y
		ADCSim.tempD = (double)(ADCSim.temp);
		ADCSim.voltageD = exp((ADCSim.tempD - 266.97)/(-25.44));
		ADCSim.voltage = (int)(floor(ADCSim.voltageD));
	}
	else{
		//e^((x - 266.97)/(-25.44)) = y
		tempD = (double)(temp);
		voltageD = exp((tempD - 266.97)/(-25.44));
		voltage = (int)(floor(voltageD));
	}
	return 0;
}


int main(void) {
    // initialize for restarts.
    word_hello = 1;
    word_coffee = 0;
    word_tea = 0;
    word_water = 0;
    word_fast = 0;
    word_slow = 0;
    start = 0;

    ////bitwise AND Bit Masks for buttons and switches////
    //buttons
    int off_mask = 1;
    int slow_mask = 2;
    int fast_mask = 4;

    //switches
    int boilType_mask = 12;
    int tempTextToggle_mask = 1;
    int tempTypeToggle_mask = 2;
    int special_mask = 4;
    int coffeeTea_mask = 8;

    ////Buttons and switches ////
    int off = 0; //prevent errors when starting code;
    int slowBoil;
    int fastBoil;
    

    int tempTextToggle;
    int tempTypeToggle;
    int special;
    int coffeeTea;
    int typeChange;
    //////////////////////////////

	//moved timer initialization to main because compiler complains
	// write  to low  period
	// only  lowest -16 bits  will be  written
	timer_1->low_period = interval;
	// write  to high  period
	timer_1->high_period = interval  >> 16;
	
	timer_1->control = 0b0110;
	
	volatile int channel_0;
	volatile int channel_1;
	
	//initialize test temperature value
	ADCSim.temp = 25;

    while(!off){  // turns off when off button is pressed.
        // check button and switches inputs
        off = (*button) &= off_mask;
        slowBoil = (*button) &= slow_mask;
        fastBoil = (*button) &= fast_mask;

        tempTextToggle = (*toggle) &= tempTextToggle_mask; // for testing purposes
        tempTypeToggle = (*toggle) &= tempTypeToggle_mask;
        special = (*toggle) &= special_mask; // toggles between normal water boil and tea/coffee boil
        coffeeTea = (*toggle) &= coffeeTea_mask;

        typeChange = (*toggle) &= boilType_mask; //checks change
        /////////////////////////////////////////////
		
        
        if ((slowBoil || fastBoil) && !start){ // starts the boil
            start = 1;
            clear = 1;
            if (slowBoil){ 
                word_slow = 1;
            }
            else{ 
                word_fast = 1;
            }

        }


        if (typeChange != currentMode){ // checks if the inputs have changed
            currentMode = typeChange; // update the current Mode, if statement runs only once
            clear = 1;
            clearToggle(); // prevent word default from showing when toggling for coffee/tea.
            if(!special){
                word_water = 1; // place max water temperature here
            }
            if(special && coffeeTea){ // place max tea temp here
                word_tea = 1;
            }
            else{
                word_coffee = 1; // place max coffee temperature here
            }
            
        }

        (*LED_ptr) = (*toggle);
        if(tempTextToggle){ // force display to show temperature over words.
            showTemp();
        }
        if(!word_hello && !word_coffee && !word_tea && !word_water && !word_fast && !word_slow){ // checks if there are any words loading
            if (!tempTypeToggle){ // default is celsius
                displayTemperature(celsius,'c');
            }
            else{
                displayTemperature(fahnrenheit,'f');
            }
        }
		
		tempToV(1);
		
		//check for timeout (0.1s passed and read from ADC)
		if(((timer_1->status)&&0b01)&1){
			(*adc_0) = 1;
			channel_1 = (*adc_1);
			//overwrite random simulator value with ADC class value
			channel_1 = ADCSim.voltage;
			
			voltage = channel_1;
			volatile int vttstatus = vToTemp(0); 
			//°F =°C * 1.8000+ 32.00
			celsius = temp;
			fahnrenheit = (temp*1.8) + 32;
			//can use a print statement to test if function was executed
			if ((4&(*switches))&&(temp>=95)){ //coffee
				stop = 1;
			}
			else if ((8&(*switches))&&(temp>=80)){ //tea
				stop = 1;
			}
			else if (temp >= 100){
				stop = 1;
			}
			if ((!stop)&slowBoiler&1){
				slow_Boil();
			}
			if ((!stop)&fastBoiler&1){
				fast_Boil();
			}
			if(slowBoiler&(!stop)){
				counter++;
				if (counter%80 == 0){
				//thermistor(physical) value updates
				ADCSim.temp++;
				}
			}
			if(fastBoiler&(!stop)){
				counter++;
				if (counter%40 == 0){
					//thermistor(physical) value updates
					ADCSim.temp++;
				}
			}
		}
		if (fastBoil){
			stop = 0;
			fastBoiler = 1;
		}
		if (slowBoil){
			stop = 0;
			slowBoiler = 1;
		}
		if ((0b0001&(*buttons))&&1){
			stop = 1;
		}
        /////////////////////// Word Displays go here. Purpose is to override the temperature display./////////
        if (word_hello){
            clearDisplay();
           word_hello = moveWord('h'); // returns 0 once word is already displayed, which disables the if statement, and resets the clear function
           if(!word_hello){
               clear = 1;
           }
        }
        if (word_water){
            clearDisplay();
            word_water = moveWord('w');
            if(!word_water){
               clear = 1;
           }
        }
        if (word_tea){
            clearDisplay();
            word_tea = moveWord('t');
            if(!word_tea){
               clear = 1;
           }
        }
        if (word_coffee){
            clearDisplay();
            word_coffee = moveWord('c');
            if(!word_coffee){
               clear = 1;
           }
        }
        if (word_fast){
            clearDisplay();
            word_fast = moveWord('f');
            if(!word_fast){
               clear = 1;
           }
        }
        if (word_slow){
            clearDisplay();
            word_slow = moveWord('s');
            if(!word_slow){
               clear = 1;
           }
        }
        //////////////////////////////////////////////////////////////////////////////////////////////////////
    }
}