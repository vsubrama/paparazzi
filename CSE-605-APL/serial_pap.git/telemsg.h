#ifndef CMSG_H
#define CMSG_H
#include <QtCore/QString>
#include <QtCore/QtGlobal>

class CMsg{
 public:
  enum estart_byte{STX=0x99,IVY_START=0x7E};
  enum end_byte{IVY_END=0x7F};
 public:
  static void BigToLittle(char* buff, unsigned int nbytes);
  static void LittleToBig(char* buff, unsigned int nbytes);
};


class CTelemetryMsg:public CMsg{
 public: 
  static  const quint8 STX=0x99;

 public:
  virtual ~CTelemetryMsg(){
    if(msgbuffer)
      delete [] msgbuffer;
  }
  virtual bool isJSBSIMmsg(){
    return false;
  }
 public:
  //allocate and fill buffer and return it's length.
  virtual int getBufferedMsg(char** buffer)=0;
  //Return a formatted string which can be directly
  //stramed to std output.
  virtual QString getPrettyMsg()=0;
  virtual quint8 getmsglength()=0;
  virtual quint8 getmsgid()=0;
 protected  :
  char* msgbuffer;
};


class GPSMsg : public CTelemetryMsg{
 public:  
  GPSMsg();
  GPSMsg( char* buff);
  ~GPSMsg(){
  }
 private:
   static quint8 msg_id;
   static quint8 msg_len;

 public:
  virtual int getBufferedMsg(char** buffer);
  virtual QString getPrettyMsg();
  virtual quint8 getmsglength();
  virtual quint8 getmsgid();

 protected:
  quint8  gps_mode;
  quint32 gps_utm_east;
  quint32 gps_utm_north;
  quint16 gps_course;
  quint32 gps_alt;
  quint16 gps_speed;
  quint16 gps_climb;
  quint16  gps_week;
  quint32 gps_tow;
  quint8  gps_utm_zone;
  quint8  gps_nb_err;
};
class IRMsg : public CTelemetryMsg{
 public:
  IRMsg();
  IRMsg(char* buff);
 public:
  virtual int getBufferedMsg(char** buffer);
  virtual QString getPrettyMsg();
  virtual quint8 getmsglength();
  virtual quint8 getmsgid();
  
 public:
  static quint8 msg_id;
   static quint8 msg_len;
  
 private:
  quint16 ir_ir1;
  quint16 ir_ir2;
  quint16 ir_longitudinal;
  quint16 ir_lateral;
  quint16 ir_vertical;
};

class IvyMsg : public CTelemetryMsg{
 public:
  IvyMsg();
  ~IvyMsg();
  IvyMsg( char* buff,int msg_len);
 public:
  virtual int getBufferedMsg(char** buffer);
  virtual QString getPrettyMsg();
  virtual quint8 getmsglength();
  virtual quint8 getmsgid();
  virtual bool isJSBSIMmsg();

 public:
  static const  int MARKER_BYTES = 3 ;/*IVY_START+LEN+IVY_LEN*/\
  static bool verifyMsg(const char* buffer);
 protected:
  int msg_len;
};



#endif 

