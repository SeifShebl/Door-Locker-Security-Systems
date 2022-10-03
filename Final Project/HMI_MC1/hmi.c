#define F_CPU 8000000
#include "uart.h"
#include "keypad.h"
#include "lcd.h"
#include "Timer.h"
#include <util/delay.h>
#include "gpio.h"

#define PASS_SIZE 5
#define RECEIVE_PASS 0x01
#define CHECK_PASS 0x08
#define OPEN_DOOR 0x12
#define CORRECT_PASS 0x03
#define WRONG_PASS 0x10
#define BUZZER_ON 0x06
#define ATTEMPTS_BEFORE_BUZZING 3

/***********************GLOBAL VARIABLES*******************************/

uint8 Pass[PASS_SIZE]; //array to save password first time
uint8 PassConfirm[PASS_SIZE]; //array to confirm password
uint8 PassPrev[PASS_SIZE]; //array to receive previous pass from user
uint8 check = 0; //to make sure that the 2 arrays are the same
uint8 Buzzer_Flag=0; //flag to trigger buzzer after 3 wrong attempts
/*****************FUNCTIONS DEFINITIONS***********************************/

/* Description: Initialization of LCD, UART,Timer0 */
void init(void) {

	LCD_init();

	UART_Config config = { DISABLED, ONE, EIGHT, 9600 };
	UART_init(&config);

	Timer_config confg = { NORMAL, F1024, 0, 0 };
	Timer0_init(&confg);
}
/* Description: This Function is used to tell the user to enter password
 * it makes sure that the user enters the same password twice
 * it sends the verified password the other ECU through UART
 * while(1) is used to stay in this function until the user
 * enters the same password twice
 */
void Enter_Pass(void) {
	uint8 i;
	while (1) {

		LCD_displayStringRowColumn(0, 0, "Please Enter Pass:");
		LCD_moveCursor(1, 0);
		for (i = 0; i < PASS_SIZE; i++) {
			//enter password for the first time
			Pass[i] = KEYPAD_getPressedKey();
			LCD_displayCharacter('*');
			_delay_ms(500);
		}
		LCD_clearScreen();
		LCD_displayStringRowColumn(0, 0, "Confirm your Pass:");
		LCD_moveCursor(1, 0);
		for (i = 0; i < PASS_SIZE; i++) {
			//confirm password
			PassConfirm[i] = KEYPAD_getPressedKey();
			LCD_displayCharacter('*');
			_delay_ms(500);
		}
		for (i = 0; i < PASS_SIZE; i++) {
			//check that the two passwords are the same
			if (Pass[i] == PassConfirm[i]) {
				check++;
			}
		}
		LCD_clearScreen();
		if (check == 5) { //check = 5 means that the two passwords are the same
			UART_sendByte(RECEIVE_PASS); //send signal to the other ECU to receive pass
			check = 0; //reset variable that makes sure that the 2 passwords are the same

			for (i = 0; i < PASS_SIZE; i++) {
				UART_sendByte(Pass[i]);
				_delay_ms(10);
			}
			break; //go out of this function only if the 2 password matches
		}
		check = 0; //reset variable that makes sure that the 2 passwords are the same
		LCD_clearScreen();
		LCD_displayString("Invalid,Try Again");
		_delay_ms(1000);
		LCD_clearScreen();
	}
}

/* Description: This function sends to the other ECU to enable buzzer
 * when the user enters password wrong 3 times
 */
void buzzer_handling(void) {
	Buzzer_Flag++;
	if (Buzzer_Flag == ATTEMPTS_BEFORE_BUZZING) {
		UART_sendByte(BUZZER_ON);
		Buzzer_Flag = 0;
	}
}


/*Description: This function check pass entered by the user according
 * to the password saved in the EEPROM
 */
uint8 check_pass(void) {
	uint8 key; //variable to receive signal from the other ECU via UART
	uint8 i;
	LCD_clearScreen();
	LCD_displayString("Enter previous pass:");
	LCD_moveCursor(1, 0);
	for (i = 0; i < PASS_SIZE; i++) {
		PassPrev[i] = KEYPAD_getPressedKey();
		LCD_intgerToString(PassPrev[i]);
		_delay_ms(500);
	}
	/* send signal to the ECU to check pass with the one saved in the EEPROM
	 * and send the password to check to the other ECU via UART
	 */
	UART_sendByte(CHECK_PASS);
	for (i = 0; i < PASS_SIZE; i++) {
		UART_sendByte(PassPrev[i]);
		_delay_ms(10);
	}
	/* receive whether the password is correct or wrong*/
	key = UART_recieveByte();
	return (key);
}

/* Description: this function is used to change password
 * it asks the user to enter the old password, it checks it
 * with the one in the EEPROM, if it's correct it allows user
 * to change password , if it's wrong for 3 times, it sends
 * signal to trigger Buzzer
 */
void change_pass(void) {
	uint8 flag=0; //flag to get out of the for loop if the pass is correct
	uint8 key; //to receive whether the password entered is right or wrong
for (uint8 i=0;i<ATTEMPTS_BEFORE_BUZZING;i++){
	key=check_pass();
	_delay_ms(100);
	switch (key) {
	case CORRECT_PASS:
		Buzzer_Flag=0;
		flag=1; //to get out of the for loop
		LCD_clearScreen();
		Enter_Pass(); //enable to the user to enter new pass
		break;
	case WRONG_PASS:
		buzzer_handling();

		break;
	}
	if (flag==1){
		//correct password so get out of the for loop
		flag=0;
		break;
	}

}
}

/* Description: Function that handles when the user wants to open the door
 * it displays when the door is opening and closing and it checks the
 * password entered before this
 */

void handle_door(void) {
	uint8 flag = 0; //variable to get out of the for loop when password is correct
	uint8 checker=0;  // to receive whether correct pass or incorrect
	for (uint8 i = 0; i < ATTEMPTS_BEFORE_BUZZING; i++) {

		checker= check_pass();

		switch (checker) {
		case CORRECT_PASS:
			Buzzer_Flag = 0;
			flag = 1;
			UART_sendByte(OPEN_DOOR);
			LCD_clearScreen();
			LCD_displayString("Door Opening");
			Timer0_Delay(15);
			LCD_clearScreen();
			_delay_ms(500);
			LCD_displayString("Door Closing");
			Timer0_Delay(15);
			LCD_clearScreen();
			break;
		case WRONG_PASS:
			LCD_clearScreen();

			_delay_ms(500);
			buzzer_handling();

			break;
		}

		if (flag == 1) {
			flag = 0;
			break;
		}
	}
}

/* Description: This Function gives options to the user to open door or
 * to change pass , and it receives the answer from the user, and it
 * processes according to this answer
 */
void options(void) {
	LCD_clearScreen();
	LCD_displayStringRowColumn(0, 0, "+ : Open Door");
	LCD_displayStringRowColumn(1, 0, "-: Change Pass");

	switch (KEYPAD_getPressedKey()) {

	case '-': //change password
		_delay_ms(500);
		change_pass();
		break;
	case '+': //open door
		_delay_ms(500);
		handle_door();
		break;
	}

}


int main(void) {
	init();
	_delay_ms(50);
	Enter_Pass();
	while (1) {

		options();

	}

}
