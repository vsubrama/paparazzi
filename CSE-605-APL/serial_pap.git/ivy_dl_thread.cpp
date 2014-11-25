#include "ivy_dl_thread.h"
#include <Ivy/ivy.h>
#include <iostream>
void CIvyDlThread::run(){
  while(!bstop){
    char* buffer=NULL;
    CTelemetryMsg *pmsg = pivymsgqueue->dequeue();
    if(pmsg==NULL)
      continue;
    int len =  pmsg->getBufferedMsg(&buffer);
    if(buffer==NULL || len==0)
      continue;
     IvySendMsg("%s",buffer);
    delete [] buffer;
    delete pmsg;
  }
}

