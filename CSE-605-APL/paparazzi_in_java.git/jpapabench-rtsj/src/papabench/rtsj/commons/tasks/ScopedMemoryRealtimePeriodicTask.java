///* $Id: ScopedMemoryRealtimePeriodicTask.java 599 2010-11-01 10:35:42Z parizek $
// * 
// * This file is a part of jPapaBench providing a Java implementation 
// * of PapaBench project.
// * Copyright (C) 2010  Michal Malohlava <michal.malohlava_at_d3s.mff.cuni.cz>
// *
// * This program is free software; you can redistribute it and/or
// * modify it under the terms of the GNU General Public License
// * as published by the Free Software Foundation; either version 2
// * of the License, or (at your option) any later version.
// *
// * This program is distributed in the hope that it will be useful,
// * but WITHOUT ANY WARRANTY; without even the implied warranty of
// * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// * GNU General Public License for more details.
// *
// * You should have received a copy of the GNU General Public License
// * along with this program; if not, write to the Free Software
// * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
// * 
// */
//package papabench.rtsj.commons.tasks;
//
//import static papabench.rtsj.utils.ParametersFactory.getPeriodicParameters;
//import static papabench.rtsj.utils.ParametersFactory.getPriorityParameters;
//
//import javax.realtime.LTMemory;
//import javax.realtime.RealtimeThread;
//import javax.realtime.ReleaseParameters;
//import javax.realtime.SchedulingParameters;
//import javax.realtime.ScopedMemory;
//
///**
// * TODO comment this ASAP
// * 
// * FIXME put memory size into argument list of constructor
// * 
// * @author Michal Malohlava
// *
// */
//public class ScopedMemoryRealtimePeriodicTask extends RealtimePeriodicTask {
//	
//	public ScopedMemoryRealtimePeriodicTask(Runnable targeRunnable, int priority, int release, int period) {
//		super(targeRunnable, getPriorityParameters(priority), getPeriodicParameters(release, period));
//	}
//	
//	public ScopedMemoryRealtimePeriodicTask(Runnable targetRunnable, SchedulingParameters schedParam, ReleaseParameters releaseParam) {
//		super(targetRunnable, schedParam, releaseParam);		
//	}
//	
//	@Override
//	public void run() {
//		// allocate scope - FIXME the sizes should be passed by ctor
//		ScopedMemory scopedMemory = new LTMemory(1024,2048);
//		
//		// FIXME handle the missed periods
//		while (true) {			
//			do {				
//				scopedMemory.enter(targetRunnable);
//			} while(this.waitForNextPeriod());
//		}
//	}
//}
