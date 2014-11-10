#!/usr/bin/env python
""" ivy test for subscript the message from paparazzi from local main loop
"""
import traceback, time, string, os, sys, getopt, socket, threading, select
import logging
# importing ivycpy 
from ivy.std_api import *

IVYAPPNAME = "jUAV-bridge"
MSG_TYPE_LIST =['NPS_SEN_NICE_IR', 'NPS_SEN_NICE_GPS','DL_SETTING','DL_VALUE']
# ['DL_PING', 'DL_ACINFO', 'DL_SETTING', 'GET_DL_SETTING',
#                'BLOCK', 'MOVE_WP', 'NPS_SEN_NICE_IR', 'NPS_SEN_NICE_GPS',
#                'NPS_AHRS_LTP', 'NPS_AHRS_ECEF', 'NPS_INS_POS', 'NPS_INS_ECEF_VEL',
#                'NPS_INS_ECEF_ACCEL', 'NPS_SENSORS_GYRO', 'NPS_SENSORS_ACCEL'
#                ]

REMOTE_IP='127.0.0.1'
REMOTE_PORT=8080
LOCAL_PORT=8081

class CommChannels:
	""" Communication challels bewteen autoplolit and ivy-bridge"""
	def __init__(self):
		self.inputConn = {}
		self.outputConn = {}
		self.inputNum = 2
		self.outputNum = len(MSG_TYPE_LIST)
		
	
	def initConnection(self):
		print "Init output connection..."
		#establish the connection to remote 
		for i in range(len(MSG_TYPE_LIST)):
			s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
			s.connect((REMOTE_IP, REMOTE_PORT));
			s.send(MSG_TYPE_LIST[i]+"\n");
			self.outputConn[MSG_TYPE_LIST[i]] = s
		#create local listening socket
		localSocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
		localSocket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
		localSocket.bind(('',LOCAL_PORT))
		localSocket.listen(10)
		print "Init Input connection..."
		while(len(self.inputConn)<self.inputNum):
			conn, addr = localSocket.accept()#new connected socket
			f = conn.makefile()
			line = f.readline()
			line = line[0:-1]#get rid of linebreaker
			if line == "TEST":
				print "ACK"
			else:
				self.inputConn[line] = conn
				print "recieve:" + line
		print "Finish connections..."


	def getOutputConn(self, key):
		if key in self.outputConn:
			return self.outputConn[key]
		else:
			return None

	def getInputConn(self, key):
		if key in self.inputConn:
			return self.inputConn[key]
		else:
			return None


class IvySubscriptor:
	"""Ivy-bus subcriptor"""
	def __init__(self, channels, ivybus):
		self.channels = channels
		sisreadymsg = "[%s is ready]" % IVYAPPNAME
	 	# initialising the bus 
		#logging.getLogger('Ivy').setLevel(logging.ERROR)
		IvyInit(IVYAPPNAME,   # application name for Ivy
				sisreadymsg , # ready message
				0,            # main loop is local (ie. using IvyMainloop)
				self.oncxproc,     # handler called on connection/deconnection
				self.ondieproc     # handler called when a diemessage is received 
				)
		# starting the bus
		# Note: env variable IVYBUS will be used if no parameter or empty string
		# is given ; this is performed by IvyStart (C)
		IvyStart(ivybus)
		# binding on dedicated message : starting with "hello ..."
		#IvyBindMsg(onhello, "^hello=([^ ]*) from=([^ ]*)")
		# binding to every message 
		IvyBindMsg(self.onmsgproc, "(.*)")
		# creating a infinite timer 
		commandTimerId = IvyTimerRepeatAfter(0,     # number of time to be called
									  10,  # delay in ms between calls
									  self.onRevCommand # handler to call
									  )
		self.lprint( "IvyTimerRepeatAfter id is  %d", commandTimerId)
		
		othersTimerId = IvyTimerRepeatAfter(0,     # number of time to be called
									  50,  # delay in ms between calls
									  self.onRevOthers # handler to call
									  )
		self.lprint( "IvyTimerRepeatAfter id is  %d", othersTimerId)
		self.lprint( "%s doing IvyMainLoop", IVYAPPNAME)
		IvyMainLoop()

	def lprint(self, fmt,*arg):
		#print ""
		print IVYAPPNAME + ": " + fmt % arg

	def oncxproc(self, agent, connected):
		if connected == IvyApplicationDisconnected :
			self.lprint( "Ivy application %r was disconnected", agent)
		else:
			self.lprint( "Ivy application %r was connected", agent)
		self.lprint("currents Ivy application are [%s]", IvyGetApplicationList())
	
	def ondieproc(self, agent, id):
		self.lprint( "received the order to die from %r with id = %d", agent, id)

	def onmsgproc(self, agent, *larg):
		#self.lprint( "Received from %r: %s in %d", agent , larg[0], len(larg))
		key = larg[0].split(' ')[0]
		#print key
		if key == '4':
			key = larg[0].split(' ')[1]
			print key
		conn = self.channels.getOutputConn(key)
		msg = larg[0] + " \n";
		#msg = msg.ljust(255,'~'); 
		#msg = "256" + msg + '\0';
		msgLen = len(msg)
		msg = str(msgLen).zfill(3) + msg
		if conn is not None:
			self.lprint("Send out: %s", msg)
			conn.send(msg)
		else:
			self.lprint("can't find conn of %s", key)
    
	def onRevCommand(self):
		f = self.channels.getInputConn("COMMAND").makefile()
		msg = f.readline()
		print msg
		#self.lprint("REC_0: %s", msg)
		IvySendMsg(msg)
	
	def onRevOthers(self):
		f = self.channels.getInputConn("OTHERS").makefile()
		msg = f.readline()
		print msg
		self.lprint("REC_1: %s", msg)
		IvySendMsg(msg)


if __name__ == '__main__':
	# initializing ivybus and isreadymsg
	sivybus = ""
	# getting option
	try:
		optlist, left_args = getopt.getopt(sys.argv[1:],'hb:', ['ivybus='])
	except getopt.GetoptError:
		# print help information and exit:
		usage(sys.argv[0])
		sys.exit(2)
	for o, a in optlist:
		if o in ("-b", "--ivybus"):
			sivybus= a
	
	if sivybus != "" :
		sechoivybus = sivybus
	elif os.environ.has_key("IVYBUS"):
		sechoivybus = os.environ["IVYBUS"]
	else:
		sechoivybus = "ivydefault"
	print "Ivy will broadcast on %s " % sechoivybus

	channels = CommChannels()
	channels.initConnection()
	ivySubscriptor = IvySubscriptor(channels, sivybus)
