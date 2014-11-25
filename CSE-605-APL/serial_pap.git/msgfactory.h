#ifndef MSG_FACTORY_H
#define MSG_FACTORY_H

#include "telemsg.h"
class CMsgFactory{
 public:
  static CTelemetryMsg* CreateMsg(quint8 msg_id,quint8 len, char* buff );
};

#endif 
