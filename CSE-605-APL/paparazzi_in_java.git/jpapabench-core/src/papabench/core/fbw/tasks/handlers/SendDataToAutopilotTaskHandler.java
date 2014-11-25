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
package papabench.core.fbw.tasks.handlers;

import papabench.core.commons.data.InterMCUMsg;
import papabench.core.fbw.conf.PapaBenchFBWConf.CheckMega128ValuesTaskConf;
import papabench.core.fbw.conf.PapaBenchFBWConf.SendDataToAutopilotTaskConf;
import papabench.core.fbw.modules.FBWModule;
import papabench.core.utils.LogUtils;

/**
 * Taks handler providing sending data to autopilot unit.
 * 
 * T = 25ms
 *  
 * @author Michal Malohlava
 * 
 */
//@SCJAllowed
public class SendDataToAutopilotTaskHandler implements Runnable {
	
	private FBWModule fbwModule;
	private boolean isExe = false;
	public SendDataToAutopilotTaskHandler(FBWModule fbwModule) {		
		this.fbwModule = fbwModule;
	}
		
	public void run() {	
//		if( !isExe ){
//			System.out.println("start: Thread_" + SendDataToAutopilotTaskConf.PRIORITY+" : " + Thread.currentThread().getName());
//			isExe = true;
//		}
		//System.out.println("SendDataToAutopilotTaskHandler start 999");
		// send only valid 
		if (fbwModule.getPPMDevice().isValid()) {
			//LogUtils.log(this, "send date to autopilot!!!");
			InterMCUMsg msg = new InterMCUMsg(); 
			msg.radioCommands = fbwModule.getPPMDevice().getLastRadioCommands().clone();
			
			msg.setRadioOK(fbwModule.isRadioOK());
			msg.setRadioReallyLost(fbwModule.isRadioReallyLost());
			msg.setAveragedChannelsSent(msg.radioCommands.containsAveragedChannels());
			
			msg.voltSupply = 0;
			msg.ppmCpt = 0; // FIXME <---this value should be computed
			
			fbwModule.getLinkToAutopilot().sendMessageToAutopilot(msg);
		}
		//System.out.println("... SendDataToAutopilotTaskHandler done 999");
	}

}
