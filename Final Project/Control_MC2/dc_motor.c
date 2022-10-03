

#include "dc_motor.h"
#include "gpio.h"
#include <avr/io.h>



/*         FUNCTIONS DEFINITIONS           */

void DcMotor_Init(void){
	GPIO_setupPinDirection(DC_PORT,DC_IN1,PIN_OUTPUT); //configure PA2 as Output
	GPIO_setupPinDirection(DC_PORT,DC_IN2,PIN_OUTPUT); //configure PA3 as Output

	GPIO_writePin(DC_PORT, DC_IN1, LOGIC_LOW);
	GPIO_writePin(DC_PORT, DC_IN2, LOGIC_LOW);


}


void DcMotor_Rotate(DcMotor_State state) {
	switch (state) {
	case CW:
		GPIO_writePin(DC_PORT, DC_IN1, LOGIC_LOW);
		GPIO_writePin(DC_PORT, DC_IN2, LOGIC_HIGH);
		break;
	case ACW:
		GPIO_writePin(DC_PORT, DC_IN1, LOGIC_HIGH);
		GPIO_writePin(DC_PORT, DC_IN2, LOGIC_LOW);
		break;
	case STOP:
		GPIO_writePin(DC_PORT, DC_IN1, LOGIC_LOW);
		GPIO_writePin(DC_PORT, DC_IN2, LOGIC_LOW);
		break;
	};
	//I removed PWM because we don't need speed or duty cycle
}
