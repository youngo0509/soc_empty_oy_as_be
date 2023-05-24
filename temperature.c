/*
 * temperature.c
 *
 *  Created on: 24 mai 2023
 *      Author: oy050
 */
#include "temperature.h"

  uint32_t rh;
  int32_t t;

int get_bt_temperature(){
  //1. declarer vos variables
  int bl_temp;
  // 2. passer ces variables par pointeur

  sl_sensor_rht_get(&rh, &t);
  bl_temp= t/10;
  return (bl_temp);
}
