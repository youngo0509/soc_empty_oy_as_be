/***************************************************************************//**
 * @file
 * @brief Core application logic.
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * SPDX-License-Identifier: Zlib
 *
 * The licensor of this software is Silicon Laboratories Inc.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 ******************************************************************************/
#include "em_common.h"
#include "app_assert.h"
#include "sl_bluetooth.h"
#include "app.h"
#include "app_log.h"
#include "sl_sensor_rht.h"
#include "temperature.h"
#include "gatt_db.h"


// The advertising set handle allocated from Bluetooth stack.
static uint8_t advertising_set_handle = 0xff;

//Create timer
sl_simple_timer_t timer;


/**************************************************************************//**
 * Application Init.
 *****************************************************************************/
SL_WEAK void app_init(void)
{
  /////////////////////////////////////////////////////////////////////////////
  // Put your additional application init code here!                         //
  // This is called once during start-up.
  /////////////////////////////////////////////////////////////////////////////
  app_log_info("%s\n",__FUNCTION__);


}

/************************************************************;**************//**
 * Application Process Action.
 *****************************************************************************/
SL_WEAK void app_process_action(void)
{
  /////////////////////////////////////////////////////////////////////////////
  // Put your additional application code here!                              //
  // This is called infinitely.                                              //
  // Do not call blocking functions from here!                               //
  /////////////////////////////////////////////////////////////////////////////
}

/**************************************************************************//**
 * Bluetooth stack event handler.
 * This overrides the dummy weak implementation.
 *
 * @param[in] evt Event coming from the Bluetooth stack.
 *****************************************************************************/
void sl_bt_on_event(sl_bt_msg_t *evt)
{
  sl_status_t sc;

  switch (SL_BT_MSG_ID(evt->header)) {
    // -------------------------------
    // This event indicates the device has started and the radio is ready.
    // Do not call any stack command before receiving this boot event!
    case sl_bt_evt_system_boot_id:
      // Create an advertising set.
      sc = sl_bt_advertiser_create_set(&advertising_set_handle);
      app_assert_status(sc);

      // Generate data for advertising
      sc = sl_bt_legacy_advertiser_generate_data(advertising_set_handle,
                                                 sl_bt_advertiser_general_discoverable);
      app_assert_status(sc);

      // Set advertising interval to 100ms.
      sc = sl_bt_advertiser_set_timing(
        advertising_set_handle,
        160, // min. adv. interval (milliseconds * 1.6)
        160, // max. adv. interval (milliseconds * 1.6)
        0,   // adv. duration
        0);  // max. num. adv. events
      app_assert_status(sc);
      // Start advertising and enable connections.
      sc = sl_bt_legacy_advertiser_start(advertising_set_handle,
                                         sl_bt_advertiser_connectable_scannable);
      app_assert_status(sc);
      break;

    // -------------------------------
    // This event indicates that a new connection was opened.
    case sl_bt_evt_connection_opened_id:
      app_log_info("%s: connection opened! \n", __FUNCTION__);

      //Initialisation of temperature and humidity sensor
      sl_sensor_rht_init();
      app_log_info("%s: Relative temperature and Humidity Sensor initialised! \n",__FUNCTION__);

      int temperature = get_bt_temperature();
      app_log_info("Relative temperature : %d \n",temperature);

      break;

    // -------------------------------
    // This event indicates that a connection was closed.
    case sl_bt_evt_connection_closed_id:
      app_log_info("%s: connection closed! \n", __FUNCTION__);

      //Deinitialisation of temperature and humidity sensor
      sl_sensor_rht_init();
      app_log_info("%s: Relative temperature and Humidity Sensor deinitialised! \n",__FUNCTION__);

      // Generate data for advertising
      sc = sl_bt_legacy_advertiser_generate_data(advertising_set_handle,
                                                 sl_bt_advertiser_general_discoverable);
      app_assert_status(sc);

      // Restart advertising after client has disconnected.
      sc = sl_bt_legacy_advertiser_start(advertising_set_handle,
                                         sl_bt_advertiser_connectable_scannable);
      app_assert_status(sc);
      break;

     // This event triggers when characteristic is read.
    case sl_bt_evt_gatt_server_user_read_request_id:
      app_log_info("%s: Read request! \n", __FUNCTION__);
      if (evt->data.evt_gatt_server_user_read_request.characteristic==gattdb_temperature)
        {
          app_log_info("%s: Temperature Read request! \n", __FUNCTION__);
          uint8_t connection = evt->data.evt_gatt_server_user_read_request.connection;
          uint8_t temperature = get_bt_temperature();
          uint16_t sent_len;
          sc = sl_bt_gatt_server_send_user_read_response(connection,
                                                                gattdb_temperature,
                                                                0,
                                                                2,
                                                                &temperature,
                                                                &sent_len);
      }
      break;

    ///////////////////////////////////////////////////////////////////////////
    // Add additional event handlers here as your application requires!      //
    ///////////////////////////////////////////////////////////////////////////
      // This event triggers when Client Characteristic Configuration Descriptor is modified.
    case  sl_bt_evt_gatt_server_characteristic_status_id:

      app_log_info("%s: CCCD modification request! \n", __FUNCTION__);

      if (evt->data.evt_gatt_server_characteristic_status.characteristic==gattdb_temperature)
             {
               app_log_info("%s: Temperature Notification request! \n", __FUNCTION__);
             }
      if (evt->data.evt_gatt_server_characteristic_status.status_flags==0x1)
        {
          app_log_info("%s: Characteristic client configuration has been changed! \n", __FUNCTION__);
        }
      uint16_t flag = evt->data.evt_gatt_server_characteristic_status.client_config_flags;
      app_log_info("flag's value : %d \n",flag);
      if (flag==1){
          int count=0;
          sc =  sl_simple_timer_start(&timer,
                                     1000,
                                     temp_timer_callback,
                                     &count,
                                     true);
      } else {
          sl_simple_timer_stop(&timer);
      }
      break;

    // -------------------------------
    // Default event handler.
    default:
      break;
  }

}
