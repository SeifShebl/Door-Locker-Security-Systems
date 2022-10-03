


#ifndef DC_MOTOR_H_
#define DC_MOTOR_H_

#include "std_types.h"

void Timer0_PWM_Init(unsigned char set_duty_cycle);


#define DC_PORT               PORTA_ID
#define DC_IN1                PIN2_ID
#define DC_IN2                PIN3_ID



typedef enum{
	STOP, CW,ACW
}DcMotor_State;


/*    FUNCTION PROTOTYPES  */
void DcMotor_Init(void);

void DcMotor_Rotate(DcMotor_State state);




#endif /* DC_MOTOR_H_ */
