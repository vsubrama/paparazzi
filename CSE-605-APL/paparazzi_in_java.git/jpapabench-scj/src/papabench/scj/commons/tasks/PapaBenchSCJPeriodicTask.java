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
package papabench.scj.commons.tasks;

import static papabench.scj.utils.ParametersFactory.getPeriodicParameters;
import static papabench.scj.utils.ParametersFactory.getPriorityParameters;

import javax.safetycritical.PeriodicEventHandler;

import papabench.core.commons.conf.CommonTaskConfiguration;
import papabench.core.utils.LogUtils;

/**
 * PeriodicEventHandler wrapper.
 * 
 * @author Michal Malohlava
 *
 */
public class PapaBenchSCJPeriodicTask extends PeriodicEventHandler implements CommonTaskConfiguration {
	
	private Runnable targetRunnable;
	
	private String name;
	
	/* 
	 * Static variable to distinguish between two repetition of PapaBench schedule
	 * 
	 * Required by JPF.
	 */	
	public static int PAPABENCH_TASKS_EXECUTION_COUNTER = 0;

	public PapaBenchSCJPeriodicTask(Runnable targetRunnable, int priority, int releaseMs, int periodMs, long memSize, String name){
		//super(getPriorityParameters(priority), getPeriodicParameters(releaseMs, periodMs), null, memSize);
		super(getPriorityParameters(priority), getPeriodicParameters(releaseMs, periodMs), null, name);
		//TODO
		//I have no idea about how to fix it!!!!
		
		this.targetRunnable = targetRunnable;
		this.name = name;
	}
	
	@Override
	final public void handleAsyncEvent() {
//		LogUtils.log(getTaskName(), "PERIOD handler enter");		
		
		try {
			PAPABENCH_TASKS_EXECUTION_COUNTER++;
			
			targetRunnable.run();			
		} finally {		
//			LogUtils.log(getTaskName(), "PERIOD handler return");
		}
	}

	@Override
	public String getTaskName() {
		return this.name;		
	}	
}
