/*
 * sendtcpserver.h
 *
 *  Created on: Apr 7, 2014
 *      Author: rtdroid
 */

#ifndef SENDTCPSERVER_H_
#define SENDTCPSERVER_H_

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include "synchqueue.h"
#include <QtCore/QThread>
#include <QtCore/QString>

class SendTcpServer : public QThread
{
    Q_OBJECT
public:
    SendTcpServer(QSynchQueue<ethernetMessage>*eth_upQueue);

protected:
 void run();
 QString getPrettyMsg(char*);

signals:

public Q_SLOTS:
    void closeConnection();

private:
    QSynchQueue<ethernetMessage>* upQueue;
    char* buffer;
    int sendPort;
    bool close;
};

#endif // SENDTCPSERVER_H_

