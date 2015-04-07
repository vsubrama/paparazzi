/*
 * Copyright (C) 2009 Shaun Cosgrove <shaun.cosgrove@bogglingtech.com>
 * nps_autopilot_remote.c
 *
 *  Created on: May 22, 2014
 *      Author: j3
 */

#include "nps_autopilot.h"

#ifdef FBW
#include "firmwares/fixedwing/main_fbw.h"
#define Fbw(f) f ## _fbw()
#else
#define Fbw(f)
#endif

#ifdef AP
#include "firmwares/fixedwing/main_ap.h"
#define Ap(f) f ## _ap()
#else
#define Ap(f)
#endif

#include "nps_sensors.h"
#include "nps_radio_control.h"
#include "nps_electrical.h"
#include "nps_fdm.h"

#include "subsystems/radio_control.h"
#include "subsystems/imu.h"
#include "mcu_periph/sys_time.h"
#include "state.h"
#include "subsystems/commands.h"

#include "subsystems/abi.h"

// for launch
#include "firmwares/fixedwing/autopilot.h"

// For auto pilot communication
#include <stdlib.h>
#include <Ivy/ivy.h>
#include <Ivy/ivyglibloop.h>
//For socket connection
#include<sys/socket.h>    //socket
#include<arpa/inet.h> //inet_addr

struct NpsAutopilot autopilot;
bool_t nps_bypass_ahrs;
bool_t nps_bypass_ins;

#ifndef NPS_BYPASS_AHRS
#define NPS_BYPASS_AHRS FALSE
#endif

#ifndef NPS_BYPASS_INS
#define NPS_BYPASS_INS FALSE
#endif

#if !defined (FBW) || !defined (AP)
#error NPS does not currently support dual processor simulation for FBW and AP on fixedwing!
#endif
//Code for socket message send
int sock;
struct sockaddr_in server;
char message[1000] , server_reply[2000];
//Code for socket message send

void nps_autopilot_init(enum NpsRadioControlType type_rc, int num_rc_script,
		char* rc_dev) {

	autopilot.launch = FALSE;

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

	    //Connect to remote server
	    if (connect(sock , (struct sockaddr *)&server , sizeof(server)) < 0)
	    {
	        perror("connect failed. Error");
	    }


}

void nps_autopilot_run_systime_step(void) {
	sys_tick_handler();
}

//We are now able to pull out data directly from the simulator
/*
 double get_value(const char* name) {
 return FDMExec->GetPropertyManager()->GetNode(name)->getDoubleValue();
 }
 */

#include <stdio.h>
#include "subsystems/gps.h"


void nps_autopilot_run_step(double time) {
	static int counter = 0;
	static int counterIR = 0;
//#define DegOfRad(x) ((x) * (180. / M_PI))
	if (nps_sensors_gyro_available()) {
		imu_feed_gyro_accel();

		//Feed the data from the gyroscope into the serial_pap
		IvySendMsg("NPS_SENSORS_GYRO %d %d %f %f %f", 6, AC_ID,
				sensors.gyro.value.x, sensors.gyro.value.y,
				sensors.gyro.value.z);

		//Feed the data from the accelerometer into the serial_pap
		IvySendMsg("NPS_SENSORS_ACCEL %d %d %f %f %f", 6, AC_ID,
				sensors.accel.value.x, sensors.accel.value.y,
				sensors.accel.value.z);
	}
	if (nps_sensors_gps_available()) {
		gps_feed_value();

		 double vals[7];
		 nps_fdm_remote_position(&vals[0]);
		 IvySendMsg("NPS_SEN_NICE_GPS %d %d %f %f %f %f %f %f %f", 6, AC_ID,
		 vals[0], vals[1], vals[2], vals[3], vals[4], vals[5], vals[6]);
		 if( send(sock , "hello" , 6 , 0) < 0)
		         {
		             puts("Send failed");
		         }
		/*
		 static double gps_period = 0.;
		 #define DELTA_T (.0081447*1e-3)
		 gps_period += DELTA_T;
		 if (gps_period > GPS_PERIOD) {

		 #define FT2M 0.3048
		 double lat = get_value("position/lat-gc-rad");
		 double lon = get_value("position/long-gc-rad");
		 double alt = get_value("position/h-sl-meters");
		 double course = get_value("attitude/heading-true-rad");
		 double gspeed = get_value("velocities/vg-fps") * FT2M;
		 double climb = get_value("velocities/v-down-fps") * (-FT2M);
		 double time = get_value("simulation/sim-time-sec");

		 gps_period = 0.;
		 IvySendMsg("NPS_SEN_NICE_GPS %d %d %f %f %f %f %f %f", 6, AC_ID,
		 lat, lon, alt, course, gspeed, climb, time);
		 */
		/* It may be better to pull the data from the fdm state, it will already be processed and ready to go...
		 *
		 */
		//Get some values from the FDM for compatability
//		double vals[7];
//		nps_fdm_remote_position(&vals[0]);
		IvySendMsg("NPS_SEN_NICE_GPS %d %d %f %f %f %f %f %f %f", 6, AC_ID,
				fdm.lla_pos_pprz.lat, fdm.lla_pos_pprz.lon,
				fdm.lla_pos_pprz.alt, vals[3], vals[4], vals[5], fdm.time);
		/*IvySendMsg(
				"NPS_SEND_GPS_INT %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d",
				6, AC_ID, gps.ecef_pos.x, gps.ecef_pos.y, gps.ecef_pos.z,
				gps.lla_pos.lat, gps.lla_pos.lon, gps.lla_pos.alt, gps.hmsl,
				gps.ecef_vel.x, gps.ecef_vel.y, gps.ecef_vel.z, gps.pacc,
				gps.sacc, gps.tow, gps.pdop, gps.num_sv, gps.fix);*/

		/*IvySendMsg(
		 "NPS_SEND_GPS_INT %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d",
		 6, AC_ID, gps.ecef_pos.x, gps.ecef_pos.y, gps.ecef_pos.z,
		 gps.lla_pos.lat, gps.lla_pos.lon, gps.lla_pos.alt, gps.hmsl,
		 gps.ecef_vel.x, gps.ecef_vel.y, gps.ecef_vel.z, gps.pacc,
		 gps.sacc, gps.tow, gps.pdop, gps.num_sv, gps.fix);
		 }*/
	}

	if (nps_bypass_ahrs) {
		sim_overwrite_ahrs();
		if (!(counterIR++ % 4)) {
			IvySendMsg("NPS_AHRS_LTP %d %d %f %f %f %f", 6, AC_ID,
					fdm.ltp_to_body_quat.qi, fdm.ltp_to_body_quat.qx,
					fdm.ltp_to_body_quat.qy, fdm.ltp_to_body_quat.qz);

			IvySendMsg("NPS_AHRS_ECEF %d %d %f %f %f", 6, AC_ID,
					fdm.body_ecef_rotvel.p, fdm.body_ecef_rotvel.q,
					fdm.body_ecef_rotvel.r);
		}

/*
		IvySendMsg("NPS_AHRS_LTP %d %d %f %f %f %f", 6, AC_ID,
				fdm.ltp_to_body_quat.qi, fdm.ltp_to_body_quat.qx,
				fdm.ltp_to_body_quat.qy, fdm.ltp_to_body_quat.qz);

		IvySendMsg("NPS_AHRS_ECEF %d %d %f %f %f", 6, AC_ID,
				fdm.body_ecef_rotvel.p, fdm.body_ecef_rotvel.q,
				fdm.body_ecef_rotvel.r);

		printf("Here!\n");
*/
	}

	if (nps_bypass_ins) {
		sim_overwrite_ins();
		static int counterINS = 0;
		if (!(counterINS++ % 4)) {
			IvySendMsg("NPS_INS_POS %d %d %f %f %f", 6, AC_ID, fdm.ltpprz_pos.x,
					fdm.ltpprz_pos.y, fdm.ltpprz_pos.z);

			IvySendMsg("NPS_INS_ECEF_VEL %d %d %f %f %f", 6, AC_ID,
					fdm.ltpprz_ecef_vel.x, fdm.ltpprz_ecef_vel.y,
					fdm.ltpprz_ecef_vel.z);

			IvySendMsg("NPS_INS_ECEF_ACCEL %d %d %f %f %f", 6, AC_ID,
					fdm.ltpprz_ecef_accel.x, fdm.ltpprz_ecef_accel.y,
					fdm.ltpprz_ecef_accel.z);
	//TODO Figure out when to send this data...
		}
	}

#ifndef USE_FDM_IR
#define USE_FDM_IR
#endif

#ifdef USE_FDM_IR
	//if (!(counterIR++ % 4)) {
		IvySendMsg("NPS_SEN_NICE_IR %d %d %f %f %f %f %f %f", 6, AC_ID,
				fdm.ltp_to_body_eulers.phi, fdm.ltp_to_body_eulers.theta,
				fdm.ltp_to_body_eulers.psi, fdm.body_ecef_rotvel.p,
				fdm.body_ecef_rotvel.q, fdm.body_ecef_rotvel.r);
	//}
#endif
#ifdef USE_JSBSIM_IR
	//Send this ~every 10 hz
	if (!(counterIR++ % 4))
	{
		double ir[6];
		nps_fdm_remote_ir(&ir[0]);
		IvySendMsg("NPS_SEN_NICE_IR %d %d %f %f %f %f %f %f", 6, AC_ID,
				ir[0], ir[1], ir[2], ir[3], ir[4], ir[5]);
	}
#endif
	//Stuff the commands from the autopilot into the buffer here
	/* scale final motor commands to 0-1 for feeding the fdm */
	for (uint8_t i = 0; i < NPS_COMMANDS_NB; i++)
	{
		autopilot.commands[i] = (double) commands[i] / MAX_PPRZ;
	}

//	for (int i = 0; i < NPS_COMMANDS_NB; i++) {
//		printf(">%f::%d", autopilot.commands[i], commands[i]);
//	}
//	printf("\r\n");

	// hack: invert pitch to fit most JSBSim models
 	autopilot.commands[COMMAND_PITCH] = -(double) commands[COMMAND_PITCH]
 			/ MAX_PPRZ;
	// do the launch when clicking launch in GCS
	autopilot.launch = launch && !kill_throttle;
//	if (!launch)
//		autopilot.commands[COMMAND_THROTTLE] = 0;

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
/*
 void sim_use_gps_pos(double lat, double lon, double alt, double course, double gspeed, double climb, double time) {
 gps.fix = 3; // Mode 3D
 gps.course = course * 1e7;
 gps.hmsl = alt * 1000.;
 gps.gspeed = gspeed * 100.;
 gps.ned_vel.z = -climb * 100.;
 gps.week = 0; // FIXME
 gps.tow = time * 1000.;

 //TODO set alt above ellipsoid and hmsl

 struct LlaCoor_f lla_f;
 struct UtmCoor_f utm_f;
 lla_f.lat = lat;
 lla_f.lon = lon;
 utm_f.zone = nav_utm_zone0;
 utm_of_lla_f(&utm_f, &lla_f);
 LLA_BFP_OF_REAL(gps.lla_pos, lla_f);
 gps.utm_pos.east = utm_f.east*100;
 gps.utm_pos.north = utm_f.north*100;
 gps.utm_pos.zone = nav_utm_zone0;

 gps_available = TRUE;

 }



 void set_ir(double roll __attribute__ ((unused)), double pitch __attribute__ ((unused))) {
 // INFRARED_TELEMETRY : Stupid hack to use with modules
 #if USE_INFRARED
 double ir_contrast = 150; //FIXME
 double roll_sensor = roll + JSBSIM_IR_ROLL_NEUTRAL; // ir_roll_neutral;
 double pitch_sensor = pitch + JSBSIM_IR_PITCH_NEUTRAL; // ir_pitch_neutral;
 infrared.roll = sin(roll_sensor) * ir_contrast;
 infrared.pitch = sin(pitch_sensor) * ir_contrast;
 infrared.top = cos(roll_sensor) * cos(pitch_sensor) * ir_contrast;
 #endif
 }


 */
