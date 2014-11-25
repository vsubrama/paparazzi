/* $Id: SimulatorServosControllerImpl.java 606 2010-11-02 19:52:33Z parizek $
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

import papabench.core.commons.data.RadioCommands;
import papabench.core.fbw.devices.impl.ServosControllerImpl;
import papabench.core.simulator.devices.SimulatedDevice;
import papabench.core.simulator.model.FlightModel;
import papabench.core.utils.LogUtils;

/**
 * TODO comment
 * 
 * @author Michal Malohlava
 *
 */
public class SimulatorServosControllerImpl extends ServosControllerImpl implements SimulatedDevice {
	
	private boolean processed = true;

	public void update(FlightModel flightModel) {
		if (!processed) {
			flightModel.processCommands(this.radioCommands);			
			processed = true;
		}
	}
	
	@Override
	public void setServos(RadioCommands radioCommands) {		
		super.setServos(radioCommands);
		//LogUtils.log(this, "Radio commands for servos: " + radioCommands.toString());
		
		processed = false;
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

	@Override
	public void setIrLeft(int irLeft) {
		// TODO Auto-generated method stub
		
	}

	@Override
	public void setIrFront(int irFront) {
		// TODO Auto-generated method stub
		
	}

	@Override
	public void setIrTop(int irTop) {
		// TODO Auto-generated method stub
		
	}
}
