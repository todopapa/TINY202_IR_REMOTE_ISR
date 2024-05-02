/*
TINY202_IR_REMOTE_ISR1.c
 * imported TV-B-Gone ATTINY85 to ATTINY202 for  general use IR remote 
 * This can use three botton Switches to control 3 functions. 
 * Created: 2024/05/02 17:29:46
 * Author : th2y-
 */ 
/*
Firmware
for use with ATtiny85
Mitch Altman + Limor Fried
7-Oct-07

Distributed under Creative Commons 2.5 -- Attib & Share Alike

Changed by Y.M. to import to ATMEL Studio environment
*/

/*
The hardware for this project is very simple:
ATtiny202 has 8 pins:
pin 1   Vdd +3v
pin 2   PA6 connects to SW2
pin 3   PA7 connects to SW0
pin 4   PA1 connects to SW1
pin 5   PA2 - visible LED, and also connects to programming circuitry
pin 6   PA0 connects to UPDI programming circuitry
pin 7   PA3 TCA0 equal to CMP0 output WO0 - IR emitter, through a 2SC1815 driver (with 330 ohm base resistor)
pin 8   ground
See the schematic for more details.
*/



#include <avr/io.h>             // this contains all the IO port definitions
#include <avr/interrupt.h>      // definitions for interrupts
#include <avr/sleep.h>          // definitions for power-down modes
#include <avr/pgmspace.h>       // definitions or keeping constants in program memory
#include "main.h"


#define LED_PIN     PIN2_bm     // PA2 (0x04)
#define IRLED_PIN   PIN3_bm     // PA3
#define SW0_PIN     PIN7_bm     // PA7
#define SW1_PIN     PIN1_bm     // PA1 (0x02)
#define SW2_PIN     PIN6_bm     // PA6
#define RESET       PIN0_bm     // PA0 (Not used)

// Original TV-B-GONE comments
/*
This project transmits a bunch of TV POWER codes, one right after the other,
with a pause in between each.  (To have a visible indication that it is
transmitting, it also pulses a visible LED once each time a POWER code is
transmitted.)  That is all TV-B-Gone does.  The tricky part of TV-B-Gone
was collecting all of the POWER codes, and getting rid of the duplicates and
near-duplicates (because if there is a duplicate, then one POWER code will
turn a TV off, and the duplicate will turn it on again (which we certainly
do not want).  I have compiled the top-40 most popular codes with the
duplicates eliminated, both for North America (which is the same as Asia, as
far as POWER codes are concerned -- even though much of Asia USES PAL video)
and for Europe (which works for Australia, New Zealand, the Middle East, and
other parts of the world that use PAL video).

Before creating a TV-B-Gone Kit, I originally started this project by hacking
the MiniPOV kit.  This presents a limitation, based on the size of
the Atmel ATtiny2313 internal flash memory, which is 2KB.  With 2KB we can only
fit about 7 POWER codes into the firmware's database of POWER codes.  40 codes
requires 8KB of flash memory, which is why we chose the ATtiny85 for the
TV-B-Gone Kit.

This version of the firmware has the most popular 40 POWER codes for North America.
*/

/*
The C compiler creates code that will transfer all constants into RAM when the microcontroller
resets.  Since this firmware has a table (powerCodes) that is too large to transfer into RAM,
the C compiler needs to be told to keep it in program memory space.  This is accomplished by
the macro PROGMEM (this is used in the definition for powerCodes).  Since the
C compiler assumes that constants are in RAM, rather than in program memory, when accessing
powerCodes, we need to use the pgm_read_word() and pgm_read_byte macros, and we need
to use powerCodes as an address.  This is done with PGM_P, defined below.
For example, when we start a new powerCode, we first point to it with the following statement:
PGM_P thecode_p = pgm_read_word(powerCodes+i);
The next read from the powerCode is a byte that indicates the carrier frequency, read as follows:
uint8_t freq = pgm_read_byte(thecode_p);
Subsequent reads from the powerCode are onTime/offTime pairs, which are words, read as follows:
ontime = pgm_read_word(thecode_p+(offset_into_table);
offtime = pgm_read_word(thecode_p+(offset_into_table);
*/

/*
This project is a good example of how to use the AVR chip timers.
 */

void SYSCLK_init(void) {
	// SYSCLK 8MHz (16MHz /2)
	/* Set the Main clock to internal 16MHz oscillator*/
	_PROTECTED_WRITE(CLKCTRL.MCLKCTRLA, CLKCTRL_CLKSEL_OSC20M_gc);
	/* Set the Main clock divider is / 2 and Main clock prescaler enabled. */
	_PROTECTED_WRITE(CLKCTRL.MCLKCTRLB, CLKCTRL_PDIV_2X_gc | CLKCTRL_PEN_bm); //0x01
}

void PIN_init(void) {
	/* set pin 7 of PA3, pin 5 of PA2 as output */
	PORTA.DIR |= LED_PIN;  // set pin5 of PA2 as Visible LED OUT
	PORTA.DIR |= IRLED_PIN;  // set pin7 of PA3 as IR LED Default WO0 output on ATtiny202P
	
   /* set PA2 and PA3 LED OFF */
	PORTA.OUTSET = LED_PIN; // Visible LED -> "H" (OFF)
	PORTA.OUTCLR = IRLED_PIN; // IR LED -> "L" (OFF)

	/* Set all pins except the LED pin to pullups*/
	PORTA.PIN0CTRL = PORT_PULLUPEN_bm;  // pullup RESET and UPDI pin (probably no meanings)
	PORTA.PIN1CTRL = PORT_PULLUPEN_bm; // pullup SW1 Input and reset ISR also
	PORTA.PIN6CTRL = PORT_PULLUPEN_bm;   // pullup SW2 Input and reset ISR also
	PORTA.PIN7CTRL = PORT_PULLUPEN_bm;  // pullup SW0 Input and reset ISR also
}

#define NOP __asm__ __volatile__ ("nop")
// This function delays the specified number of 10 microseconds
// #define DELAY_CNT 11
#define DELAY_CNT 7  // adjust timing for ATMEL Studio porting by Y.M.
void delay_ten_us(uint16_t us) {
	uint8_t timer;
	while (us != 0) {
		for (timer=0; timer <= DELAY_CNT; timer++) {
			 NOP;NOP;
		}
		//NOP;
		NOP;NOP;NOP;NOP;NOP;NOP;NOP;// adjust timing for ATMEL Studio porting
		us--;
	}
}

// This function quickly pulses the visible LED (connected to PB0, pin 5)
void quickflashLED( void ) {
	// pulse LED on for 30ms
	PORTA.OUTCLR = LED_PIN;         // turn on visible LED at PB0 by pulling pin to ground
	delay_ten_us(3000);         // 30 millisec delay
	PORTA.OUTSET = LED_PIN;          // turn off visible LED at PB0 by pulling pin to +3V
}

// This function quickly pulses the visible LED (connected to PB0, pin 5) 4 times
void quickflashLED4x( void ) {
	quickflashLED();
	delay_ten_us(15000);        // 150 millisec delay
	quickflashLED();
	delay_ten_us(15000);        // 150 millisec delay
	quickflashLED();
	delay_ten_us(15000);        // 150 millisec delay
	quickflashLED();
}
// This function transmits one Code Element of a POWER code to the IR emitter,
//   given offTime and onTime for the codeElement
//     If offTime = 0 that signifies the last Code Element of the POWER code
//     and the delay_ten_us function will have no delay for offTime
//     (but we'll delay for 250 milliseconds in the main function)
void xmitCodeElement(uint16_t ontime, uint16_t offtime ) {
	// start Timer1 outputting the carrier frequency to IR emitters on CMP0 WO0 (PA3, pin 7)
	/* set waveform output on PORT A */
	TCA0.SINGLE.CTRLB = TCA_SINGLE_CMP0EN_bm // enable compare channel 0
	| TCA_SINGLE_WGMODE_FRQ_gc;	// set Frequency mode
	/* disable event counting */
	TCA0.SINGLE.EVCTRL &= ~(TCA_SINGLE_CNTEI_bm);
	/* set frequency in FRQ mode */
	//	TCA0.SINGLE.CMP0 = PERIOD_EXAMPLE_VALUE;   //the period time (FRQ) already set in main()
	/* set clock source (sys_clk/div_value) */
	TCA0.SINGLE.CTRLA = TCA_SINGLE_CLKSEL_DIV1_gc   // set clock source (sys_clk/1)
	| TCA_SINGLE_ENABLE_bm; /* and start timer */
	// keep transmitting carrier for onTime
	delay_ten_us(ontime);
	//for debug continue emitting test
	// while(1);
	
	// turn off output to IR emitters on 0C1A (PB1, pin 6) for offTime
	TCA0.SINGLE.CTRLA &= ~(TCA_SINGLE_ENABLE_bm); /* stop timer to set bit "0" */
	TCA0.SINGLE.CTRLB = 0;	// CTRLB register RESET add for forced OUTPUT "L"
	PORTA.OUTCLR = IRLED_PIN;  // turn off IR LED

	delay_ten_us(offtime);
}

// IR Player Global parameter **********************************************
volatile int Play = 0;

// Pin change interrupt
ISR (PORTA_PORT_vect) {
	// pin change detection for each buttons and decide Play #
	int Buttons = PORTA.IN;
	if ((Buttons & SW0_PIN) == 0) Play = 0;
	else if ((Buttons & SW1_PIN) == 0) Play = 1;
	else if ((Buttons & SW2_PIN) == 0) Play = 2;
	else Play = 0;

	PORTA.INTFLAGS =  PORT_INT1_bm | PORT_INT6_bm | PORT_INT7_bm ;        // Clear PA1,PA6,PA7 interrupt flag
}


extern const struct powercode *const irCodes[] PROGMEM; // Changed by Y.M. to avoid error
extern uint8_t num_codes;

int main(void) {
	SYSCLK_init();
	PIN_init();
	while(1){	
		// pin change interrupt for buttons
		PORTA.PIN1CTRL |= PORT_ISC_LEVEL_gc;  // SW1  set ISR 
		PORTA.PIN6CTRL |= PORT_ISC_LEVEL_gc;  // SW2  set ISR
		PORTA.PIN7CTRL |= PORT_ISC_LEVEL_gc;  // SW0  set ISR
		sei();          // Interrupt enable
		
		// Sleep mode setting and enable
		set_sleep_mode(SLEEP_MODE_PWR_DOWN);
		sleep_enable();	// Pin change interrupt setting

		sleep_cpu();    // put CPU into Power Down Sleep Mode

		// after CPU wake
		PORTA.PIN1CTRL &= ~(PORT_ISC_LEVEL_gc);  // Turn off pin sense interrupt for SW1
		PORTA.PIN6CTRL &= ~(PORT_ISC_LEVEL_gc);  // Turn off pin sense interrupt for SW2
		PORTA.PIN7CTRL &= ~(PORT_ISC_LEVEL_gc);  // Turn off pin sense interrupt for SW0

		uint8_t i,j;  // counter i is not used
		uint16_t ontime, offtime;
		// if it doesn't need repeat IR emission, please comment out next line and line 288 "}"
		while ((~PORTA.IN & SW0_PIN) | (~PORTA.IN & SW1_PIN) | (~PORTA.IN & SW2_PIN)){ //while PINB1 or PIN2B is low then continue IR LED flashing
        // Play IR code for each buttons
		quickflashLED(); // visible indication that a code is being output
		//    PGM_P thecode_p = pgm_read_word(powerCodes+i);     // point to next POWER code
		PGM_P thecode_p = (PGM_P)pgm_read_word(irCodes+Play);     // emit each IR codes respectivelyto Play #
		uint8_t freq = pgm_read_byte(thecode_p);
		// set TCA0 CMP0 for Timer1 to output this POWER code's carrier frequency
		TCA0.SINGLE.CMP0 = freq;
		
		// transmit all codeElements for this POWER code (a codeElement is an onTime and an offTime)
		// transmitting onTime means pulsing the IR emitters at the carrier frequency for the length of time specified in onTime
		// transmitting offTime means no output from the IR emitters for the length of time specified in offTime
		j = 0;  // index into codeElements of this POWER code
		do {
			// read the onTime and offTime from the program memory
			ontime = pgm_read_word(thecode_p+(j*4)+1);
			offtime = pgm_read_word(thecode_p+(j*4)+3);

			xmitCodeElement(ontime, offtime);  // transmit this codeElement (ontime and offtime)
			j++;        // turn off IR LED
		} while ( offtime != 0 );  // offTime = 0 signifies last codeElement for a POWER code

		PORTA.OUTCLR = IRLED_PIN ;           // turn off IR LED

		// delay 250 milliseconds before transmitting next POWER code
		delay_ten_us(5000);  // 50mS wait untill next IR emit
		}
	
		// flash the visible LED on PB0  4 times to indicate that we're done
		delay_ten_us(20000); // wait 200mS
		quickflashLED4x();  // flash 4 times to indicate emit IR code finihed 
	}
}

