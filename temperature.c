/*
 * temperature.c
 *
 *  Created on: 24 mai 2023
 *      Author: oy050
 */
#include "temperature.h"
#include "sl_simple_timer.h"
#include "app_log.h"


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

void temp_timer_callback(sl_simple_timer_t *timer, void *data){
  int *pc = data;
   *pc = *pc +1;
   app_log_info("Timer timeout %ld Step %d \n", timer->timeout_ms,  *pc);

}
