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

struct ma_struct_s {uint8_t connection;
       uint16_t characteristic;
       size_t value_len;
       const uint8_t* value;
       };

int get_bt_temperature(void);
void temp_timer_callback(sl_simple_timer_t* timer, void *data);

#endif /* TEMPERATURE_H_ */
