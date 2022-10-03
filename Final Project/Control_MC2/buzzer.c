
#include "buzzer.h"
#include "gpio.h"

/*******************************************************************************
 *                      Functions Definitions                                  *
 *******************************************************************************/

void Buzzer_init(void){
	GPIO_setupPinDirection(PORTA_ID,PIN0_ID,PIN_OUTPUT);
	GPIO_writePin(BUZZER_PORT,BUZZER_PIN,LOGIC_LOW);
}

void Buzzer_on(void){ //function to turn on the buzzer
	GPIO_writePin(BUZZER_PORT,BUZZER_PIN,LOGIC_HIGH);

}

void Buzzer_off(void){ //function to turn off the buzzer
	GPIO_writePin(BUZZER_PORT,BUZZER_PIN,LOGIC_LOW);

}
