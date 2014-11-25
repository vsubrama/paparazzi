#ifndef MYTCPSERVER_H
#define MYTCPSERVER_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QtCore/QThread>
#include <QtCore/QString>
#include "synchqueue.h"
#include "common.h"

class RecvTcpServer : public QThread
{
    Q_OBJECT
public:
    RecvTcpServer(QSynchQueue<ethernetMessage>*eth_downQueue);

protected:
    void run();
    QString getPrettyMsg(char*);

signals:

public Q_SLOTS:
    void newConnection();
    void closeConnection();

private:
    int waitForInput( QTcpSocket *socket );
    QTcpServer *tcpServer;
    QSynchQueue<ethernetMessage>* downQueue;
    int recvPort;
    bool close;
    RecvTcpServer *ref;
    QMutex mutex;
};

#endif // MYTCPSERVER_H
