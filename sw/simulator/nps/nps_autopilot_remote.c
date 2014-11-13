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

void nps_autopilot_init(enum NpsRadioControlType type_rc, int num_rc_script,
		char* rc_dev) {

	autopilot.launch = FALSE;

	nps_radio_control_init(type_rc, num_rc_script, rc_dev);
	nps_electrical_init();

	nps_bypass_ahrs = NPS_BYPASS_AHRS;
	nps_bypass_ins = NPS_BYPASS_INS;

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
//#define DegOfRad(x) ((x) * (180. / M_PI))

	if (nps_sensors_gps_available()) {
		gps_feed_value();

		double vals[7];
		nps_fdm_remote_position(&vals[0]);
		IvySendMsg("NPS_SEN_NICE_GPS %d %d %f %f %f %f %f %f %f", 6, AC_ID,
				vals[0], vals[1], vals[2], vals[3], vals[4], vals[5], vals[6]);
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
		//		IvySendMsg("NPS_SEN_NICE_GPS %d %d %f %f %f %f %f %f %f", 6, AC_ID,
		//				DegOfRad(fdm.lla_pos_pprz.lat), DegOfRad(fdm.lla_pos_pprz.lon), fdm.lla_pos_pprz.alt, gps.course,
		//				gps.gspeed, 1.0, fdm.time);
	}

	if (nps_bypass_ahrs) {
		sim_overwrite_ahrs();
	}

	if (nps_bypass_ins) {
		sim_overwrite_ins();
	}

	//TODO Figure out when to send this data...
	{
		/*
		 //print(FDMExec);
		 double roll = get_value("attitude/roll-rad");
		 double pitch = get_value("attitude/pitch-rad");
		 double yaw = get_value("attitude/heading-true-rad");
		 double p = get_value("velocities/p-rad_sec");
		 double q = get_value("velocities/q-rad_sec");
		 double r = get_value("velocities/r-rad_sec");

		 IvySendMsg("NPS_SEN_NICE_IR %d %d %f %f %f %f %f %f", 6, AC_ID, roll,
		 pitch, yaw, p, q, r);
		 */
		//It may be better to get the pose data from the fdm state ... decide in future
		/*
		 *
		 */
//		IvySendMsg("NPS_SEN_NICE_IR %d %d %f %f %f %f %f %f", 6, AC_ID,
//				fdm.ltp_to_body_eulers.phi, fdm.ltp_to_body_eulers.theta,
//				fdm.ltp_to_body_eulers.psi, fdm.body_ecef_rotvel.p,
//				fdm.body_ecef_rotvel.q, fdm.body_ecef_rotvel.r);
		double ir[6];
		nps_fdm_remote_ir(&ir[0]);
		IvySendMsg("NPS_SEN_NICE_IR %d %d %f %f %f %f %f %f", 6, AC_ID, ir[0],
				ir[1], ir[2], ir[3], ir[4], ir[5]);
	}

	//Stuff the commands from the autopilot into the buffer here
	/* scale final motor commands to 0-1 for feeding the fdm */
	for (uint8_t i = 0; i < NPS_COMMANDS_NB; i++)
		autopilot.commands[i] = (double) commands[i] / MAX_PPRZ;
	// hack: invert pitch to fit most JSBSim models
	autopilot.commands[COMMAND_PITCH] = -(double) commands[COMMAND_PITCH]
			/ MAX_PPRZ;

	// do the launch when clicking launch in GCS
	autopilot.launch = launch && !kill_throttle;
	if (!launch)
		autopilot.commands[COMMAND_THROTTLE] = 0;

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
