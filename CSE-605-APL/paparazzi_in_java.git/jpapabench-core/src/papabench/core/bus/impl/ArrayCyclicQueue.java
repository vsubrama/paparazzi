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
package papabench.core.bus.impl;

import javax.realtime.RealtimeThread;

/**
 * Simple cyclic queue.
 * 
 * @author Michal Malohlava
 * 
 */
public class ArrayCyclicQueue /* implements Queue */{

	private Object[] queue;
	// the head of queue
	private int queueStart;
	// the last item of queue
	private int queueEnd;
	private int queueSize;

	private int maxSize;
	private String buf;
	
	public ArrayCyclicQueue(int maxSize) {
		this.maxSize = maxSize;
		this.queue = new Object[maxSize];
		this.queueEnd = 0; // tail - first empty slot
		this.queueStart = -1; // first element
		this.queueSize = 0;
		this.buf = "";
	}

	/*
	 * enqueue function
	 */
	public synchronized void offer(Object o) {
		//buf += RealtimeThread.currentThread().getName() + "offter:"+queueSize+"\n";
		queue[queueEnd] = o;
		queueSize++;
		if (queueSize == 1) {
			queueStart = queueEnd;
		}
		queueEnd = (queueEnd + 1) % maxSize;
	}

	/*
	 * dequeue function
	 */
	public synchronized Object poll() {
		//buf += RealtimeThread.currentThread().getName()+"poll:"+queueSize+"\n";
		if (queueSize == 0) {
			//System.out.println(buf);
			return null;
		} else {
			Object o = queue[queueStart];
			queue[queueStart] = null;
			queueStart = (queueStart + 1) % maxSize;
			queueSize--;
			return o;
		}
	}

	/*
	 * return the first element in queue, but don't pop up
	 */
	public synchronized Object peek() {
		if (queueSize == 0) {
			return null;
		} else {
			return queue[queueStart];
		}
	}

	public synchronized boolean isEmpty() {
		return queueSize == 0;
	}

	public void dumpAllStackTraces() {
//		for (Map.Entry<Thread, StackTraceElement[]> entry : Thread.getAllStackTraces().entrySet()) {
//			printk(entry.getKey().getName() + ":");
//			for (StackTraceElement element : entry.getValue())
//				printk("\t" + element);
//		}
		try {
            throw new Exception("returning null");
        } catch (Exception e) {
            e.printStackTrace();
        }  
	}
}
