#ifndef LISTENER_H
#define LISTENER_H
#include <QtCore/QDebug>
#include <QCoreApplication>
#include <QtCore/QTimer>
#include <QtExtSerialPort/qextserialport.h>
#include <QtExtSerialPort/qextserialenumerator.h>
#include <QtCore/QSemaphore>
#include <QtCore/QVector>

#include "ivy_dl_thread.h"
#include "ivythread.h"
#include "reader.h"
#include "serial_port_writer.h"
#include "procthread.h"
#include "nps_thread.h"
#include "telemsg.h"
#include "synchqueue.h"
#include "recv_tcpserver.h"
#include "sendtcpserver.h"

class ListenApp:public QCoreApplication{
    Q_OBJECT
public:
  ListenApp(int argc, char* argv[]);
  ~ListenApp();
  bool setup(int brate,int time);
 private Q_SLOTS:
  void serial_port_dsr_event(bool bstatus);
 signals:
  void serial_port_closed();
private:
  /*Thread responsible for reading the serial port*/
  CReaderThread* reader; /*reader thread.*/
  /*Thread responsible for writing the serial port*/
  CWriterThread* writer; /*writer thread.*/

  /*Thread which process the incoming raw bytes
    coming from serial port in to meaning full message
    used by JSBSIM and Ivy Thread.
   */
  CMsgProcThread* processor;

  /*
    Thread which runs ivy main loop. This thread also 
    listens messages from ivy bus and convert them in to
    raw bytes which are required to be sent to autopilot
    during simulation. 
  */
  IvyThread* pivyloopthread; 

  /*
    Thread which gets messages from ivyqueue and 
    put them on the ivy bus. These message are meant
    to be listen by GCS(ground control station.)running
    on a laptop.
  */
  
  CIvyDlThread* ivy_dl_thread; /*reader thread.*/

  /*
    Thread that get the messages from the Autopilot running on the remote
    board and compiles them into an IVYBus message format and transmits to
    whatever is listening for them (paparazzi nps simulator in this case)
  */
  //NpsCommThread* nps_com_thread;

  /*Queue for collecting incoming message bytes from serial port.*/
  QSynchQueue<ethernetMessage> msgbuffqueue;
  /*Queue for collecting outgoing messages bytes to serial port.*/
  QSynchQueue<ethernetMessage>uplinkmsgqueue;
  QSynchQueue<ethernetMessage>dummyqueue;

  /*a synch queue for collecting pprz_type messages from autopilot.
    (we can remove it later)*/
  QSynchQueue<CTelemetryMsg*>telemsgcontainer;
  
  /*a synch queue for input messages from autopilot to be
   sent to ivy bus.*/
  QSynchQueue<CTelemetryMsg*>ivyqueue;
  /*a synch queue for collecting input messages which are
    required by jsbsim for producing sensor values.*/
  QSynchQueue<CTelemetryMsg*>jsbsimqueue;

#ifdef ENABLE_ETHERNET_TRANSPORT
  // Sai:

  /*Queue for collecting outgoing messages bytes to Ethernet.*/
  QSynchQueue<ethernetMessage>eth_uplinkmsgqueue;
  RecvTcpServer* recvTcpServerThread;
  SendTcpServer* sendTcpServerThread;
  //EthernetWriterThread* ethWriterThread;
#endif

  QextSerialPort *pqtSerialPort;
  QTimer *ptimer;

 public Q_SLOTS:  
  void closeapp();
};

#endif // LISTENER_H
