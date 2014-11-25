#include <iostream>
#include <strings.h>
#include "telemsg.h"
#include "synchqueue.h"
#include "generated/airframe.h"
#include "hitl_transport.h"

#define HITL_TRANSPORT     PprzTransport
#define HITL_DEVICE        Fn_



#define _trans               HITL_TRANSPORT
#define _dev                 HITL_DEVICE
#define DownlinkPut(_trans,_dev,_type,barray,len,data) if(barray){  \
{  DownlinkPut##_type##Array(_trans,_dev,len,data);}	    \
    }else{							    \
{DownlinkPut##_type##ByAddr(_trans,_dev,data);}}

void HitlTransport::Fn_Transmit(uint8_t _x){
    buffer[cur_index++]=_x;
    //std::cout<<cur_index << "-" << buffer[cur_index]<<":";
}

bool_t HitlTransport::Fn_CheckFreeSpace(uint8_t _x){
    (void)_x;
    return true;
}

void HitlTransport::Fn_SendMessage(){
    return;
}

uint8_t HitlTransport::SizeOf (void *impl, uint8_t size){
    (void)impl;
    return DownlinkSizeOf(HITL_TRANSPORT,HITL_DEVICE,size);
}

int HitlTransport::CheckFreeSpace(void *impl, uint8_t _x){
    (void)impl;
    return  DownlinkCheckFreeSpace(HITL_TRANSPORT,HITL_DEVICE, _x);
}

void HitlTransport::PutBytes(void *impl, enum DownlinkDataType data_type, uint8_t len, const void *bytes){
    (void)impl;
    static bool_t b_cur_array = 0;


    switch(data_type){
    case DL_TYPE_UINT8:{
        DownlinkPut(_trans, _dev,Uint8,b_cur_array,len,((const uint8_t*)bytes));
    }
        break;
    case DL_TYPE_UINT16:{
        DownlinkPut(_trans, _dev,Uint16,b_cur_array,len,((const uint16_t*)bytes));
    }
        break;
    case DL_TYPE_INT16:{
        DownlinkPut(_trans, _dev,Int16,b_cur_array,len,((const int16_t*)bytes));
    }
        break;
    case DL_TYPE_UINT32:{
        DownlinkPut(_trans, _dev,Uint32,b_cur_array,len,((const uint32_t*)bytes));
    }
        break;
    case DL_TYPE_INT32:{
        DownlinkPut(_trans, _dev,Int32,b_cur_array,len,((const int32_t*)bytes));
    }
        break;
    case DL_TYPE_DOUBLE:{
        DownlinkPut(_trans, _dev,Double,b_cur_array,len,((const double*)bytes));
    }
        break;
    case DL_TYPE_FLOAT:{
        DownlinkPut(_trans, _dev,Float,b_cur_array,len,((const float*)bytes));
    }
        break;

    case DL_TYPE_ARRAY_LENGTH:{
    }
        break;
    case DL_TYPE_INT8:
    case DL_TYPE_UINT64:
    case DL_TYPE_INT64:
    default :{
        std::cout << "No Type Matched in HITL_TRANSPORT PutBytes" << std::endl;
    }
    }


    /*A litle patch for recognizing the array.*/
    if(data_type==DL_TYPE_ARRAY_LENGTH){
        /*Record the array len.*/
        b_cur_array = 1;
    }else{
        b_cur_array = 0;
    }

    return;
}
void HitlTransport::InitData(){
    cur_index=0;
    ck_a=0;ck_b=0;
    downlink_nb_ovrn=0; downlink_nb_bytes=0;
    downlink_nb_bytes=0;

}
void HitlTransport::StartMessage(void *impl,const char *name, uint8_t msg_id, uint8_t payload_len){
    (void)impl;
    InitData();
    DownlinkStartMessage(HITL_TRANSPORT,HITL_DEVICE, _name, msg_id, payload_len);
    return;
}

void HitlTransport::EndMessage(void *impl){
    (void)impl;
    DownlinkEndMessage(HITL_TRANSPORT,HITL_DEVICE);
    /*copy the buffer in msg queue*/
    char* data = new char[cur_index];
    ethernetMessage m;
    int i =0;
    for(i=0;i<cur_index;i++){
        m.msg[i] = buffer[i];
    }
    m.id = VALID_MESSAGE_ID;
    m.length = cur_index;

    /*This is a thread safe queue.*/
    ptransmitqueue->enqueue(m);
    return;
}
void HitlTransport::Overrun(void *impl){
    (void)impl;
    DownlinkOverrun(HITL_TRANSPORT,HITL_DEVICE);
}
void HitlTransport::CountBytes(void *impl, uint8_t len){
    (void)impl;
    DownlinkCountBytes(HITL_TRANSPORT,HITL_DEVICE,len);
}
void HitlTransport::Periodic(void *impl){
    (void)impl;
    std::cout<<"HitlTransport::Periodic() Not Implemented"<<std::endl;
    return;
}


