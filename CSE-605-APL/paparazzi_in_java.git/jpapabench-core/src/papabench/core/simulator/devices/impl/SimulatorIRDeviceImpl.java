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
package papabench.core.simulator.devices.impl;

import papabench.core.autopilot.data.Attitude;
import papabench.core.autopilot.devices.impl.IRDeviceImpl;
import papabench.core.simulator.devices.SimulatedDevice;
import papabench.core.simulator.model.FlightModel;

/**
 * Simulated IR device.
 * 
 * It computes IR values according to a state for airplane.
 * 
 * @author Michal Malohlava
 * 
 */
public class SimulatorIRDeviceImpl extends IRDeviceImpl implements
		SimulatedDevice {

	private int irLeft; // roll
	private int irFront; // pitch
	private int irTop; // top

	public void update(FlightModel flightModel) {
		Attitude attitude = flightModel.getState().getAttitude();

		float phiSensor = attitude.phi + getIrRollNeutral();
		float thetaSensor = attitude.theta + getIrPitchNeutral();

		irLeft = (int) (Math.sin(phiSensor) * getIrContrast());
		irFront = (int) (Math.sin(thetaSensor) * getIrContrast());
		irTop = (int) (Math.cos(phiSensor) * Math.cos(thetaSensor) * getIrContrast());
	}

	@Override
	public void update() {
		// do nothing
	}

	@Override
	public int getIrRoll() {
		return irLeft;
	}

	@Override
	public int getIrPitch() {
		return irFront;
	}

	@Override
	public int getIrTop() {
		return irTop;
	}

	public void setIrLeft(int irLeft) {
		this.irLeft = irLeft;
	}

	public void setIrFront(int irFront) {
		this.irFront = irFront;
	}

	public void setIrTop(int irTop) {
		this.irTop = irTop;
	}

	@Override
	public void update(float[] val) {
		// TODO Auto-generated method stub

	}

	@Override
	public void setAltitude(float altitude) {
		// TODO Auto-generated method stub

	}

	@Override
	public void setClimb(float climb) {
		// TODO Auto-generated method stub

	}

	@Override
	public void setCourse(float course) {
		// TODO Auto-generated method stub

	}

	@Override
	public void setEast(float east) {
		// TODO Auto-generated method stub

	}

	@Override
	public void setNorth(float north) {
		// TODO Auto-generated method stub

	}

	@Override
	public void setUtmEast(int utmEast) {
		// TODO Auto-generated method stub

	}

	@Override
	public void setUtmNorth(int utmNorth) {
		// TODO Auto-generated method stub

	}

	@Override
	public void setSpeed(float speed) {
		// TODO Auto-generated method stub

	}

	@Override
	public void setMode(int mode) {
		// TODO Auto-generated method stub

	}

	@Override
	public void setTow(float tow) {
		// TODO Auto-generated method stub

	}

	@Override
	public void setPositionAvailable(boolean positionAvailable) {
		// TODO Auto-generated method stub

	}
}
