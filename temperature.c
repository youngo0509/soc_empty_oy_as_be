/*
 * temperature.c
 *
 *  Created on: 24 mai 2023
 *      Author: oy050
 */
#include "temperature.h"
#include "sl_simple_timer.h"
#include "app_log.h"
#include "sl_bt_api.h"

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
  struct ma_struct_s * pc = data;

  timer = timer;
  uint16_t value= get_bt_temperature();
  size_t value_len = sizeof(value);
  sl_status_t sc = sl_bt_gatt_server_send_notification(pc->connection,
                                           pc->characteristic,
                                           value_len,
                                           (const uint8_t*)&value);

  if (sc != SL_STATUS_OK) {
      app_log_info("%s : error %ld", __FUNCTION__, sc);
  }

}
