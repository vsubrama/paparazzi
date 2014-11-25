#include <iostream>
#include <string>
#include <Ivy/ivy.h>
#include <Ivy/ivyloop.h>
#include <QDateTime>
#include "messages.h"
#include "common.h"
#include "dl_protocol2.h"
#include "hitl_transport.h"
#include "ivythread.h"


using namespace std;
#ifdef __APPLE__
string ivyBus = "224.255.255.255";
#else
string ivyBus = "127.255.255.255";
#endif
string fgAddress = "127.0.0.1";

using namespace std;


static quint64 mod_value(QString string,qint8* psign, bool* ok){
    qint64 value = string.toLongLong(ok);
    (value<0) ? (*psign = -1) : (*psign = 1);
    value*=(*psign)	   ;
    if(value<0) std::cout << "incorrect conversion of type" << std::endl;
    return value;
}

/*
 *This is a generic function which converts a string representation
 *into it's byte form.
*/
static bool fill_buffer_array(const char* const* argv,DownlinkDataType typev[],unsigned int argc,char* buffer,unsigned int bytes){
    quint32 buffer_index = 0;
    quint32 argv_index = 0;
    quint8 ob_size =0;
    qint8 sign;
    bool ok=false;
    QString argstring;
    for(;argv_index<argc;argv_index++){
        argstring = argv[argv_index];
        if(buffer_index > bytes ){
            std::cout << "buffer overflowed"<<std::endl;
        }
        switch(typev[argv_index]){
        case DL_TYPE_TIMESTAMP:
            echo_data_type_error("string","DL_TYPE_TIMESTAMP");
            break;
        case DL_TYPE_ARRAY_LENGTH:
        case DL_TYPE_UINT8:{
            ob_size = sizeof(quint8);
            quint8* p = (quint8*)(&buffer[buffer_index]);
            /*because these values are already unsigned we
    need not be bother about the sign of the data*/
            *p = argstring.toUInt(&ok);
            if(ok==false) echo_data_type_error("string","DL_TYPE_UINT8");
        }
            break;
        case DL_TYPE_UINT16:{
            ob_size = sizeof(quint16);
            quint16* p = (quint16*)(&buffer[buffer_index]);
            /*because these values are already unsigned and conversion
    from bigger datatype to small data type will not hurt.*/
            *p = argstring.toUInt(&ok);
            if(ok==false) echo_data_type_error("string","DL_TYPE_UINT16");
        }
            break;
        case DL_TYPE_UINT32:{
            ob_size = sizeof(quint32);
            quint32* p = (quint32*)(&buffer[buffer_index]);
            /*because these values are already unsigned and conversion
    from bigger datatype to small data type will not hurt.*/
            *p = argstring.toUInt(&ok);
            if(ok==false) echo_data_type_error("string","DL_TYPE_UINT32");
        }
            break;
        case DL_TYPE_UINT64:{
            ob_size = sizeof(qulonglong);
            qulonglong* p = (qulonglong*)(&buffer[buffer_index]);
            /*because these values are already unsigned and conversion
    from bigger datatype to small data type will not hurt.*/
            *p = argstring.toULongLong(&ok);
            if(ok==false) echo_data_type_error("string","DL_TYPE_UINT32");
        }
            break;
        case DL_TYPE_INT8:{
            ob_size = sizeof(qint8);
            qint8* p = (qint8*)(&buffer[buffer_index]);
            /*because these values are already unsigned we
    need not be bother about the sign of the data*/
            *p= (qint8)(mod_value(argstring,&sign,&ok));
            *p *=sign;
            if(ok==false) echo_data_type_error("string","DL_TYPE_INT8");
        }
            break;
        case DL_TYPE_INT16:{
            ob_size = sizeof(qint16);
            qint16* p = (qint16*)(&buffer[buffer_index]);
            *p=(qint16)(mod_value(argstring,&sign,&ok));
            *p *=sign;
            if(ok==false) echo_data_type_error("string","DL_TYPE_INT16");
        }
            break;
        case DL_TYPE_INT32:{
            ob_size = sizeof(qint32);
            qint32* p = (qint32*)(&buffer[buffer_index]);
            *p=(qint32)(mod_value(argstring,&sign,&ok));
            *p *=sign;
            if(ok==false) echo_data_type_error("string","DL_TYPE_INT32");
        }
            break;
        case DL_TYPE_INT64:{
            ob_size = sizeof(qint64);
            qint64* p = (qint64*)(&buffer[buffer_index]);
            *p=(qint64)(mod_value(argstring,&sign,&ok));
            *p *=sign;
            if(ok==false) echo_data_type_error("string","DL_TYPE_INT64");
        }
            break;
        case DL_TYPE_FLOAT:{
            /*Though we do have qreal type for coverting a float or
    double value.But I am not sure aout the size of qreal. Therefore
    to be safe I am convert this value to raw float.*/
            ob_size = sizeof(float);
            float* p = (float*)(&buffer[buffer_index]);
            *p = argstring.toFloat(&ok);
            if(ok==false) echo_data_type_error("string","DL_TYPE_FLOAT");
        }
            break;
        case DL_TYPE_DOUBLE:{
            /*Though we do have qreal type for coverting a float or
    double value.But I am not sure aout the size of qreal. Therefore
    to be safe I am convert this value to raw float.*/
            ob_size = sizeof(double);
            double* p = (double*)(&buffer[buffer_index]);
            *p = argstring.toDouble(&ok);
            if(ok==false) echo_data_type_error("string","DL_TYPE_DOUBLE");
        }
            break;
        default:
            echo_switch_nomatch_error("fill_buffer_array");
        }
        if(!ok) return false;
        buffer_index+=ob_size;
    }
    return true;
}


static void on_DL_PING(IvyClientPtr app __attribute__ ((unused)),
                       void *user_data ,
                       int argc , char *argv[] __attribute__ ((unused))){

    DownlinkTransport *pchannel = (DownlinkTransport*)user_data;
    if(argc!=1){
        echo_msg_arg_error("DL_PING");
    }

    /*DL_PING doesn't have any important number of arguments*/
    DOWNLINK_SEND_PING(pchannel);

}

static void on_DL_ACINFO(IvyClientPtr app __attribute__ ((unused)),
                         void *user_data ,
                         int argc , char *argv[]){

    int index = 0,nbytes=0;
    QString qstring;
    char* buffer;

    DownlinkDataType typev[]= {
        DL_TYPE_INT16,
        DL_TYPE_INT32,
        DL_TYPE_INT32,
        DL_TYPE_INT32,
        DL_TYPE_UINT32,
        DL_TYPE_UINT16,
        DL_TYPE_INT16,
        DL_TYPE_UINT8
    };

    if(argc!=9){
        echo_msg_arg_error("DL_ACINFO");
    }

    nbytes=2+4+4+4+4+2+2+1;
    buffer = new char[nbytes];
    index = 1;
    if(fill_buffer_array(&argv[index],typev,sizeof(typev)/sizeof(DownlinkDataType),buffer,nbytes)==false){
        std::cout<<"some error occured in while constructing the arg buffer"<<std::endl;
    }
    
    DownlinkTransport *pchannel = (DownlinkTransport*)user_data;
    
    int buffer_index=0;
    qint16 course =((quint16*)(buffer))[buffer_index];
    buffer_index+=sizeof(course);

    qint32 utm_east =((quint32*)(buffer))[buffer_index];
    buffer_index+=sizeof(utm_east);


    qint32 utm_north = ((quint32*)(buffer))[buffer_index];
    buffer_index+=sizeof(utm_north);
    
    qint32 alt = ((quint32*)(buffer))[buffer_index];
    buffer_index+=sizeof(alt);



    quint32 itow = ((quint32*)(buffer))[buffer_index];
    buffer_index+=sizeof(itow);


    quint16 speed = ((quint16*)(buffer))[buffer_index];
    buffer_index+=sizeof(speed);


    qint16 climb= ((quint16*)(buffer))[buffer_index];
    buffer_index+=sizeof(climb);


    quint8 ac_id= ((quint8*)(buffer))[buffer_index];
    buffer_index+=sizeof(ac_id);

    DOWNLINK_SEND_ACINFO(pchannel,
                         &course,
                         &utm_east,
                         &utm_north,
                         &alt,
                         &itow,
                         &speed,
                         &climb,
                         &ac_id);
    

    delete [] buffer;
    return;
}

static void on_DL_SETTING(IvyClientPtr app __attribute__ ((unused)),
                          void *user_data ,
                          int argc , char *argv[]){
    int index = 0,nbytes=0;
    QString qstring;
    char* buffer;

    DownlinkDataType typev[]= {
        DL_TYPE_UINT8,
        DL_TYPE_UINT8,
        DL_TYPE_FLOAT
    };

    if(argc!=4){
        echo_msg_arg_error("DL_SETTING");
    }
    /*
    qstring = argv[index++];
    nbytes = qstring.toInt(&ok);
    if(ok==false){
    echo_msg_conversion_error("bytes","int");
    }
    qstring = argv[index++];
    if(qstring!="DL_SETTING"){
    echo_msg_arg_error("DL_SETTING");
    }*/

    nbytes=1+1+4;
    buffer = new char[nbytes];
    index=1;
    if(fill_buffer_array(&argv[index],typev,sizeof(typev)/sizeof(DownlinkDataType),buffer,nbytes)==false){
        std::cout<<"some error occured in while constructing the arg buffer"<<std::endl;
    }
    
    DownlinkTransport *pchannel = (DownlinkTransport*)user_data;
    
    int buffer_index=0;
    quint8 _index = ((quint8*)(buffer))[buffer_index];
    buffer_index+=sizeof(_index);

    quint8 ac_id = ((quint8*)(buffer))[buffer_index];
    buffer_index+=sizeof(ac_id);


    float value = ((float*)(buffer))[buffer_index];
    buffer_index+=sizeof(value);

    DOWNLINK_SEND_SETTING(pchannel,
                          &_index,
                          &ac_id,
                          &value);


    delete [] buffer;


}

static void on_DL_GET_SETTING(IvyClientPtr app __attribute__ ((unused)),
                              void *user_data __attribute__ ((unused)),
                              int argc __attribute__ ((unused)), char *argv[]){


    int index = 0,nbytes=0;
    QString qstring;
    char* buffer;

    DownlinkDataType typev[]= {
        DL_TYPE_UINT8,
        DL_TYPE_UINT8,
    };

    if(argc!=3){
        echo_msg_arg_error("DL_GET_SETTING");
    }
    /*
    qstring = argv[index++];
    nbytes = qstring.toInt(&ok);
    if(ok==false){
    echo_msg_conversion_error("bytes","int");
    }
    qstring = argv[index++];
    if(qstring!="GET_DL_SETTING"){
    echo_msg_arg_error("DL_GET_SETTING");
    }*/

    nbytes = 1+1;
    buffer = new char[nbytes];
    index=1;
    if(fill_buffer_array(&argv[index],typev,sizeof(typev)/sizeof(DownlinkDataType),buffer,nbytes)==false){
        std::cout<<"some error occured in while constructing the arg buffer"<<std::endl;
    }
    
    DownlinkTransport *pchannel = (DownlinkTransport*)user_data;
    
    int buffer_index=0;
    quint8 _index = ((quint8*)(buffer))[buffer_index];
    buffer_index+=sizeof(_index);

    quint8 ac_id = ((quint8*)(buffer))[buffer_index];
    buffer_index+=sizeof(ac_id);

    DOWNLINK_SEND_GET_SETTING(pchannel,
                              &_index,
                              &ac_id);


    delete [] buffer;

}

static void on_DL_BLOCK(IvyClientPtr app __attribute__ ((unused)),
                        void *user_data,
                        int argc, char *argv[]){
    int index = 0,nbytes=0;
    QString qstring;
    char* buffer;

    DownlinkDataType typev[]= {
        DL_TYPE_UINT8,
        DL_TYPE_UINT8,
    };

    if(argc!=3){
        echo_msg_arg_error("DL_BLOCK");
    }

    nbytes=1+1;
    buffer = new char[nbytes];
    index=1;
    if(fill_buffer_array(&argv[index],typev,sizeof(typev)/sizeof(DownlinkDataType),buffer,nbytes)==false){
        std::cout<<"some error occured in while constructing the arg buffer"<<std::endl;
    }
    
    DownlinkTransport *pchannel = (DownlinkTransport*)user_data;
    
    int buffer_index=0;
    quint8 block_id = ((quint8*)(buffer))[buffer_index];
    buffer_index+=sizeof(block_id);

    quint8 ac_id = ((quint8*)(buffer))[buffer_index];
    buffer_index+=sizeof(ac_id);

    DOWNLINK_SEND_BLOCK(pchannel,
                        &block_id,
                        &ac_id);

    delete [] buffer;


}

static void on_DL_MOVE_WP(IvyClientPtr app __attribute__ ((unused)),
                          void *user_data __attribute__ ((unused)),
                          int argc __attribute__ ((unused)), char *argv[]){

    int index = 0,nbytes=0;
    QString qstring;
    char* buffer;
    DownlinkDataType typev[]= {
        DL_TYPE_UINT8,
        DL_TYPE_UINT8,
        DL_TYPE_INT32,
        DL_TYPE_INT32,
        DL_TYPE_INT32,
    };
    nbytes=1+1+4+4+4;
    buffer = new char[nbytes];
    index=1;


    if(fill_buffer_array(&argv[index],typev,sizeof(typev)/sizeof(DownlinkDataType),buffer,nbytes)==false){
        std::cout<<"some error occured in while constructing the arg buffer"<<std::endl;
    }
    
    DownlinkTransport *pchannel = (DownlinkTransport*)user_data;
    
    int buffer_index=0;
    quint8 wp_id = ((quint8*)(buffer))[buffer_index];
    buffer_index+=sizeof(wp_id);

    quint8 ac_id = ((quint8*)(buffer))[buffer_index];
    buffer_index+=sizeof(ac_id);

    qint32 lat = ((qint32*)(buffer))[buffer_index];
    buffer_index+=sizeof(lat);

    qint32 lon = ((qint32*)(buffer))[buffer_index];
    buffer_index+=sizeof(lon);


    qint32 alt = ((qint32*)(buffer))[buffer_index];
    buffer_index+=sizeof(alt);


    DOWNLINK_SEND_MOVE_WP(pchannel,
                          &wp_id,
                          &ac_id,
                          &lat,
                          &lon,
                          &alt);

    delete [] buffer;

}

static void on_NPS_SEN_NICE_IR(IvyClientPtr app __attribute__ ((unused)),
                               void *user_data ,
                               int argc , char *argv[]){
    //    const char* arg = (argc < 1) ? ":::" : argv[0];
    //    const char* arg2 = (argc < 2) ? "!!!" : argv[1];
    //    const char* arg3 = (argc < 3) ? "???" : argv[2];

    //    cout <<"NPS_SEN_NICE_IR message is ";
    //    int i = 0;
    //    for(i; i<argc;i++){
    //        cout << argv[i]<< " ";
    //    }
    //    cout <<"\n";

    //TODO Send the actual data to the autopilot.
    /*
    DOWNLINK_SEND_HITL_IR_AHRS(&TransportChannel,
                                 &ir_id,
                                 &ac_id,
                                 &roll,
                                 &pitch,
                                 &yaw,
                                 &p,
                                 &q,
                                 &r);
    */
    static long counter = 0;
    //    static qint64 tstamp = QDateTime::currentMSecsSinceEpoch();
    //    if(counter++ %500 == 0){
    //        cout<<"counter " << counter << " " << QDateTime::currentMSecsSinceEpoch()<<endl;
    //        cout<<"average "<<(QDateTime::currentMSecsSinceEpoch()-tstamp)/counter<<""<<endl;
    //    }


    int index = 0,nbytes=0;
    QString qstring;
    char* buffer;

    DownlinkDataType typev[]= {
        DL_TYPE_UINT8,
        DL_TYPE_UINT8,
        DL_TYPE_DOUBLE,
        DL_TYPE_DOUBLE,
        DL_TYPE_DOUBLE,
        DL_TYPE_DOUBLE,
        DL_TYPE_DOUBLE,
        DL_TYPE_DOUBLE
    };

    nbytes=1+1+8+8+8+8+8+8;
    buffer = new char[nbytes];
    index=0;

    if(fill_buffer_array(&argv[index],typev,sizeof(typev)/sizeof(DownlinkDataType),buffer,nbytes)==false){
        std::cout<<"some error occured in while constructing the arg buffer"<<std::endl;
    }

    DownlinkTransport *pchannel = (DownlinkTransport*)user_data;
    QString tmp;
    tmp = argv[0];
    quint8 ir_id = tmp.toUInt();

    tmp = argv[1];
    quint8 ac_id = tmp.toUInt();

    tmp = argv[2];
    double roll = tmp.toDouble();

    tmp = argv[3];
    double pitch = tmp.toDouble();

    tmp = argv[4];
    double yaw = tmp.toDouble();

    tmp = argv[5];
    double p = tmp.toDouble();

    tmp = argv[6];
    double q = tmp.toDouble();

    tmp = argv[7];
    double r = tmp.toDouble();

    /*
    static double id =0.000001;
    roll = 1.234 + id;
    pitch = 2.345 + id;
    yaw = 3.456 + id;
    p = 4.567+ id;
    q = 5.678+ id;
    r = 6.789+ id;
    id += 0.000001;
    */

    DOWNLINK_SEND_HITL_IR_AHRS(pchannel,
                               &ir_id,
                               &ac_id,
                               &roll,
                               &pitch,
                               &yaw,
                               &p,
                               &q,
                               &r);
    delete [] buffer;

    counter++;
    if((counter%200)==0){
        std::cout<<counter<<" IR messages \r\n";
    }

}

static void on_NPS_SEN_NICE_GPS(IvyClientPtr app __attribute__ ((unused)),
                                void *user_data ,
                                int argc , char *argv[]){

    //    cout <<"NPS_SEN_NICE_GPS message is ";
    //    int i = 0;
    //    for(i; i<argc;i++){
    //        cout << argv[i]<< " ";
    //    }
    //    cout <<"\n";

    //TODO Send the actual data to the autopilot.
    /*
     *
        DOWNLINK_SEND_HITL_GPS_COMMON(&TransportChannel,
                                      &gps_id,
                                      &ac_id,
                                      &lat,
                                      &lon,
                                      &alt,
                                      &course,
                                      &gspeed,
                                      &climb,
                                      &time);
     * */
    int index = 0,nbytes=0;
    QString qstring;
    char* buffer;

    DownlinkDataType typev[]= {
        DL_TYPE_UINT8,
        DL_TYPE_UINT8,
        DL_TYPE_DOUBLE,
        DL_TYPE_DOUBLE,
        DL_TYPE_DOUBLE,
        DL_TYPE_DOUBLE,
        DL_TYPE_DOUBLE,
        DL_TYPE_DOUBLE,
        DL_TYPE_DOUBLE
    };

    nbytes=1+1+8+8+8+8+8+8+8;
    buffer = new char[nbytes];
    index=0;


    if(fill_buffer_array(&argv[index],typev,sizeof(typev)/sizeof(DownlinkDataType),buffer,nbytes)==false){
        std::cout<<"some error occured in while constructing the arg buffer"<<std::endl;
    }

    DownlinkTransport *pchannel = (DownlinkTransport*)user_data;

    int buffer_index=0;
    QString tmp;
    tmp = argv[0];
    quint8 gps_id = tmp.toUInt();

    tmp = argv[1];
    quint8 ac_id = tmp.toUInt();

    tmp = argv[2];
    double lat = tmp.toDouble();

    tmp = argv[3];
    double lon = tmp.toDouble();

    tmp = argv[4];
    double alt = tmp.toDouble();

    tmp = argv[5];
    double course = tmp.toDouble();

    tmp = argv[6];
    double gspeed = tmp.toDouble();

    tmp = argv[7];
    double climb = tmp.toDouble();

    tmp = argv[8];
    double time = tmp.toDouble();

    //    cout << lat << "," << lon << "," << alt << ","<< course << ","<< gspeed << ","<< climb << "," <<time<<endl;

    /*
    lat = 12.345;
    lon = 23.456;
    alt = 34.567;
    course = 45.678;
    gspeed = 56.789;
    climb = 67.890;
    time = 78.901;
    */

    DOWNLINK_SEND_HITL_GPS_COMMON(pchannel,
                                  &gps_id,
                                  &ac_id,
                                  &lat,
                                  &lon,
                                  &alt,
                                  &course,
                                  &gspeed,
                                  &climb,
                                  &time);

    delete [] buffer;
}


static void on_NPS_SEND_GPS_INT(IvyClientPtr app __attribute__ ((unused)),
                                void *user_data ,
                                int argc , char *argv[]){

    //    cout <<"NPS_SEN_NICE_GPS message is ";
    //    int i = 0;
    //    for(i; i<argc;i++){
    //        cout << argv[i]<< " ";
    //    }
    //    cout <<"\n";

    int index = 0,nbytes=0;
    QString qstring;
    char* buffer;

    DownlinkDataType typev[]= {
        DL_TYPE_UINT8,
        DL_TYPE_UINT8,
        DL_TYPE_DOUBLE,
        DL_TYPE_DOUBLE,
        DL_TYPE_DOUBLE,
        DL_TYPE_DOUBLE,
        DL_TYPE_DOUBLE,
        DL_TYPE_DOUBLE,
        DL_TYPE_DOUBLE
    };

    nbytes=1+1+8+8+8+8+8+8+8;
    buffer = new char[nbytes];
    index=0;


    if(fill_buffer_array(&argv[index],typev,sizeof(typev)/sizeof(DownlinkDataType),buffer,nbytes)==false){
        std::cout<<"some error occured in while constructing the arg buffer"<<std::endl;
    }

    DownlinkTransport *pchannel = (DownlinkTransport*)user_data;

    int buffer_index=0;
    QString tmp;
    tmp = argv[0];
    quint8 gps_id = tmp.toUInt();

    tmp = argv[1];
    quint8 ac_id = tmp.toUInt();

    tmp = argv[2];
    int32_t ecef_pos_x = tmp.toInt();

    tmp = argv[3];
    int32_t ecef_pos_y = tmp.toInt();

    tmp = argv[4];
    int32_t ecef_pos_z = tmp.toInt();

    tmp = argv[5];
    int32_t lla_pos_lat = tmp.toInt();
    \
    tmp = argv[6];
    int32_t lla_pos_lon = tmp.toInt();

    tmp = argv[7];
    int32_t lla_pos_alt = tmp.toInt();

    tmp = argv[8];
    int32_t hmsl = tmp.toInt();

    tmp = argv[9];
    int32_t ecef_vel_x = tmp.toInt();

    tmp = argv[10];
    int32_t ecef_vel_y = tmp.toInt();

    tmp = argv[11];
    int32_t ecef_vel_z = tmp.toInt();

    tmp = argv[12];
    int32_t pacc = tmp.toInt();

    tmp = argv[13];
    int32_t sacc = tmp.toInt();

    tmp = argv[14];
    int32_t tow = tmp.toInt();

    tmp = argv[15];
    int16_t pdop = tmp.toShort();

    tmp = argv[16];
    uint8_t num_sv = tmp.toUInt();

    tmp = argv[17];
    uint8_t fix = tmp.toUInt();

    /*
    gps_id = 1;
    ac_id = 2;
    ecef_pos_x = 3;
    ecef_pos_y = 4;
    ecef_pos_z = 5;
    lla_pos_lat = 6;
    lla_pos_lon = 7;
    lla_pos_alt = 8;
    hmsl = 9;
    ecef_vel_x =10;
    ecef_vel_y=11;
    ecef_vel_z=12;
    pacc =13;
    sacc =14;
    tow=15;
    pdop=16;
    num_sv=17;
    fix=18;
*/

    DOWNLINK_SEND_GPS_INT(pchannel,
                          &gps_id,
                          &ac_id,
                          &ecef_pos_x,
                          &ecef_pos_y,
                          &ecef_pos_z,
                          &lla_pos_lat,
                          &lla_pos_lon,
                          &lla_pos_alt,
                          &hmsl,
                          &ecef_vel_x,
                          &ecef_vel_y,
                          &ecef_vel_z,
                          &pacc,
                          &sacc,
                          &tow,
                          &pdop,
                          &num_sv,
                          &fix);

    delete [] buffer;
}

static void on_NPS_AHRS_LTP(IvyClientPtr app __attribute__ ((unused)),
                            void *user_data ,
                            int argc , char *argv[]){
    char* buffer;

    DownlinkTransport *pchannel = (DownlinkTransport*)user_data;

    QString tmp;
    tmp = argv[0];
    quint8 ltp_id = tmp.toUInt();

    tmp = argv[1];
    quint8 ac_id = tmp.toUInt();

    tmp = argv[2];
    double ltp_to_body_quat_qi = tmp.toDouble();

    tmp = argv[3];
    double ltp_to_body_quat_qx = tmp.toDouble();

    tmp = argv[4];
    double ltp_to_body_quat_qy = tmp.toDouble();

    tmp = argv[5];
    double ltp_to_body_quat_qz = tmp.toDouble();

    //std::cout << " " << ltp_to_body_quat_qi << " " << ltp_to_body_quat_qx << " " << ltp_to_body_quat_qy << " " << ltp_to_body_quat_qz << std::endl;

   DOWNLINK_SEND_AHRS_LTP(pchannel,&ltp_id,&ac_id,&ltp_to_body_quat_qi,&ltp_to_body_quat_qx,
                           &ltp_to_body_quat_qy,&ltp_to_body_quat_qz);

    delete [] buffer;
}


static void on_NPS_AHRS_ECEF(IvyClientPtr app __attribute__ ((unused)),
                             void *user_data ,
                             int argc , char *argv[]){
    char* buffer;

    DownlinkTransport *pchannel = (DownlinkTransport*)user_data;

    QString tmp;
    tmp = argv[0];
    quint8 ecef_id = tmp.toUInt();

    tmp = argv[1];
    quint8 ac_id = tmp.toUInt();

    tmp = argv[2];
    double body_ecef_rotvel_p = tmp.toDouble();

    tmp = argv[3];
    double body_ecef_rotvel_q = tmp.toDouble();

    tmp = argv[4];
    double body_ecef_rotvel_r = tmp.toDouble();

    //std::cout << " " << body_ecef_rotvel_p << " " << body_ecef_rotvel_q << " " << body_ecef_rotvel_r << std::endl;

    DOWNLINK_SEND_AHRS_ECEF(pchannel,&ecef_id,&ac_id,&body_ecef_rotvel_p,&body_ecef_rotvel_q,
                            &body_ecef_rotvel_r);

    delete [] buffer;
}

static void on_NPS_INS_POS(IvyClientPtr app __attribute__ ((unused)),
                           void *user_data ,
                           int argc , char *argv[]){
    char* buffer;
    DownlinkTransport *pchannel = (DownlinkTransport*)user_data;

    QString tmp;
    tmp = argv[0];
    quint8 ecef_id = tmp.toUInt();

    tmp = argv[1];
    quint8 ac_id = tmp.toUInt();

    tmp = argv[2];
    double body_ecef_rotvel_p = tmp.toDouble();

    tmp = argv[3];
    double body_ecef_rotvel_q = tmp.toDouble();

    tmp = argv[4];
    double body_ecef_rotvel_r = tmp.toDouble();

    DOWNLINK_SEND_INS_POS(pchannel,&ecef_id,&ac_id,&body_ecef_rotvel_p,&body_ecef_rotvel_q,
                          &body_ecef_rotvel_r);

    //std::cout<<"on_NPS_INS_POS: " << body_ecef_rotvel_p << " " << body_ecef_rotvel_q << " " << body_ecef_rotvel_r << "\r\n";

    delete [] buffer;
}

static void on_NPS_INS_ECEF_VEL(IvyClientPtr app __attribute__ ((unused)),
                                void *user_data ,
                                int argc , char *argv[]){
    char* buffer;

    DownlinkTransport *pchannel = (DownlinkTransport*)user_data;

    QString tmp;
    tmp = argv[0];
    quint8 ecef_id = tmp.toUInt();

    tmp = argv[1];
    quint8 ac_id = tmp.toUInt();

    tmp = argv[2];
    double body_ecef_rotvel_p = tmp.toDouble();

    tmp = argv[3];
    double body_ecef_rotvel_q = tmp.toDouble();

    tmp = argv[4];
    double body_ecef_rotvel_r = tmp.toDouble();

    DOWNLINK_SEND_INS_ECEF_VEL(pchannel,&ecef_id,&ac_id,&body_ecef_rotvel_p,&body_ecef_rotvel_q,
                               &body_ecef_rotvel_r);

    //std::cout<<"on_NPS_INS_ECEF_VEL: " << body_ecef_rotvel_p << " " << body_ecef_rotvel_q << " " << body_ecef_rotvel_r << "\r\n";

    delete [] buffer;
}

static void on_NPS_INS_ECEF_ACCEL(IvyClientPtr app __attribute__ ((unused)),
                                  void *user_data ,
                                  int argc , char *argv[]){
    char* buffer;

    DownlinkTransport *pchannel = (DownlinkTransport*)user_data;

    QString tmp;
    tmp = argv[0];
    quint8 ecef_id = tmp.toUInt();

    tmp = argv[1];
    quint8 ac_id = tmp.toUInt();

    tmp = argv[2];
    double body_ecef_rotvel_p = tmp.toDouble();

    tmp = argv[3];
    double body_ecef_rotvel_q = tmp.toDouble();

    tmp = argv[4];
    double body_ecef_rotvel_r = tmp.toDouble();

    DOWNLINK_SEND_INS_ECEF_ACCEL(pchannel,&ecef_id,&ac_id,&body_ecef_rotvel_p,&body_ecef_rotvel_q,
                                 &body_ecef_rotvel_r);

    //std::cout<<"on_NPS_INS_ECEF_ACCEL: " << body_ecef_rotvel_p << " " << body_ecef_rotvel_q << " " << body_ecef_rotvel_r << "\r\n";

    delete [] buffer;
}

static void on_NPS_SENSORS_GYRO(IvyClientPtr app __attribute__ ((unused)),
                                void *user_data ,
                                int argc , char *argv[]){
    char* buffer;

    DownlinkTransport *pchannel = (DownlinkTransport*)user_data;

    QString tmp;
    tmp = argv[0];
    quint8 ecef_id = tmp.toUInt();

    tmp = argv[1];
    quint8 ac_id = tmp.toUInt();

    tmp = argv[2];
    double x = tmp.toDouble();

    tmp = argv[3];
    double y = tmp.toDouble();

    tmp = argv[4];
    double z = tmp.toDouble();

    //std::cout << "NPS_SENSORS_GYRO " << x << " " << y << " " << z << std::endl;

    DOWNLINK_SEND_NPS_SENSORS_GYRO(pchannel,&ecef_id,&ac_id,&x,&y,
                                   &z);

    delete [] buffer;
}

static void on_NPS_SENSORS_ACCEL(IvyClientPtr app __attribute__ ((unused)),
                                 void *user_data ,
                                 int argc , char *argv[]){
    char* buffer;

    DownlinkTransport *pchannel = (DownlinkTransport*)user_data;

    QString tmp;
    tmp = argv[0];
    quint8 ecef_id = tmp.toUInt();

    tmp = argv[1];
    quint8 ac_id = tmp.toUInt();

    tmp = argv[2];
    double x = tmp.toDouble();

    tmp = argv[3];
    double y = tmp.toDouble();

    tmp = argv[4];
    double z = tmp.toDouble();

    //std::cout << "NPS_SENSORS_ACCEL " << x << " " << y << " " << z << std::endl;

    DOWNLINK_SEND_NPS_SENSORS_ACCEL(pchannel,&ecef_id,&ac_id,&x,&y,
                                    &z);

    delete [] buffer;
}

IvyThread::IvyThread(QSynchQueue<ethernetMessage> *pdatalink_queue){
    ethQueue = pdatalink_queue;
}

void IvyThread::ivy_transport_init(void) {
    IvyInit ("Paparazzi jsbsim " + AC_ID, "READY", NULL, NULL, NULL, NULL);
    IvyStart(ivyBus.c_str());
}


void IvyThread::sim_autopilot_init(void){
    DownlinkTransport *ptr =new HitlTransport(this->ethQueue);
    TransportChannelVector.push_back(ptr);
    IvyBindMsg(on_DL_PING, ptr, "^(\\S*) DL_PING");

    ptr =new HitlTransport(this->ethQueue);
    TransportChannelVector.push_back(ptr);
    IvyBindMsg(on_DL_ACINFO, ptr, "^(\\S*) DL_ACINFO (\\S*) (\\S*) (\\S*) (\\S*) (\\S*) (\\S*) (\\S*) (\\S*)");


    ptr =new HitlTransport(this->ethQueue);
    TransportChannelVector.push_back(ptr);
    IvyBindMsg(on_DL_SETTING,ptr, "^(\\S*) DL_SETTING (\\S*) (\\S*) (\\S*)");

    ptr =new HitlTransport(this->ethQueue);
    TransportChannelVector.push_back(ptr);
    IvyBindMsg(on_DL_GET_SETTING,ptr, "^(\\S*) GET_DL_SETTING (\\S*) (\\S*)");

    ptr =new HitlTransport(this->ethQueue);
    TransportChannelVector.push_back(ptr);
    IvyBindMsg(on_DL_BLOCK, ptr, "^(\\S*) BLOCK (\\S*) (\\S*)");

    ptr =new HitlTransport(this->ethQueue);
    TransportChannelVector.push_back(ptr);
    IvyBindMsg(on_DL_MOVE_WP,ptr, "^(\\S*) MOVE_WP (\\S*) (\\S*) (\\S*) (\\S*) (\\S*)");

    //Create the transport channel and send into the callback message
    DownlinkTransport *ePtr = new HitlTransport(this->ethQueue);
    TransportChannelVector.push_back(ePtr);
    IvyBindMsg(on_NPS_SEN_NICE_IR, ePtr, "^NPS_SEN_NICE_IR (\\S*) (\\S*) (\\S*) (\\S*) (\\S*) (\\S*) (\\S*) (\\S*)");

    ePtr = new HitlTransport(this->ethQueue);
    TransportChannelVector.push_back(ePtr);
    IvyBindMsg(on_NPS_SEN_NICE_GPS, ePtr, "^NPS_SEN_NICE_GPS (\\S*) (\\S*) (\\S*) (\\S*) (\\S*) (\\S*) (\\S*) (\\S*) (\\S*)");

    ePtr = new HitlTransport(this->ethQueue);
    TransportChannelVector.push_back(ePtr);
    IvyBindMsg(on_NPS_SEND_GPS_INT, ePtr, "^NPS_SEND_GPS_INT (\\S*) (\\S*) (\\S*) (\\S*) (\\S*) (\\S*) (\\S*) (\\S*) (\\S*) (\\S*) (\\S*) (\\S*) (\\S*) (\\S*) (\\S*) (\\S*) (\\S*) (\\S*)");

#ifdef TRANSMIT_AHRS
    ePtr = new HitlTransport(this->ethQueue);
    TransportChannelVector.push_back(ePtr);
    IvyBindMsg(on_NPS_AHRS_LTP, ePtr, "^NPS_AHRS_LTP (\\S*) (\\S*) (\\S*) (\\S*) (\\S*) (\\S*)");

    ePtr = new HitlTransport(this->ethQueue);
    TransportChannelVector.push_back(ePtr);
    IvyBindMsg(on_NPS_AHRS_ECEF, ePtr, "^NPS_AHRS_ECEF (\\S*) (\\S*) (\\S*) (\\S*) (\\S*)");
#endif

#ifdef TRANSMIT_INS
    ePtr = new HitlTransport(this->ethQueue);
    TransportChannelVector.push_back(ePtr);
    IvyBindMsg(on_NPS_INS_POS, ePtr, "^NPS_INS_POS (\\S*) (\\S*) (\\S*) (\\S*) (\\S*)");

    ePtr = new HitlTransport(this->ethQueue);
    TransportChannelVector.push_back(ePtr);
    IvyBindMsg(on_NPS_INS_ECEF_VEL, ePtr, "^NPS_INS_ECEF_VEL (\\S*) (\\S*) (\\S*) (\\S*) (\\S*)");

    ePtr = new HitlTransport(this->ethQueue);
    TransportChannelVector.push_back(ePtr);
    IvyBindMsg(on_NPS_INS_ECEF_ACCEL, ePtr, "^NPS_INS_ECEF_ACCEL (\\S*) (\\S*) (\\S*) (\\S*) (\\S*)");
#endif

    ePtr = new HitlTransport(this->ethQueue);
    TransportChannelVector.push_back(ePtr);
    IvyBindMsg(on_NPS_SENSORS_GYRO, ePtr, "^NPS_SENSORS_GYRO (\\S*) (\\S*) (\\S*) (\\S*) (\\S*)");

    ePtr = new HitlTransport(this->ethQueue);
    TransportChannelVector.push_back(ePtr);
    IvyBindMsg(on_NPS_SENSORS_ACCEL, ePtr, "^NPS_SENSORS_ACCEL (\\S*) (\\S*) (\\S*) (\\S*) (\\S*)");

}

void IvyThread::stop_processing(){
    IvyStop();
}

void IvyThread::run(){
    ivy_transport_init();
    sim_autopilot_init();
    /* main loop */
    IvyMainLoop();
}
