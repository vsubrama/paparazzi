#include "recv_tcpserver.h"
#include "common.h"
#include <iostream>
#include <QtCore/QDebug>

RecvTcpServer::RecvTcpServer(QSynchQueue<ethernetMessage>*eth_downQueue)
{
    recvPort = 9999;
    downQueue = eth_downQueue;
    close = false;
    ref = this;
}

void RecvTcpServer::run(){
    tcpServer = new QTcpServer();
    connect(tcpServer, SIGNAL(newConnection()), this, SLOT(newConnection()), Qt::DirectConnection);
    if(!tcpServer->listen(QHostAddress::Any, recvPort))
    {
        std::cout << " RecvTcpServer: Server could not start"<<std::endl;
    }
    else
    {
        if (tcpServer->isListening()) {
            std::cout << " RecvTcpServer : Server started!"<<std::endl;
        } else {
            std::cout << " RecvTcpServer: not started!"<<std::endl;
        }
    }
    QThread::exec();
}

inline int RecvTcpServer::waitForInput( QTcpSocket *socket )
{
    mutex.lock();
    int bytesAvail = -1;
    while (socket->state() == QAbstractSocket::ConnectedState && bytesAvail < 0) {
        if (socket->waitForReadyRead( 20 )) {
            bytesAvail = socket->bytesAvailable();
        }
        else {
            //std::cout << "waiting for input" << std::endl;
            msleep(1);
        }
    }
    mutex.unlock();
    return bytesAvail;
}

void RecvTcpServer::newConnection()
{  
#define MAX_SIZE 256
    std::cout << " RecvTcpServer: in new connection!"<<std::endl;
    QTcpSocket *socket = tcpServer->nextPendingConnection();

    char* buffer = new char[MAX_ETHERNET_MSG_BYTES];
    qint64 sizeOfbuff = 0;
    while(socket->state() == QAbstractSocket::ConnectedState && !close){

        sizeOfbuff = socket->bytesAvailable();

        //Some min size for a message, don't want to read single bytes
        if (sizeOfbuff > 100) {
            mutex.lock();
            qint64 status = socket->read(buffer, MAX_ETHERNET_MSG_BYTES);

            if(status >0){
                //Copy the new message into the container and put into the queue
                ethernetMessage m;
                memcpy(m.msg,buffer,status);
                m.id = VALID_MESSAGE_ID;
                m.length = status;
                downQueue->enqueue(m);
            }else if(status < 0){
                std::cout << "closing socket with error " << status << std::endl;
                //The connection is down
                socket->close();
                close = true;
            }

            memset(buffer,0x55,MAX_ETHERNET_MSG_BYTES);
            mutex.unlock();
        }else{
            RecvTcpServer::waitForInput(socket);
        }
    }

    socket->close();

    /*
    std::cout << " RecvTcpServer: in new connection!"<<std::endl;
    QTcpSocket *socket = tcpServer->nextPendingConnection();

    //socket->waitForBytesWritten(10000);
    //socket->waitForReadyRead(10000);

    qint64 sizeOfbuff = RecvTcpServer::waitForInput(socket);
    while(socket->state() == QAbstractSocket::ConnectedState && !close){
        //Some min size for a message, don't want to read single bytes
        if (sizeOfbuff > 128) {
            char* buffer = new char[sizeOfbuff];
            qint64 status = socket->read(buffer, sizeOfbuff);
            std::cout << getPrettyMsg(buffer).toStdString() << std::endl;
            std::cout << buffer <<std::endl;
            if (status > 0) {
                downQueue->enqueue(buffer);
            }
            else if(status < 0){
                std::cout << "closing socket with error " << status << std::endl;
                //The connection is down (0 is the return code)
                socket->close();
                close = true;
            }
            bzero(buffer, sizeOfbuff);
        } else {
            sizeOfbuff = RecvTcpServer::waitForInput(socket);
        }
    }
    socket->close();
*/ 
}

QString RecvTcpServer::getPrettyMsg(char* msg){
    QString str;
    if(msg)
        str+=msg;
    return str;
}

void RecvTcpServer::closeConnection()
{
    close = true;
    tcpServer->close();
}
