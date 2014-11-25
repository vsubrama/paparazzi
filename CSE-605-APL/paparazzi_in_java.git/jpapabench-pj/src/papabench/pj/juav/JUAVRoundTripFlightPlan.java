/* $Id$
 * 
 * This file is a part of jPapaBench providing a Java implementation 
 * of PapaBench project.
 * Copyright (C) 2010  Michal Malohlava <michal.malohlava_at_d3s.mff.cuni.cz>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 * 
 */
package papabench.pj.juav;

import static papabench.core.commons.conf.AirframeParametersConf.CARROT;
import papabench.core.autopilot.data.Position3D;
import papabench.core.commons.data.UTMPosition;
import papabench.core.commons.data.impl.AbstractFlightPlan;

/**
 * @author Michal Malohlava
 * 
 */
public class JUAVRoundTripFlightPlan extends AbstractFlightPlan {

	protected static final int GROUND_ALTITUDE = 185;
	protected static final int SECURE_ALTITUDE = 250;

	// private final Position2D centerPos = new Position2D(10.548188f,
	// 52.316323f);

	public String getName() {
		return "Round trip flight plan";
	}

	@Override
	protected int getNumberOfNavBlocks() {
		return 2;
	}

	@Override
	protected int getNumberOfWaypoints() {
		return 5;
	}

	public float getGroundAltitude() {
		return GROUND_ALTITUDE;
	}

	public float getSecureAltitude() {
		return SECURE_ALTITUDE;
	}

	@Override
	protected void initNavigationBlocks() {
		this.addNavBlock(new NavigationBlock(3) { // navigation block 0
					// contains, two stages
					@Override
					protected void preCall() {
						// if (estimator.getPosition().z > getGroundAltitude() +
						// 25) {
						// gotoBlock(1);
						// System.out.println("JUAVFlightPlan: gotoBlock 1");
						// }
					}
				}).addNavStage(new NavigationStage() {
			@Override
			protected void execute() {
				if (!status.isLaunched()) {
					// System.out.println("Waiting for launch");
				} else {
					// Set autopitch to false, this ensures that the pitch and
					// Gaz are tracked
					killThrottle();
					estimator.setFlightTime(0);
					status.setLaunched(true);
					nextStage();
					System.out.println("JUAVFlightPlan: nextStage");
				}
			}
		}).addNavStage(new NavigationStage() {
			@Override
			protected void execute() {
				if (navApproachingFrom(4, 1, CARROT)) {
					nextStageFrom(4);
					System.out.println("JUAVFlightPlan: nextStageFrom 4");
				} else {
					navGotoWaypoint(4);
					// Set the vertical mode to altitude control
					// navVerticalAltitudeMode(250, 200);
					// Set to auto throttle to maintain a pitch of 15'
					navVerticalAutoThrottleMode((float) Math.toRadians(15));
					navVerticalThrottleMode(7777);
					// System.out
					// .println("JUAVFlightPlan: navGotoWaypoint 4, climb");
				}
			}
		}).addNavStage(new NavigationStage() {
			@Override
			protected void execute() {
				if (navApproachingFrom(2, 1, CARROT)) {
					nextStageFrom(2);
					System.out.println("JUAVFlightPlan: nextStageFrom 2");
				} else {
					navGotoWaypoint(2);
					navVerticalAutoThrottleMode((float) Math.toRadians(15));
					navVerticalThrottleMode(9600);
					System.out.println("JUAVFlightPlan: navGotoWaypoint 2");
				}
			}
		});

		this.addNavBlock(new NavigationBlock(2)) // navigation block 1
				.addNavStage(new NavigationStage() {
					@Override
					protected void execute() {
						if (navApproachingFrom(2, 1, CARROT)) {
							nextStageFrom(2);
							System.out
									.println("JUAVFlightPlan: nextStageFrom 2");
						} else {
							navGotoWaypoint(2);
							navVerticalAutoThrottleMode(0);
							navVerticalAltitudeMode(WPALT(2), 0f);
							System.out
									.println("JUAVFlightPlan: navGotoWaypoint 2");
						}

					}
				}).addNavStage(new NavigationStage() {

					@Override
					protected void execute() {
						if (navApproachingFrom(3, getLastWPNumber(), CARROT)) {
							nextStageFrom(3);
							System.out
									.println("JUAVFlightPlan: nextStageFrom 3");
						} else {
							navGotoWaypoint(3);
							navVerticalAutoThrottleMode(0f);
							navVerticalAltitudeMode(WPALT(3), 0f);
							System.out
									.println("JUAVFlightPlan: navGotoWaypoint 3");
						}
					}
				});
	}

	// FIXME replace by a method getWaypoint(X)
	@Override
	protected void initWaypoints() {
		this.addWaypoint(new Position3D(42, 42, 250)); // dummy
		this.addWaypoint(new Position3D(0f, 0f, 0f)); // HOME
		this.addWaypoint(new Position3D(-337, 17, 250)); // CHECK POINT #1
		this.addWaypoint(new Position3D(238, -30, 250)); // CHECK POINT #2
		this.addWaypoint(new Position3D(100, -100, 220)); // CLIMB
	}

	@Override
	protected UTMPosition getCenterUTMPosition() {
		return null;
	}
}
