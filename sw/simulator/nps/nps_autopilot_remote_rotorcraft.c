/*
 * Copyright (C) 2015 Vasanth Subramanian <vsubrama@buffalo.edu>
 */

#include "nps_autopilot.h"

#include "firmwares/rotorcraft/main.h"
#include "nps_sensors.h"
#include "nps_radio_control.h"
#include "nps_electrical.h"
#include "nps_fdm.h"

#include "subsystems/radio_control.h"
#include "subsystems/imu.h"
#include "mcu_periph/sys_time.h"
#include "state.h"
#include "subsystems/ahrs.h"
#include "subsystems/ins.h"
#include "math/pprz_algebra.h"

#include "subsystems/actuators/motor_mixing.h"

#include "subsystems/abi.h"

// For auto pilot communication
#include <stdlib.h>
#include <Ivy/ivy.h>
#include <Ivy/ivyglibloop.h>
//For socket connection
#include<sys/socket.h>    //socket
#include<arpa/inet.h> //inet_addr

#include "messages.h"
#include "subsystems/datalink/downlink.h"

struct NpsAutopilot autopilot;
bool_t nps_bypass_ahrs;
bool_t nps_bypass_ins;

#ifndef NPS_BYPASS_AHRS
#define NPS_BYPASS_AHRS TRUE
#endif

#ifndef NPS_BYPASS_INS
#define NPS_BYPASS_INS FALSE
#endif

#if NPS_COMMANDS_NB != MOTOR_MIXING_NB_MOTOR
#error "NPS_COMMANDS_NB does not match MOTOR_MIXING_NB_MOTOR!"
#endif
//Code for socket message send
int sock;
struct sockaddr_in server;
char message[1000] , server_reply[2000];
//Code for socket message send

void nps_autopilot_init(enum NpsRadioControlType type_rc, int num_rc_script,
	 char* rc_dev) {

  autopilot.launch = TRUE;

  nps_radio_control_init(type_rc, num_rc_script, rc_dev);
  nps_electrical_init();

  nps_bypass_ahrs = NPS_BYPASS_AHRS;
  nps_bypass_ins = NPS_BYPASS_INS;
sock = socket(AF_INET , SOCK_STREAM , 0);
	    if (sock == -1)
	    {
	        printf("Could not create socket");
	    }
	    puts("Socket created");

	    server.sin_addr.s_addr = inet_addr("127.0.0.1");
	    server.sin_family = AF_INET;
	    server.sin_port = htons( 8080 );

        //Connect to remote server commented temporarily
//	    if (connect(sock , (struct sockaddr *)&server , sizeof(server)) < 0)
//	    {
//	        perror("connect failed. Error");
//	    }
  main_init();

}

void nps_autopilot_run_systime_step( void ) {
  sys_tick_handler();
}

#include <stdio.h>
#include "subsystems/gps.h"

void nps_autopilot_run_step(double time) {

  //nps_electrical_run_step(time);

#ifdef RADIO_CONTROL_TYPE_PPM
  if (nps_radio_control_available(time)) {
    radio_control_feed();
    main_event();
  }
#endif

  if (nps_sensors_gyro_available()) {
    imu_feed_gyro_accel();
	//Feed the data from the gyroscope into the serial_pap
		IvySendMsg("NPS_SENSORS_GYRO %d %d %f %f %f", 6, AC_ID,
				sensors.gyro.value.x, sensors.gyro.value.y,
				sensors.gyro.value.z);

        printf("NPS_SENSORS_GYRO %d %d %f %f %f\n", 6, AC_ID,
                sensors.gyro.value.x, sensors.gyro.value.y,
                sensors.gyro.value.z);

		//Feed the data from the accelerometer into the serial_pap
		IvySendMsg("NPS_SENSORS_ACCEL %d %d %f %f %f", 6, AC_ID,
				sensors.accel.value.x, sensors.accel.value.y,
				sensors.accel.value.z);

        printf("NPS_SENSORS_ACCEL %d %d %f %f %f\n", 6, AC_ID,
                sensors.accel.value.x, sensors.accel.value.y,
                sensors.accel.value.z);
    main_event();
  }

//  if (nps_sensors_mag_available()) {
//    imu_feed_mag();
//    main_event();
//  }

//  if (nps_sensors_baro_available()) {
//    float pressure = (float) sensors.baro.value;
//    AbiSendMsgBARO_ABS(BARO_SIM_SENDER_ID, &pressure);
//    main_event();
//  }

#if USE_SONAR
  if (nps_sensors_sonar_available()) {
    float dist = (float) sensors.sonar.value;
    AbiSendMsgAGL(AGL_SONAR_NPS_ID, &dist);

    uint16_t foo = 0;
    DOWNLINK_SEND_SONAR(DefaultChannel, DefaultDevice, &foo, &dist);

    main_event();
  }
#endif

  if (nps_sensors_gps_available()) {
    gps_feed_value();
    double vals[7];
		 nps_fdm_remote_position(&vals[0]);
		 IvySendMsg("NPS_SEN_NICE_GPS %d %d %f %f %f %f %f %f %f", 6, AC_ID,
		 vals[0], vals[1], vals[2], vals[3], vals[4], vals[5], vals[6]);
//Debug
printf("NPS_SEN_NICE_GPS %d %d %f %f %f %f %f %f %f\n", 6, AC_ID,vals[0], vals[1], vals[2], vals[3], vals[4], vals[5], vals[6]);

         IvySendMsg("NPS_SEN_FDM_GPS %d %d %f %f %f %f %f %f %f", 6, AC_ID,
				fdm.lla_pos_pprz.lat, fdm.lla_pos_pprz.lon,
				fdm.lla_pos_pprz.alt, vals[3], vals[4], vals[5], fdm.time);
//Debug
printf("NPS_SEN_FDM_GPS %d %d %f %f %f %f %f %f %f\n", 6, AC_ID,fdm.lla_pos_pprz.lat, fdm.lla_pos_pprz.lon,fdm.lla_pos_pprz.alt, vals[3], vals[4], vals[5], fdm.time);
printf("NPS_AHRS_LTP %d %d %f %f %f %f\n", 6, AC_ID,fdm.ltp_to_body_quat.qi, fdm.ltp_to_body_quat.qx,fdm.ltp_to_body_quat.qy, fdm.ltp_to_body_quat.qz);
printf("NPS_AHRS_ECEF %d %d %f %f %f\n", 6, AC_ID,fdm.body_ecef_rotvel.p, fdm.body_ecef_rotvel.q,fdm.body_ecef_rotvel.r);
        //Network Send

    main_event();
  }

  if (nps_bypass_ahrs) {
    sim_overwrite_ahrs();
    IvySendMsg("NPS_AHRS_LTP %d %d %f %f %f %f\n", 6, AC_ID,
					fdm.ltp_to_body_quat.qi, fdm.ltp_to_body_quat.qx,
					fdm.ltp_to_body_quat.qy, fdm.ltp_to_body_quat.qz);
//Debug
//printf("NPS_AHRS_LTP %d %d %f %f %f %f\n", 6, AC_ID,fdm.ltp_to_body_quat.qi, fdm.ltp_to_body_quat.qx,fdm.ltp_to_body_quat.qy, fdm.ltp_to_body_quat.qz);

    IvySendMsg("NPS_AHRS_ECEF %d %d %f %f %f\n", 6, AC_ID,
					fdm.body_ecef_rotvel.p, fdm.body_ecef_rotvel.q,
					fdm.body_ecef_rotvel.r);
//Debug
//printf("NPS_AHRS_ECEF %d %d %f %f %f\n", 6, AC_ID,fdm.body_ecef_rotvel.p, fdm.body_ecef_rotvel.q,fdm.body_ecef_rotvel.r);
  }

  if (nps_bypass_ins) {
    sim_overwrite_ins();
  }

  handle_periodic_tasks();

  /* scale final motor commands to 0-1 for feeding the fdm */
  for (uint8_t i=0; i < NPS_COMMANDS_NB; i++)
    autopilot.commands[i] = (double)motor_mixing.commands[i]/MAX_PPRZ;

}


void sim_overwrite_ahrs(void) {

  struct FloatQuat quat_f;
  QUAT_COPY(quat_f, fdm.ltp_to_body_quat);
  stateSetNedToBodyQuat_f(&quat_f);

  struct FloatRates rates_f;
  RATES_COPY(rates_f, fdm.body_ecef_rotvel);
  stateSetBodyRates_f(&rates_f);

}

void sim_overwrite_ins(void) {

  struct NedCoor_f ltp_pos;
  VECT3_COPY(ltp_pos, fdm.ltpprz_pos);
  stateSetPositionNed_f(&ltp_pos);

  struct NedCoor_f ltp_speed;
  VECT3_COPY(ltp_speed, fdm.ltpprz_ecef_vel);
  stateSetSpeedNed_f(&ltp_speed);

  struct NedCoor_f ltp_accel;
  VECT3_COPY(ltp_accel, fdm.ltpprz_ecef_accel);
  stateSetAccelNed_f(&ltp_accel);

}

