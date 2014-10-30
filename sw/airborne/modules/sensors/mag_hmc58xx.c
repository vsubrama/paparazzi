/*
 * Copyright (C) 2013 Felix Ruess <felix.ruess@gmail.com>
 *
 * This file is part of paparazzi.
 *
 * paparazzi is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * paparazzi is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with paparazzi; see the file COPYING.  If not, write to
 * the Free Software Foundation, 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

/**
 * @file modules/sensors/mag_hmc58xx.c
 *
 * Module wrapper for Honeywell HMC5843 and HMC5883 magnetometers.
 */

#include "modules/sensors/mag_hmc58xx.h"
#include "mcu_periph/uart.h"
#include "messages.h"
#include "subsystems/datalink/downlink.h"

#if MODULE_HMC58XX_UPDATE_AHRS
#include "subsystems/imu.h"
#include "subsystems/ahrs.h"

#ifndef HMC58XX_CHAN_X
#define HMC58XX_CHAN_X 0
#endif
#ifndef HMC58XX_CHAN_Y
#define HMC58XX_CHAN_Y 1
#endif
#ifndef HMC58XX_CHAN_Z
#define HMC58XX_CHAN_Z 2
#endif

#endif

struct Hmc58xx mag_hmc58xx;

void mag_hmc58xx_module_init(void) {
  hmc58xx_init(&mag_hmc58xx, &(MAG_HMC58XX_I2C_DEV), HMC58XX_ADDR);
}

void mag_hmc58xx_module_periodic(void) {
  hmc58xx_periodic(&mag_hmc58xx);
}

void mag_hmc58xx_module_event(void) {
#if USE_AUTO_AHRS_FREQ || !defined(AHRS_MAG_CORRECT_FREQUENCY)
PRINT_CONFIG_MSG("Calculating dt for AHRS mag update.")
  // timestamp in usec when last callback was received
  static uint32_t last_ts = 0;
#else
PRINT_CONFIG_MSG("Using fixed AHRS_MAG_CORRECT_FREQUENCY for AHRS mag update.")
PRINT_CONFIG_VAR(AHRS_MAG_CORRECT_FREQUENCY)
  const float dt = 1. / (AHRS_MAG_CORRECT_FREQUENCY);
#endif

  hmc58xx_event(&mag_hmc58xx);

#if MODULE_HMC58XX_UPDATE_AHRS
  if (mag_hmc58xx.data_available) {
    // set channel order
    struct Int32Vect3 mag = {
      (int32_t)(mag_hmc58xx.data.value[HMC58XX_CHAN_X]),
      (int32_t)(mag_hmc58xx.data.value[HMC58XX_CHAN_Y]),
      (int32_t)(mag_hmc58xx.data.value[HMC58XX_CHAN_Z])
    };
    // unscaled vector
    VECT3_COPY(imu.mag_unscaled, mag);
    // scale vector
    imu_scale_mag(&imu);
    // update ahrs
    if (ahrs.status == AHRS_RUNNING) {
#if USE_AUTO_AHRS_FREQ || !defined(AHRS_MAG_CORRECT_FREQUENCY)
      // current timestamp
      uint32_t now_ts = get_sys_time_usec();
      // dt between this and last callback in seconds
      float dt = (float)(now_ts - last_ts) / 1e6;
      last_ts = now_ts;
#endif
      ahrs_update_mag(dt);
    }
    mag_hmc58xx.data_available = FALSE;
  }
#endif
#if MODULE_HMC58XX_SYNC_SEND
  if (mag_hmc58xx.data_available) {
    mag_hmc58xx_report();
    mag_hmc58xx.data_available = FALSE;
  }
#endif
}

void mag_hmc58xx_report(void) {
  struct Int32Vect3 mag = {
    (int32_t)(mag_hmc58xx.data.vect.x),
    (int32_t)(mag_hmc58xx.data.vect.y),
    (int32_t)(mag_hmc58xx.data.vect.z)
  };
  DOWNLINK_SEND_IMU_MAG_RAW(DefaultChannel, DefaultDevice, &mag.x, &mag.y, &mag.z);
}
