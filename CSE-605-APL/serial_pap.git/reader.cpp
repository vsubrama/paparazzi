#include <iostream>
#include "reader.h"
void CReaderThread::run(){
    readPort();
    return;
}

//Check for start of message '~'
//Check for trailer of message
bool CReaderThread::checkForStartofMsg(char bPtr, int i){
    char val = bPtr;
    if(val == '~')
        return true;

    return false;
}

bool CReaderThread::checkForEndOfMsg1(char bPtr, int i){
    if(bPtr == 0x00)
        return true;

    return false;
}


bool CReaderThread::checkForEndOfMsg2(char bPtr, int i){
    if(bPtr == 0x7f)
        return true;

    return false;
}

void CReaderThread::readPort(){
    if(pqtSerialPort==NULL)
        return;
    int i=0;
    char* temp = NULL;
    int state = 0;
    int index = 0;
    bzero(buffer,MAX_BYTE);

    while(pqtSerialPort->isOpen()){

        qint64 status= pqtSerialPort->read(&buffer[i++],1);
        if(status<=0){
            i--;
        }
        /*
        switch (state) {
        case 0:
            //Waiting for start of message
            index = i-1;
            if(index>0){
                if(checkForStartofMsg(buffer[index],i)){
                    state = 1;
                }
            }
            break;
        case 1:
            index = i-1;
            //Wait for ending char 0x00
            if(index>0)
                if(checkForEndOfMsg1(buffer[index],i) ){
                    state = 2;
                    std::cout<<buffer << std::endl;
                }
            break;
        case 2:
            index = i-1;
            //Wait for ending char 0x7f
            if(index>0)
                if(checkForEndOfMsg2(buffer[index],i) ){
                    state = 3;
                    std::cout<<"Here\r\n";
                }
            break;
        case 3:
*/
        if(i==MAX_BYTE){
            //Add the message to the queue
            //Because the rest of the program is working onthe expectation
            //that we have a buffer of MAX_BYTE, ONLY send a single message
            //and pad the rest with spaces. Quicker than rooting out the changes
            for(int j=i;j<MAX_BYTE;j++){
                buffer[j] = 0x01;
            }
            ethernetMessage m;
            m.id = VALID_MESSAGE_ID;
            m.length = MAX_BYTE;
            memcpy(m.msg,buffer,MAX_BYTE);

            pbufferedQueue->enqueue(m);

            bzero(m.msg,MAX_BYTE);
        }
            /*
            state = 0;
            break;
        default:
            break;
        }
        */
        //Catch all incase the message is incorrect, reset statemachine
        if(i==MAX_BYTE){
            state = 0;
            i=0;
            bzero(buffer,MAX_BYTE);
        }
    }
    std::cout << std::endl;
    std::cout << "serial port is closed."<<std::endl;
    std::cout << "reader thread is now exiting"<<std::endl;
    return;
}


/*
void CReaderThread::readPort(){
  if(pqtSerialPort==NULL)
    return;
  int i=0;
  char* temp = NULL;
  bzero(buffer,MAX_BYTE);
  while(pqtSerialPort->isOpen()){
    qint64 status= pqtSerialPort->read(&buffer[i++],1);
    if(status<=0){
      i--;
    }
    if(i==MAX_BYTE){
      i=0;
      temp = new char[256];
      memcpy(temp,buffer,MAX_BYTE);
      pbufferedQueue->enqueue(temp);
      bzero(buffer,MAX_BYTE);
    }
  }
  std::cout << std::endl;
  std::cout << "serial port is closed."<<std::endl;
  std::cout << "reader thread is now exiting"<<std::endl;
  return;
  }
*/
