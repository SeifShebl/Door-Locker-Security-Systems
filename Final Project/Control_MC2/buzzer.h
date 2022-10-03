/*
 * buzzer.h
 *
 *  Created on: Mar 20, 2022
 *      Author: hends
 */

#ifndef BUZZER_H_
#define BUZZER_H_

#include "std_types.h"
/********************DEFINITIONS*********************************************/
#define BUZZER_PORT PORTA_ID

#define BUZZER_PIN PIN0_ID

/*******************FUNCTIONS PROTOTYPES******************************************/

void Buzzer_init(void);

void Buzzer_on(void);

void Buzzer_off (void);

#endif /* BUZZER_H_ */
