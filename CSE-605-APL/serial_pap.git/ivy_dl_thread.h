#ifndef IVY_DL_THREAD_H
#define IVY_DL_THREAD_H
#include <QtCore/QThread>
#include <QtCore/QVector>
#include <QtCore/QtGlobal>
#include "telemsg.h"
#include "synchqueue.h"
class CIvyDlThread:public QThread{
  Q_OBJECT
    public:
  CIvyDlThread(QSynchQueue<CTelemetryMsg*>* qu){
    bstop = false;
    pivymsgqueue=qu;
  }
  public Q_SLOTS:
  void stop_processing(){
    bstop = true;
  }
 protected:
  void run();
 private:
  bool bstop;
  QSynchQueue<CTelemetryMsg*>*pivymsgqueue;
};
#endif
