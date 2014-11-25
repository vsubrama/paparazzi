/*
 * sendtcpserver.cpp
 *
 *  Created on: Apr 7, 2014
 *      Author: rtdroid
 */

#include "sendtcpserver.h"
#include "common.h"
#include <QtCore/QDebug>
#include <iostream>
#include <QEventLoop>
#include <QTime>

SendTcpServer::SendTcpServer(QSynchQueue<ethernetMessage>*eth_upQueue)
{
    upQueue= eth_upQueue;
    sendPort = 10000;
    close = false;
}
void SendTcpServer::run(){

    std::cout <<"Sender sleeping for 20s \n";
    sleep(20);
    QHostAddress hostAddr( "10.42.0.2" );
    QTcpSocket* socket = new QTcpSocket();
    std::cout <<"Created Socket Object \n";
    socket->connectToHost( hostAddr, sendPort );
    std::cout <<"connectToHost()\n";
    if (socket->waitForConnected( 300000 )){
        std::cout << " sendTcpServer: in new connection!"<<std::endl;
        while (socket->state() == QAbstractSocket::ConnectedState && !close){
            if(upQueue->get_size()>0){
                QTime time = QTime::currentTime();
                QString timeString = time.toString();
                std::cout<< timeString.toStdString() << "TxThread:" << time.msec() << " " << upQueue->get_size() << " elements remaining \r\n";
            }

            if(upQueue->get_size()){

                ethernetMessage msg;
                msg = upQueue->dequeue();
                if(msg.id == VALID_MESSAGE_ID){
                    //std::cout << ">> " << getPrettyMsg(msg.msg).toStdString() << std::endl;
                    socket->write(msg.msg,msg.length);
                    socket->flush();
                }

            }else
                msleep(1);

        }
    }
    std::cout << "No connection, exiting\n";
    socket->close();
}

QString SendTcpServer::getPrettyMsg(char* msg){
    QString str("");
    if(msg)
        str+=msg;
    return str;
}

void SendTcpServer::closeConnection()
{
    close = true;
}



