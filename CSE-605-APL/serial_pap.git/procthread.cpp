#include "procthread.h"
#include <QtCore/QDebug>
#include <iostream>
#include "messages.h"
#include "msgfactory.h"
#include "common.h"
#include <QTime>

#define MSG_HEADER_LEN        ((1+1+1+1))/*STX+MSG_LEN+AC_ID|MSG_ID*/
#define MSG_TAIL_LEN          ((1+1)) /*ck_a+ck_b*/
#define MOVE_TO_DATA(buffer,start)  (buffer=&(((char*)buffer)[start+MSG_HEADER_LEN]))
#define GET_MSG_LEN(buffer,start)    ((buffer[start+1]))
#define GET_MSG_ID(buffer,start)    ((buffer[start+3]))
#define GET_MSG_PTR(buffer,start)   (&(buffer[start+MSG_HEADER_LEN]))
#define GET_IVY_PAYLOAD_PTR(buffer,start)   (&(buffer[start+IvyMsg::MARKERBYTES-1]))


/*
  Message Format:
  ----------------------------------------------------------------------------------------
  | |STX|MSG_LEN|AC_ID|MSG_ID|-----Actual Msg----|ck_a|ck_b|                               |
  | total length=len(stx)+len(MSG_LEN)+len(AC_ID)+len(MSG_ID)+len(MSG)+len(ck_a)+len(ck_b) |
  ----------------------------------------------------------------------------------------
*/
bool CMsgProcThread::verifychecksum(const char* buffer){
    quint8 checksum_a=0,checksum_b=0,ck_a=0,ck_b=0,msg_len=0;
    int i=0;
    if(buffer[0]!=CTelemetryMsg::STX ){
        qDebug()<<"Incoorect Input to verify checksum\n";
        return false;
    }

    msg_len = buffer[1];

    if(msg_len <MSG_TAIL_LEN)
        return false;

    i=1;/*start from the message len*/
    while(i!=(msg_len-MSG_TAIL_LEN)){
        checksum_a+=buffer[i++];
        checksum_b+=checksum_a;
    }

    ck_a = buffer[i++];
    ck_b = buffer[i++];

    if(ck_a != checksum_a){
        qDebug()<<"Recived data has checksum error ck_a!=checksum_a\n";
        return false;
    }
    if(checksum_b!=ck_b){
        qDebug()<<"Recived data has checksum error ck_b!=checksum_b\n";
        return false;
    }
    return true;
}
void CMsgProcThread::processmsg(char* buffer){
    if(buffer==NULL)
        return;
    int i=0;
    bool processFlag = true;
    while(i<MAX_BYTE && processFlag){
        char start_byte = buffer[i];
        //char sanityCheck = buffer[i+1];
        //if(sanityCheck == 0x01 && start_byte == '~')
        //    std::cout<<"Sanity Check Failed!!!";
        CTelemetryMsg* pmsg=NULL;
        bool isMsgValid=false;
        int msg_len=0;
        quint8 msg_id=0;
        switch(start_byte){
        case CMsg::STX:{
            isMsgValid = verifychecksum(&buffer[i]);
            if(isMsgValid){
                msg_len = GET_MSG_LEN(buffer,i);
                pmsg = CMsgFactory::CreateMsg(GET_MSG_ID(buffer,i),msg_len,GET_MSG_PTR(buffer,i));
            }
            if(pmsg){
                pmsgContainer->enqueue(pmsg);
                std::cout << pmsg->getPrettyMsg().toStdString() << std::endl;
            }else{
                std::cout <<"Couldn't form the message from message id:"<<(int)(buffer[i+3])<<std::endl;
            }
        }
            break;
        case CMsg::IVY_START:{
            isMsgValid = IvyMsg::verifyMsg(&buffer[i]);
            if(isMsgValid){
                msg_len = buffer[i+1];
                pmsg = CMsgFactory::CreateMsg(DL_IVY_MSG_ID,msg_len,&buffer[i]);
            }
            if(pmsg){
                //std::cout <<"^" << pmsg->getPrettyMsg().toStdString() << std::endl;
                pivymsgqueue->enqueue(pmsg);
            }
            //There is onle ONE message to process here, the rest is padding
            processFlag = false;
        }
            break;
        default:
            // std::cout <<"X";
            break;
        }
        isMsgValid?(i+=msg_len):(i++);
    }
    return;
}

void CMsgProcThread::run(){
    while(bstop==false){

        /*
         * The incoming message from the queue will be a block of data that will contain fragments of messages
         * This data will need to be reconstructed to ensure that no messages are lost at the boundary of the buffers
         * A simple mechanism to do all of this is to stuff it into a circular buffer and do our message processing
         * from the circular buffer. Then there is no need to worry about storing fragments and correctly assembling
         * them as we just either pull an entire message from the buffer or wait until the entire message is in
         * the buffer. Bad joins will only happen with bad messages
         * */
        //While we have any messages in the queue
        while(pEtherRx->get_size()>0){

            if(pEtherRx->get_size()>0){
                QTime time = QTime::currentTime();
                QString timeString = time.toString();
                std::cout<< timeString.toStdString() << "ProcThread:" << time.msec() << " " << pEtherRx->get_size() << " elements remaining \r\n";
            }

            //First, get a message from the queue
            ethernetMessage rawMsg = pEtherRx->dequeue();
            //Second, copy this message into the circular buffer
            circBuf.putBlock((unsigned char*)rawMsg.msg,rawMsg.length);
            //While there is data in the circular buffer parse it
            while(circBuf.getSize() > 128){
                //Third, process parse the circular buffer
                ethernetMessage processedMsg = UtilityParse::parsePacket(&circBuf);
                //If the message is try to process it
                if(processedMsg.id == VALID_MESSAGE_ID)
                    processmsg(processedMsg.msg);
            }
        }
        msleep(5);
        /*
                //        std::cout<< "Message left in pbufferedQueue "<< pbufferedQueue->get_size() << std::endl;

                //char* msgbuff = pbufferedQueue->dequeue();
                if(pEtherRx->get_size()>0){

            //            QString message =
            //            std::cout << message.toStdString() << std::endl;
            //            QByteArray ba = message.toLocal8Bit();

            ethernetMessage msg;
            msg = pEtherRx->dequeue();
            //ethernetMessage message = *pEtherRx->dequeue();
            if(msg.id == VALID_MESSAGE_ID){
                processmsg(msg.msg);
            }
        }else{
            msleep(10);
        }
        //        QString str;
        //        if(msgbuff)
        //            str+=msgbuff;
        //        std::cout<< "::Processing "<< str.toStdString()  << std::endl;

        //        //If there is no data to process, then sleep
        //        if(msgbuff==NULL)
        //            msleep(10);
        //        else{
        //            processmsg(msgbuff);
        //            memset(msgbuff, '0', sizeof(msgbuff));
        //        }
    }
    std::cout<<"now processor thread is exiting" << std::endl;
    return;
    */
    }
}


