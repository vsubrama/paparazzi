#include <stdio.h>
#include "subsystems/datalink/transport.h"

extern char ivy_buf[];
extern char* ivy_p;

#define IVY_START 0x7E
#define IVY_END   0x7F
/*IVY_START + LEN_BUFFER + IVY_END */

char *plen =&ivy_buf[1];

#define IvyTransportCheckFreeSpace(_dev,_) TRUE
#define IvyTransportSizeOf(_dev, _payload) (_payload)
#define IvyTransportPut1Byte(_dev,_x)  { \
    int bytes =sprintf(ivy_p,"%c",_x);		 \
    (*plen)+=bytes;				 \
    ivy_p+=bytes;				 \
  }
#define IvyTransportByte(_dev,_x)  TransportLink(_dev, Transmit(_x))

#define IvyTransportHeader(_dev,len) { \
    ivy_p=ivy_buf;		       \
    (*plen)=0;				\
    IvyTransportPut1Byte(_dev,IVY_START);	\
    IvyTransportPut1Byte(_dev,*plen);		\
  }


#define IvyTransportTrailer(_dev) {		\
  IvyTransportPut1Byte(_dev,'\0');		\
  IvyTransportPut1Byte(_dev,IVY_END);		\
  uint8_t _i=0;					\
  while(_i<*plen) {				\
  IvyTransportByte(_dev,ivy_buf[_i]);		\
  _i++;						\
  }						\
  }

#define IvyTransportPutUint8(_dev,x) {		\
    int bytes= sprintf(ivy_p, "%u ", x);	\
    (*plen)+=bytes;				\
    ivy_p+=bytes;				\
  }

#define IvyTransportPutNamedUint8(_dev,_name, _x) {	\
    int bytes = sprintf(ivy_p, "%s ", _name);		\
    (*plen)+=bytes;					\
    ivy_p+=bytes;					\
  }

#define Space() {					\
    int bytes = srintf(ivy_p, " ");			\
    (*plen)+=bytes;					\
    ivy_p+=bytes;					\
  }



#define Comma() {					\
    int bytes  = sprintf(ivy_p, ",");			\
    (*plen)+=bytes;					\
    ivy_p+=bytes;					\
  }

#define IvyTransportPutUintByAddr(_dev,x) {		\
    int bytes = sprintf(ivy_p, "%u", *x);				\
    (*plen)+=bytes;					\
    ivy_p+=bytes;					\
  }
#define IvyTransportPutUint8ByAddr(_dev,x) IvyTransportPutUintByAddr(_dev,x) Space()
#define IvyTransportPutUint16ByAddr(_dev,x) IvyTransportPutUintByAddr(_dev,x) Space()
#define IvyTransportPutUint32ByAddr(_dev,x) IvyTransportPutUintByAddr(_dev,x) Space()

#define IvyTransportPutIntByAddr(_dev,x){ \
  int bytes = sprintf(ivy_p, "%d", *x);\
  (*plen)+=bytes;					\
  ivy_p+=bytes;						\
}
#define IvyTransportPutInt8ByAddr(_dev,x) IvyTransportPutIntByAddr(_dev,x) Space()
#define IvyTransportPutInt16ByAddr(_dev,x) IvyTransportPutIntByAddr(_dev,x) Space()
#define IvyTransportPutInt32ByAddr(_dev,x) IvyTransportPutIntByAddr(_dev,x) Space()

#define IvyTransportPutOneFloatByAddr(_dev,x) { \
    int bytes = sprintf(ivy_p, "%f", *x);		\
    (*plen)+=bytes;					\
    ivy_p+=bytes;					\
  }

#define IvyTransportPutFloatByAddr(_dev,x) IvyTransportPutOneFloatByAddr(_dev,x) Space()
#define IvyTransportPutDoubleByAddr(_dev,x) IvyTransportPutOneFloatByAddr(_dev,x) Space()


#define IvyTransportPutArray(_dev,_put, _n, _x) { \
  int __i; \
  for(__i = 0; __i < _n; __i++) { \
    _put(_dev,&_x[__i]); \
    Comma(); \
  } \
}

#define IvyTransportPutUint8Array(_dev,_n, _x) IvyTransportPutArray(_dev,IvyTransportPutUintByAddr, _n, _x)
#define IvyTransportPutInt16Array(_dev,_n, _x) IvyTransportPutArray(_dev,IvyTransportPutIntByAddr, _n, _x)
#define IvyTransportPutUint16Array(_dev,_n, _x) IvyTransportPutArray(_dev,IvyTransportPutUintByAddr, _n, _x)
#define IvyTransportPutUint32Array(_dev,_n, _x) IvyTransportPutArray(_dev,IvyTransportPutUintByAddr, _n, _x)
#define IvyTransportPutFloatArray(_dev,_n, _x) IvyTransportPutArray(_dev,IvyTransportPutOneFloatByAddr, _n, _x)
#define IvyTransportPutDoubleArray(_dev,_n, _x) IvyTransportPutFloatArray(_dev,_n, _x)
