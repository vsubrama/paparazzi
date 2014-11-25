#ifndef NPS_THREAD_H
#define NPS_THREAD_H
#include <string>
#include <iostream>

#include <QtCore/QThread>
#include <QtCore/QTimer>

#include "telemsg.h"
#include "synchqueue.h"


class NpsCommThread : public QThread{
    Q_OBJECT
public:
    NpsCommThread(QSynchQueue<CTelemetryMsg*>*pQueue);
    ~NpsCommThread(){
        if(psimtimer){
            psimtimer->stop();
            delete psimtimer;
        }
    }
protected:
    void run();

private Q_SLOTS:
    void nps_comm_periodic();
    void stop();
private:
    void copy_autopilot_commands_to_nps();

private:
    QSynchQueue<CTelemetryMsg*>*ptelemetry_msg_queue;
    QTimer *psimtimer;
    static  quint8 gps_id;
    static  quint8 ac_id;
    static  quint8 ir_id;

};

#endif // NPS_THREAD_H
