#include <Ivy/ivy.h>
#include "nps_thread.h"
#include "generated/airframe.h"
#include <QtCore/QtGlobal>
#include <QtCore/QStringList>

quint8 NpsCommThread::gps_id=1;
quint8 NpsCommThread::ac_id=AC_ID;
quint8 NpsCommThread::ir_id=1;

NpsCommThread::NpsCommThread(QSynchQueue<CTelemetryMsg*>*pQueue){
    ptelemetry_msg_queue = pQueue;
}

void NpsCommThread::nps_comm_periodic(){
    bool result;
    /* read actuators positions and feed JSBSim inputs */
    copy_autopilot_commands_to_nps();
}

void NpsCommThread::copy_autopilot_commands_to_nps(){

    static double throttle_slewed = 0.;
    static double th = 0.;
    int msg_queue_size=0;

    msg_queue_size = ptelemetry_msg_queue->get_size();
    if(msg_queue_size==0){
        /*don't block*/
        return;
    }

    CTelemetryMsg* pmsg = ptelemetry_msg_queue->dequeue();
    if(pmsg==NULL){
        return;
    }

    qint16 *commands = new qint16[COMMANDS_NB];

    char* payload=NULL;
    pmsg->getBufferedMsg(&payload);
    if(payload==NULL){
        delete[] pmsg;
        return;
    }else{
        std::cout << "Autopilot FBW msg received " << payload << std::endl;
    }
    QString qpayload(payload);
    QStringList strList =qpayload.split(" ",QString::SkipEmptyParts);

    quint8 ac_id;
    QString msg_name;
    QString qcommands_array;

    if(qpayload.size()<3){
        std::cout << "Incomplete Autopilot FBW Msg Received " << std::endl;
    }

    ac_id = strList[0].toUInt();
    msg_name = strList[1];
    if(msg_name != "COMMANDS"){
        std::cout << "Incorrect Autopilot FBW Msg Received " << std::endl;
    }

    qcommands_array = strList[2];

    QStringList commands_value = qcommands_array.split(",",QString::SkipEmptyParts);

    if(commands_value.size()!=COMMANDS_NB){
        std::cout << "Incomplete Commands array Received " << std::endl;
    }

    for(int i=0; i<COMMANDS_NB;i++){
        commands[i] = commands_value[i].toUInt();
    }

    //Here are the full set of commands to put into the IvyBusMessage
    //TODO Place them on the bus here
    IvySendMsg("%d NPS_ATP_CMD %d %d %d %d", ac_id, commands[0], commands[1], commands[2], commands[3]);

    /*Delete the buffer now.*/
    if(payload)
        delete [] payload;
    if(pmsg)
        delete pmsg;

}

void NpsCommThread::stop(){
    if(psimtimer)
        psimtimer->stop();
    exit(0);
}



void NpsCommThread::run(){
    psimtimer = new QTimer(0);
    //TODO DEBUG MODE, increase timing in future
    //Set the interval to 500 milliseconds. NPS runs at ~8 so this should suffice
    psimtimer->setInterval(500);
    psimtimer->moveToThread(this);
    connect(psimtimer,SIGNAL(timeout()),this,SLOT(nps_comm_periodic()));
    psimtimer->start();
    QThread::exec();
}



