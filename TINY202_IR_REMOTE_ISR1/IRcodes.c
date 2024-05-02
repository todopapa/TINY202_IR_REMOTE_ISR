#include <avr/io.h>            //  // this contains all the IO port definitions
#include <avr/pgmspace.h>       // definitions or keeping constants in program memory
#include "main.h"
// #define F_CPU 8000000UL // CPU Clock = 8MHz Add by Y.M.
#ifndef F_CPU
//#define F_CPU 20000000UL // CPU Clock = 20MHz no prescaler divisor
#define F_CPU 8000000UL // CPU Clock = 16MHz 1/2 prescaler divisor
#endif

// table of IR codes

#define freq_to_timerval(x) ((F_CPU / x - 1 )/ 2)

// Code 000 -- Panasonic ceiling light remote controller HK9337 substitution
const struct powercode modeCode PROGMEM = {  // Changed to M50560 MODE code
	freq_to_timerval(36370), // 36.37 KHz
	//   {{352,  181},  // Leader code
	{{372,  191},  // Leader code adjusted
	// Custom code 34h 4Ah 90h
	{ 46,   46}, // 0 C0
	{ 46,   46}, // 0 C1
	{ 46,   136}, // 1 C2
	{ 46,   136}, // 1 C3
	{ 46,   46}, // 0 C4
	{ 46,   136}, // 1 C5
	{ 46,   46}, // 0 C6
	{ 46,   46}, // 0 C7
	// Custom code 4Ah
	{ 46,   46}, // 0 C0
	{ 46,   136}, // 1 C1
	{ 46,   46}, // 0 C2
	{ 46,   46}, // 0 C3
	{ 46,   136}, // 1 C1
	{ 46,   46}, // 0 C5
	{ 46,   136}, // 1 C1
	{ 46,   46}, // 0 C7
	
	// Custom code 90h
	{ 46,   136}, // 1 C0
	{ 46,   46}, // 0 C1
	{ 46,   46}, // 0 C2
	{ 46,   136}, // 1 C3
	{ 46,   46}, // 0 C4
	{ 46,   46}, // 0 C5
	{ 46,   46}, // 0 C6
	{ 46,   46}, // 0 C7
	
	// Data code  14h 84h
	{ 46,   46},  // 0 D0
	{ 46,   46},  // 0 D1
	{ 46,   46},  // 0 D2
	{ 46,   136},  // 1 D3
	{ 46,   46},  // 0 D4
	{ 46,   136},  // 1 D5
	{ 46,   46},  // 0 D6
	{ 46,   46},  // 0 D7
	// Data code  84h
	{ 46,   136},  // 1 D0
	{ 46,   46},  // 0 D1
	{ 46,   46},  // 0 D2
	{ 46,   46},  // 0 D3
	{ 46,   46},  // 0 D4
	{ 46,   136}, // 1 D5
	{ 46,   46},  // 0 D6
	{ 46,   46},  // 0 D7
	// { 59, 4117}
	{ 46, 0}  // end of code
}
};

//Code 001 - Panasonic ceiling light remote controller HK9337 substitution
const struct powercode upCode PROGMEM = { // Changed to M50560 UP code
	freq_to_timerval(36370), // 36.37 KHz
	//   {{352,  181},  // Leader code
	{{372,  191},  // Leader code adjusted

	// Custom code 34h 4Ah 90h
	{ 46,   46}, // 0 C0
	{ 46,   46}, // 0 C1
	{ 46,   136}, // 1 C2
	{ 46,   136}, // 1 C3
	{ 46,   46}, // 0 C4
	{ 46,   136}, // 1 C5
	{ 46,   46}, // 0 C6
	{ 46,   46}, // 0 C7
	// Custom code 4Ah
	{ 46,   46}, // 0 C0
	{ 46,   136}, // 1 C1
	{ 46,   46}, // 0 C2
	{ 46,   46}, // 0 C3
	{ 46,   136}, // 1 C1
	{ 46,   46}, // 0 C5
	{ 46,   136}, // 1 C1
	{ 46,   46}, // 0 C7
	
	// Custom code 90h
	{ 46,   136}, // 1 C0
	{ 46,   46}, // 0 C1
	{ 46,   46}, // 0 C2
	{ 46,   136}, // 1 C3
	{ 46,   46}, // 0 C4
	{ 46,   46}, // 0 C5
	{ 46,   46}, // 0 C6
	{ 46,   46}, // 0 C7
	
	// Data code  54h C4h
	{ 46,   46},  // 0 D0
	{ 46,   136},  // 1 D1
	{ 46,   46},  // 0 D2
	{ 46,   136},  // 1 D3
	{ 46,   46},  // 0 D4
	{ 46,   136},  // 1 D5
	{ 46,   46},  // 0 D6
	{ 46,   46},  // 0 D7
	// Data code  C4h
	{ 46,   136},  // 1 D0
	{ 46,   136},  // 1 D1
	{ 46,   46},  // 0 D2
	{ 46,   46},  // 0 D3
	{ 46,   46},  // 0 D4
	{ 46,   136},  // 1 D5
	{ 46,   46},  // 0 D6
	{ 46,   46},  // 0 D7
	// { 59, 4117}
	{ 46, 0}  // end of code
}
};
// Code 002 -- Panasonic ceiling light remote controller HK9337 substitution
const struct powercode downCode PROGMEM = {// // Changed to M50560 DOWN code
	freq_to_timerval(36370), // 36.37 KHz
	// {{352,  181},  // Leader code
	{{372,  191},  // Leader code adjusted
	// Custom code 34h 4Ah 90h
	{ 46,   46}, // 0 C0
	{ 46,   46}, // 0 C1
	{ 46,   136}, // 1 C2
	{ 46,   136}, // 1 C3
	{ 46,   46}, // 0 C4
	{ 46,   136}, // 1 C5
	{ 46,   46}, // 0 C6
	{ 46,   46}, // 0 C7
	// Custom code 4Ah
	{ 46,   46}, // 0 C0
	{ 46,   136}, // 1 C1
	{ 46,   46}, // 0 C2
	{ 46,   46}, // 0 C3
	{ 46,   136}, // 1 C1
	{ 46,   46}, // 0 C5
	{ 46,   136}, // 1 C1
	{ 46,   46}, // 0 C7
	
	// Custom code 90h
	{ 46,   136}, // 1 C0
	{ 46,   46}, // 0 C1
	{ 46,   46}, // 0 C2
	{ 46,   136}, // 1 C3
	{ 46,   46}, // 0 C4
	{ 46,   46}, // 0 C5
	{ 46,   46}, // 0 C6
	{ 46,   46}, // 0 C7
	
	// Data code  D4h 44h
	{ 46,   136},  // 1 D0
	{ 46,   136},  // 1 D1
	{ 46,   46},   // 0 D2
	{ 46,   136},  // 1 D3
	{ 46,   46},   // 0 D4
	{ 46,   136},  // 1 D5
	{ 46,   46},   // 0 D6
	{ 46,   46},   // 0 D7
	// Data code  44h
	{ 46,   46},  // 0 D0
	{ 46,   136},  // 1 D1
	{ 46,   46},  // 0 D2
	{ 46,   46},  // 0 D3
	{ 46,   46},  // 0 D4
	{ 46,   136},  // 1 D5
	{ 46,   46},  // 0 D6
	{ 46,   46},  // 0 D7
	// { 59, 4117}ir
	{ 46, 0}  // end of code
}
};
// const PGM_P const *irCodes[] PROGMEM  = {
const struct powercode * const irCodes[] PROGMEM  = {
	&modeCode, &upCode, &downCode
};
//uint8_t num_codes = (sizeof(irCodes)/sizeof(*irCodes));
uint8_t num_codes = (sizeof(irCodes)/sizeof(*(irCodes)));

