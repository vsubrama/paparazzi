#include <QtCore/QThread>
#include <QtExtSerialPort/qextserialport.h>
#include "common.h"
#include "synchqueue.h"
class CReaderThread:public QThread{
  Q_OBJECT
    public:
  CReaderThread(QextSerialPort* pSerialport,QSynchQueue< ethernetMessage>*qu):
  pqtSerialPort(pSerialport),
    pbufferedQueue(qu){
    }
  ~CReaderThread(){
    pbufferedQueue=NULL;
    pqtSerialPort=NULL;
  }
 protected:
  void run();
 private:
  bool checkForStartofMsg(char bPtr, int i);
  bool checkForEndOfMsg1(char bPtr, int i);
  bool checkForEndOfMsg2(char bPtr, int i);
  QextSerialPort *pqtSerialPort;
  //A shared queue to put the incoming message from
  //serial port.
  QSynchQueue< ethernetMessage>* pbufferedQueue;
 private:
  void readPort();
  char buffer[MAX_BYTE];
};
