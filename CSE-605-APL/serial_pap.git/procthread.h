#include <QtCore/QThread>
#include <QtCore/QVector>
#include <QtCore/QtGlobal>
#include "telemsg.h"
#include "synchqueue.h"
#include "common.h"
#include "ring_buf.h"

class CMsgProcThread:public QThread{
  Q_OBJECT
 public:
  CMsgProcThread(QSynchQueue<ethernetMessage>*qu, QSynchQueue<CTelemetryMsg*>* pmsgvec,
		 QSynchQueue<CTelemetryMsg*>* pivyqueue,
         QSynchQueue<CTelemetryMsg*>* pjsbqueue,QSynchQueue<ethernetMessage>*q
		 ):
  pbufferedQueue(qu),
    pmsgContainer(pmsgvec),
    pivymsgqueue(pivyqueue),
    pjsbsimqueue(pjsbqueue),
    pEtherRx(q)
    {
      bstop=false;
    }
  
  public Q_SLOTS:
  void stop_processing(){
    bstop = true;
  }
 protected:
  void run();
 private:
  bool bstop;
  void processmsg( char* buffer);
  /*this function always assumes that the data ponited by
    buffer always has STX as it starts byte. This function check
    for validity of the message by verfying the checksum of the buffer.
  */
  bool verifychecksum(const char* buffer);
 private:
  //A shared queue to put the incoming message from
  //serial port.
  QSynchQueue< ethernetMessage>* pbufferedQueue;
  QSynchQueue< ethernetMessage>* pEtherRx;
  QSynchQueue< CTelemetryMsg*>* pmsgContainer;
  QSynchQueue< CTelemetryMsg*>* pivymsgqueue;
  QSynchQueue< CTelemetryMsg*>* pjsbsimqueue;
  ringBuffer circBuf;
}; 


class UtilityParse
{
public:
    UtilityParse();
    static ethernetMessage parsePacket(ringBuffer* r){
        static int state;
        static int msgStrIdx;
        static int msgLen;
        unsigned char start,c;
        ethernetMessage dummyMessage;
        static ethernetMessage validMessage;
        dummyMessage.id = 0;
        //First cut
        switch (state) {
        case 0:
            //Need two bytes to process the beginning of a message
            if(r->getSize()<2) return dummyMessage;

            start = r->get();

            //If the start byte is correct
            if(start == 0x7E){
                //Get the message length and increment the state
                msgLen = r->get();
                validMessage.msg[0] = start;
                validMessage.msg[1] = msgLen;
                validMessage.length = 2;
                state++;
            }else return dummyMessage; //Message is not fully in this packet

            break;

        case 1:
            c = r->get();
            validMessage.msg[validMessage.length++] = c;
            if(c==0x00){
                c = r->get();
                validMessage.msg[validMessage.length++] = c;
                if(c==0x7f){
                    validMessage.id = VALID_MESSAGE_ID;
                    state=msgStrIdx=msgLen=0;
                    return validMessage;
                }
            }
            break;
        default:
            state=msgStrIdx=msgLen=0;
            break;
        }
        return dummyMessage;
    }
};




