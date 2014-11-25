#include "serial_port_writer.h"
#include <iostream>
#include <QTime>
#include "reader.h"

#ifdef SERIO_TESTING
#include "paparazzi.h"
#include  "dl_protocol2.h"
#include "messages.h"
#include "hitl_transport.h"
#endif

void CWriterThread::run(){
    writePort();
    return;
}
#ifndef SERIO_TESTING
void CWriterThread::writePort(){
    if(pqtSerialPort==NULL)
        return;
    int i=0;
    qint64 status=0;
    while(pqtSerialPort->isOpen()){
        if(pbufferedQueue->get_size()>0){
            QTime time = QTime::currentTime();
            QString timeString = time.toString();
            std::cout<< timeString.toStdString() << "TxThread:" << time.msec() << " " << pbufferedQueue->get_size() << " elements remaining \r\n";
        }
        ethernetMessage m;
        m = pbufferedQueue->dequeue();
        if(m.id == VALID_MESSAGE_ID){
            for(i=0;i<m.length;i++){
                status = pqtSerialPort->write(&m.msg[i],1);
            }
        }else{
            msleep(5);
        }
    }
    std::cout << std::endl;
    std::cout << "serial port is closed."<<std::endl;
    std::cout << "writer thread is now exiting"<<std::endl;
    return;
}
#else
void CWriterThread::writePort(){


    double lat    = 0.1;//get_value("position/lat-gc-rad");
    double lon    = 0;//get_value("position/long-gc-rad");
    double alt    = 0.3;//get_value("position/h-sl-meters");
    double course = 0.4;//get_value("attitude/heading-true-rad");
    double gspeed = 0.5;//get_value("velocities/vg-fps") * FT2M;
    double climb  = 0.6;//get_value("velocities/v-down-fps") * (-FT2M);
    double time   = 0.7;//get_value("simulation/sim-time-sec");
    uint8_t gps_id = 1;

    uint8_t ac_id=AC_ID;

    int i=0;
    qint64 status=0;


    /*Queue for collecting outgoing messages bytes to serial port.*/
    QSynchQueue<char*> uplinkmsgqueue;
    QSynchQueue<char*>*pdatalink_queue=&uplinkmsgqueue;;
    HitlTransport TransportChannel(pdatalink_queue);



    if(pqtSerialPort==NULL)
        return;

    while(pqtSerialPort->isOpen()){


        DOWNLINK_SEND_HITL_GPS_COMMON(&TransportChannel,
                                      &gps_id,
                                      &ac_id,
                                      &lat,
                                      &lon,
                                      &alt,
                                      &course,
                                      &gspeed,
                                      &climb,
                                      &time);


        buffer = pdatalink_queue->dequeue();
        if(buffer==NULL)
            continue;
        for(i=0;i<MAX_BYTE;i++){
            status = pqtSerialPort->write(&buffer[i],1);
        }
        /*Delete and reset buffer to NULL*/
        delete [] buffer;
    }
    std::cout << std::endl;
    std::cout << "serial port is closed."<<std::endl;
    std::cout << "writer thread is now exiting"<<std::endl;
    return;


}
#endif 
