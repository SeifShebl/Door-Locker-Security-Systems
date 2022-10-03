


#ifndef TIMER_H_
#define TIMER_H_

#include "std_types.h"

/******************** TYPES DECLARATION ******************************************/

typedef enum{
	NORMAL , PWM,CTC,FAST_PWM
} Timer_Mode;

typedef enum {
	NO_CLOCK,NO_PRESCALER, F8,F64,F256,
	F1024,EXTERNAL_FALLING,EXTERNAL_RISING
} Timer_Prescaler;


typedef struct {
	Timer_Mode mode;
	Timer_Prescaler prescaler;
	uint8 Initial_value;
	uint16 Compare_value;

} Timer_config;






/************************* FUNCTIONS PROTOTYPES************************************/
void Timer0_init(const Timer_config *Config_Ptr  );

void Timer0_DeInit(void);

void Timer0_Delay(uint8 seconds);

void Timer0_setCallBack(void(*a_ptr)(void));



#endif /* TIMER_H_ */
