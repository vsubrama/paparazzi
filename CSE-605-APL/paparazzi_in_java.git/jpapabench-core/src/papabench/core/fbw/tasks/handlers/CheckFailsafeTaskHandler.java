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

import papabench.core.autopilot.conf.PapaBenchAutopilotConf.ReportingTaskConf;
import papabench.core.commons.conf.FBWMode;
import papabench.core.commons.conf.RadioConf;
import papabench.core.fbw.conf.PapaBenchFBWConf.CheckFailsafeTaskConf;
import papabench.core.fbw.modules.FBWModule;
/**
 * Switch to fail save state of airplane control.
 * 
 * The switching depends on message lost.
 * 
 * T = 50ms
 * 
 * @author Michal Malohlava
 *
 */
public class CheckFailsafeTaskHandler implements Runnable {
	
	private FBWModule fbwModule;
	private boolean isExe =false;
	public CheckFailsafeTaskHandler(FBWModule fbwModule) {		
		this.fbwModule = fbwModule;		
	}
	
	public void run() {
//		if( !isExe ){
//			System.out.println("start: Thread_" + CheckFailsafeTaskConf.PRIORITY+" : " + Thread.currentThread().getName());
//			isExe = true;
//		}
		//System.out.println("CheckFailsafeTaskHandler start 777");
		if (fbwModule.getFBWMode() == FBWMode.MANUAL && !fbwModule.isRadioOK()
			|| fbwModule.getFBWMode() == FBWMode.AUTO && !fbwModule.isMega128OK()) {

			this.fbwModule.getServosController().setServos(RadioConf.safestateRadioCommands);
			
			System.out.println("Airplane was switched into failsafe mode");
		}
		
		//System.out.println("... CheckFailsafeTaskHandler done 777");
	}
}
