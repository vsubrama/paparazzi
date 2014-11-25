#include "messages.h"
#include "msgfactory.h"
#include "telemsg.h"
#include "common.h"
CTelemetryMsg* CMsgFactory::CreateMsg(quint8 msg_id,quint8 len, char* buff )
{
  CTelemetryMsg* pmsg=NULL;
  (void)len;
  switch(msg_id){
  case DL_GPS:
    pmsg = new GPSMsg(buff);
    break;
  case DL_IR_SENSORS:
    pmsg = new IRMsg(buff);
    break;
  case DL_IVY_MSG_ID:
    /*IVY_START+LEN+PAYLOAD+IVY_END*/
    if(len >IvyMsg::MARKER_BYTES){
      pmsg = new IvyMsg(&buff[IvyMsg::MARKER_BYTES-1],len-IvyMsg::MARKER_BYTES);
    }
    break;
  default:
    pmsg = NULL;
    break;
  }
  return pmsg;
}

