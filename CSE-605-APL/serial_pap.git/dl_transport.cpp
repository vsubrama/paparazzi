#include <iostream>
#include <strings.h>
#include "telemsg.h"
#include "downlink_transport.h"
#include "synchqueue.h"
#include "generated/airframe.h"
uint8_t DownlinkTransport::SizeOf (void *impl, uint8_t size){
  (void)impl;
  msg_len= size+extra_bytes;
  return msg_len;
}

int DownlinkTransport::CheckFreeSpace(void *impl, uint8_t size){
  (void)impl;
  (void)size;
  return true;
}


void DownlinkTransport::PutBytes(void *impl, enum DownlinkDataType data_type, uint8_t len, const void *bytes){
  (void)impl;
  char data[256];
  /*copying the input buffer to temp buffer for modifying the data.*/
  memcpy(data,bytes,len);
  uint8_t object_size=0;
  switch(data_type){
  case  DL_TYPE_ARRAY_LENGTH:
  case DL_TYPE_UINT8:
  case DL_TYPE_INT8:{
    object_size = 1;
  }
    break;
  case DL_TYPE_UINT16:
  case DL_TYPE_INT16:{
    object_size = 2;
  }
    break;
  case DL_TYPE_UINT32:
  case DL_TYPE_INT32:{
    object_size = 4;
  }
    break;
  case DL_TYPE_UINT64:
  case DL_TYPE_INT64:
  case DL_TYPE_DOUBLE:{
    object_size = 8;
  }
    break;
  case DL_TYPE_FLOAT:{
    object_size = 4;
  }
    break;
  case DL_TYPE_TIMESTAMP:
  default:{
    std::cout <<"Not defined yet" << std::endl;
  }
  }
  
  if(object_size==0)
    std::cout <<"incorrect object size" << std::endl;
  
  int index = 0;
  while(index < len){
    CMsg::LittleToBig(&data[index],object_size);
    index+=object_size;
  }
  
  for(index=0;index<len;index++){
    ck_a +=data[index];
    ck_b +=ck_a;
  }

  memcpy(&buffer[cur_index],data,len);
  cur_index+=len;
}
void DownlinkTransport::StartMessage(void *impl, const char *name, uint8_t msg_id, uint8_t payload_len){
  (void)impl;
  (void)payload_len;
  (void)name;
  if(cur_index!=0 || ck_a !=0 || ck_b!=0){
    std::cout << "previous msg has not been fixed yet"<<std::endl;
  }

  /*put the header in the buffer*/
  /*first put STX bytes in the buffer*/
  buffer[cur_index++]=CMsg::STX;
  /*now put msg_len in the buffer and start recording crc*/
  buffer[cur_index++]=msg_len;
  ck_a = msg_len,ck_b = msg_len;
  
  /*Put AC_ID.*/
  buffer[cur_index++]=AC_ID;
  ck_a +=AC_ID,ck_b +=ck_a;
  /*now put msg_id.*/
  buffer[cur_index++]=msg_id;
  ck_a +=msg_id,ck_b +=ck_a;

  return;
}

void DownlinkTransport::EndMessage(void *impl){
  (void)impl;

  buffer[cur_index++]= ck_a;
  buffer[cur_index++]= ck_b;
  
  if(cur_index != msg_len){
    std::cout <<std::endl << "error: error in constructing the msg frame"<<std::endl;
  }
  /*copy the buffer in msg queue*/
  char* data = new char[msg_len];
  memcpy(data,buffer,msg_len);
  
  /*This is a thread safe queue.*/
  ptransmitqueue->enqueue(data);
  /*Now reset everything*/
  /*always zero the buffer*/
  bzero(buffer,MAX_BYTE);
  /*reset the cur_index*/
  cur_index=0;
  /*reset both crc1 and crc2 to zero*/
  ck_a=ck_b=0;
  msg_len = 0;
}
void DownlinkTransport::Overrun(void *impl){
  (void)impl;
}
void DownlinkTransport::CountBytes(void *impl, uint8_t len){
  (void)impl;
  (void)len;
}
void DownlinkTransport::Periodic(void *impl){
  (void)impl;
}


