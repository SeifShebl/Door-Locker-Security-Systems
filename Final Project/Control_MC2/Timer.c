#include "Timer.h"
#include "common_macros.h" /* To use the macros like SET_BIT */
#include <avr/io.h> /* To use ICU/Timer1 Registers */
#include <avr/interrupt.h> /* For ICU ISR */

/**********************GLOBAL VARIABLES*******************************/
static volatile void (*g_callBackPtr)(void) = NULL_PTR;

/***********************INTERRUPT SERVICE ROUTINE***********************/
ISR(TIMER0_OVF) {
	if (g_callBackPtr != NULL_PTR) {
		/* Call the Call Back function  */
		(*g_callBackPtr)();
	}
}

ISR(TIMER0_COMP) {
	if (g_callBackPtr != NULL_PTR) {
		/* Call the Call Back function */
		(*g_callBackPtr)();
	}
}

/**********************FUNCTIONS DECLARATION***************************/

void Timer0_init(const Timer_config *Config_Ptr) {
	/*************Timer0 Initialization**************/

	if (Config_Ptr->mode == NORMAL || Config_Ptr->mode == CTC) {
		SET_BIT(TCCR0, FOC0);
	} else {
		CLEAR_BIT(TCCR0, FOC0);
	}
	/* Choose the mode according to the config struct*/
	TCCR0 = (TCCR0 & 0xF7) | (((Config_Ptr->mode) & 0b10) << 2);
	TCCR0 = (TCCR0 & 0xBF) | (((Config_Ptr->mode) & 0b01) << 6);

	/* Choose the prescaler according to the config struct */
	TCCR0 = (TCCR0 & 0xF8) | (Config_Ptr->prescaler);

	/* Select the initial value*/
	TCNT0 = Config_Ptr->Initial_value;

	/* Select compare value*/
	OCR0 = Config_Ptr->Compare_value;

	/* enable interrupt based on the mode */
	if (Config_Ptr->mode == NORMAL) {
		SET_BIT(TIMSK, TOIE0);
	} else if (Config_Ptr->mode == CTC) {
		SET_BIT(TIMSK, OCIE0);

	}
}

void Timer0_DeInit(void) {
	/***********Timer0 Deinitialization**********/

	TCCR0 = 0;
	TCNT0 = 0;
	OCR0 = 0;

	TIMSK &= 0xFC;

}

void Timer0_Delay(uint8 seconds) {
	uint8 count;
	TCNT0 = 0; // Set Timer0 initial value to 0
	OCR0 = 245;

	/* Configure the timer control register
	 * 1. Non PWM mode FOC0=1
	 * 2. Comp Mode WGM01=1 & WGM00=0
	 * 3. clock = F_CPU/1024 CS00=1 CS01=0 CS02=1
	 */
	TCCR0 = (1 << FOC0) | (1 << WGM01) | (1 << CS02) | (1 << CS00);

	for (int i = 0; i < seconds; i++) {

		for (count = 0; count < 32; count++) { // to stay for 1 second

			while (!(TIFR & (1 << OCF0)))
				; // Wait until the Timer0 Overflow occurs (wait until TOV0 = 1)

			TIFR |= (1 << OCF0); // Clear TOV0 bit by set its value

		}
	}
}

void Timer0_setCallBack(void (*a_ptr)(void)) {
	/* Save the address of the Call back function in a global variable */
	g_callBackPtr = a_ptr;
}




