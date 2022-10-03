#define F_CPU 8000000

#include "buzzer.h"
#include "util/delay.h"
#include "uart.h"
#include "twi.h"
#include "external_eeprom.h"
#include "dc_motor.h"
#include "Timer.h"

#define PASS_SIZE 5
#define RECEIVE_PASS 0x01
#define CHECK_PASS 0x08
#define OPEN_DOOR 0x12
#define CORRECT_PASS 0x03
#define WRONG_PASS 0x10
#define BUZZER_ON 0x06

/*****************GLOBAL VARIABLES*************************/

uint8 Pass_To_Save[PASS_SIZE];
uint8 Pass_Recieved[PASS_SIZE];
uint8 Pass_From_EEPROM[PASS_SIZE];

/* Description:  Initialization of motor,buzzer,I2C, UART,Timer0 */

void init(void) {
	/* Description: Initialization of LCD, UART,Timer0 */
	UART_Config config= { DISABLED, ONE, EIGHT, 9600 };
	UART_init(&config);

	I2C_Config configr = { 400000, 0x01 };
	TWI_init(&configr);

	DcMotor_Init();

	Timer_config configur={NORMAL,F1024,0,0};
	Timer0_init(&configur);

	Buzzer_init();
}

/* Description: This function receives pass and saves it in the EEPROM */
void receive_pass(void) {
	uint8 i;
	for (i = 0; i < PASS_SIZE; i++) {
		Pass_To_Save[i] = UART_recieveByte();
		_delay_ms(10);
	}
	for (i = 0; i < PASS_SIZE; i++) {
		EEPROM_writeByte(i, Pass_To_Save[i]);
		_delay_ms(10);
	}
}
/* Description: this function checks the password received from
 * the first ECU with the one saved in the EEPROM and sends
 * signal to the first ECU whether it's correct or
 * incorrect
 */
	void check_pass(void) {
		uint8 i;
		uint8 check=0;

		for (i = 0; i < PASS_SIZE; i++) {
			Pass_Recieved[i] = UART_recieveByte();
			_delay_ms(10);
		}
		for (i = 0; i < PASS_SIZE; i++) {
			EEPROM_readByte(i, &Pass_From_EEPROM[i]);
			_delay_ms(10);
		}
		for (i = 0; i < PASS_SIZE; i++){
			if (Pass_Recieved[i]==Pass_From_EEPROM[i]){
				check++;
			}

			}
		if (check==5) {
			UART_sendByte(CORRECT_PASS);

		}
		else {
			UART_sendByte(WRONG_PASS);
		}
		check=0;
		}

/* Description: this function rotates motor for 15 seconds clock wise
 * to open the door then rotates it for 15 seconds anti clock wise
 * to close the door
 */
void rotate(void){

	DcMotor_Rotate(CW);
	Timer0_Delay(15);

	DcMotor_Rotate(ACW);
	Timer0_Delay(15);
	DcMotor_Rotate(STOP);
	Timer0_Delay(2);


}


int main(void){
	init();
	_delay_ms(50);
	while(1){
		switch (UART_recieveByte()){

		case RECEIVE_PASS:
			receive_pass();
			break;

		case CHECK_PASS:
			check_pass();
			break;

		case OPEN_DOOR:
			rotate();
			break;

		case BUZZER_ON:
			Buzzer_on();
			Timer0_Delay(60);
			Buzzer_off();
		}

	}

}
