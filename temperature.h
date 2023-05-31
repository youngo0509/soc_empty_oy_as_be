/*
 * temperature.h
 *
 *  Created on: 24 mai 2023
 *      Author: oy050
 */

#ifndef TEMPERATURE_H_
#define TEMPERATURE_H_

#include "sl_sensor_rht.h"
#include <stdint.h>
#include "sl_status.h"
#include "sl_simple_timer.h"


int get_bt_temperature(void);
void temp_timer_callback(sl_simple_timer_t* timer, void *data);

#endif /* TEMPERATURE_H_ */
