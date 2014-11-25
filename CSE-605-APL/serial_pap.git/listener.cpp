#include <Ivy/ivy.h>
#include <iostream>
#include "common.h"

#include "listener.h"


extern int enumerateserialports();
extern void setserialconf(PortSettings& portsettings);

ListenApp::ListenApp(int argc, char *argv[]):

    QCoreApplication(argc,argv) {
    std::cout << "Starting listenapp"<<std::endl;

    ptimer = NULL;

#ifdef ENABLE_SERIAL_TRANSPORT
    pqtSerialPort =NULL;
    PortSettings settings = {BAUD230400, DATA_8, PAR_NONE, STOP_1, FLOW_OFF, 0};
    //Choosing S0 as there is no serial port comms required and not ready to root this functionality
    //out of the code yet. So, set to unneeded port
    pqtSerialPort = new QextSerialPort ("/dev/ttyUSB0",settings,QextSerialPort::Polling);
#endif
    /*create timer but don't start it now*/
    ptimer = new QTimer(0);

    /*first create threads.*/
#ifdef ENABLE_SERIAL_TRANSPORT
    reader = new CReaderThread(pqtSerialPort,&msgbuffqueue);
    writer = new CWriterThread(pqtSerialPort,&uplinkmsgqueue);
    processor = new CMsgProcThread(&msgbuffqueue,&telemsgcontainer,&ivyqueue,
                                   &jsbsimqueue,&msgbuffqueue);
#endif
    ivy_dl_thread = new CIvyDlThread(&ivyqueue);
    pivyloopthread = new IvyThread(&uplinkmsgqueue);
#ifdef ENABLE_ETHERNET_TRANSPORT
    processor = new CMsgProcThread(&msgbuffqueue,&telemsgcontainer,&ivyqueue,
                                   &jsbsimqueue,&dummyqueue);
    recvTcpServerThread = new RecvTcpServer(&dummyqueue/*&msgbuffqueue*/);
    sendTcpServerThread = new SendTcpServer(&uplinkmsgqueue);
#endif
}
ListenApp::~ListenApp(){
    if(pqtSerialPort)
        delete pqtSerialPort;
    if(ptimer)
        delete ptimer;

}
bool ListenApp::setup(int brate,int time){
#ifdef ENABLE_SERIAL_TRANSPORT
    //  enumerateserialports();
    if(brate==9600){
        pqtSerialPort->setBaudRate(BAUD9600);
    }else if(brate == 115200){
         pqtSerialPort->setBaudRate(BAUD115200);
    }else if(brate == 230400){
        pqtSerialPort->setBaudRate(BAUD230400);
   }else{
        std::cout << "Baud Rate Not Supported Yet"<<std::endl;
        return 0;
    }
#endif

    QObject::connect(ptimer,SIGNAL(timeout()),this,SLOT(closeapp()));

#ifdef ENABLE_SERIAL_TRANSPORT
    if(pqtSerialPort->open(QIODevice::ReadWrite)==false){
        std::cout << "couldn't open the serial port";
        pqtSerialPort->close();
        return 0;
    }else{
        std::cout<<"serial port opened succesfully";
    }
    //first connect to dsr signal of pqtSerialPort
    connect(pqtSerialPort,SIGNAL(dsrChanged ( bool )),
            this,SLOT(serial_port_dsr_event(bool )));
    /*connect processor thread slot to serial port closed signal*/
    connect(this,SIGNAL(serial_port_closed()),processor,SLOT(stop_processing()));
    /*connect ivy downlink thread slot to serial port closed signal*/
    connect(this,SIGNAL(serial_port_closed()),ivy_dl_thread,SLOT(stop_processing()));
    /*connect ivy main loop  thread slot to serial port closed signal*/
    connect(this,SIGNAL(serial_port_closed()),pivyloopthread,SLOT(stop_processing()));
#ifdef ENABLE_ETHERNET_TRANSPORT
    /* connect closed app signal to ethernet send thread slot */
    connect(this,SIGNAL(serial_port_closed()),recvTcpServerThread,SLOT(closeConnection()));
    connect(this,SIGNAL(serial_port_closed()),sendTcpServerThread,SLOT(closeConnection()));
#endif
#endif

    /*set the timer interval*/
    ptimer->setInterval(time);
    /*start timer*/
    ptimer->start();

#ifdef ENABLE_SERIAL_TRANSPORT
    /*start reader*/
    reader->start();
    writer->start();
#endif

#ifndef SERIO_TESTING  
    /*start processor*/
    processor->start();
    ivy_dl_thread->start();
    pivyloopthread->start();
    //nps_com_thread->start(); //not needed now
#ifdef ENABLE_ETHERNET_TRANSPORT
    recvTcpServerThread->start();
    sendTcpServerThread->start();
#endif
#endif
    return true;
}

void ListenApp::closeapp(){
    std::cout<< "app is being closed";
    if(pqtSerialPort){
        pqtSerialPort->close();
    }
    if(ptimer){
        ptimer->stop();
    }
    emit serial_port_closed();
    /*Make every queue non blocking*/
    msgbuffqueue.makequeuenonblocking();
    ivyqueue.makequeuenonblocking();
    jsbsimqueue.makequeuenonblocking();
    telemsgcontainer.makequeuenonblocking();
    uplinkmsgqueue.makequeuenonblocking();
#ifdef ENABLE_ETHERNET_TRANSPORT
    //Sai:
    eth_uplinkmsgqueue.makequeuenonblocking();
    /*wait for finshing of both threads*/
#endif
    if(reader)
        reader->wait();
    if(writer)
        writer->wait();

    if(processor){
        processor->wait();
    }
    if(ivy_dl_thread){
        ivy_dl_thread->wait();
    }
    if(pivyloopthread){
        pivyloopthread->wait();
    }

#ifdef ENABLE_ETHERNET_TRANSPORT
    if (recvTcpServerThread) {
        recvTcpServerThread->wait();
    }
    if (sendTcpServerThread) {
        sendTcpServerThread->wait();
    }
#endif
    exit(0);
}

void ListenApp::serial_port_dsr_event(bool bstatus){
    if(bstatus)/*currently not using anything when connection starts*/
        return;
    qDebug()<<"serial port has been disconnected\n";
    closeapp();
}
