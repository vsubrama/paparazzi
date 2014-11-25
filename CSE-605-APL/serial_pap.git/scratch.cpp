
static bool fill_buffer_array(const char* const* argv,DownlinkDataType typev[],int argc,char* buffer,int bytes){
  quint32 buffer_index = 0;
  quint32 argv_index = 0;
  quint8 ob_size =0;
  qint8 sign;
  bool ok=false;
  QString argstring;
`  quint64 value;
  for(;argv_index<argc;argv_index++){
    argstring = argv[argv_index];
    if(buffer_index > bytes ){
      std::cout << "buffer overflowed"<<std::endl;
    }
    switch(typev[argv_index]){
    case DL_TYPE_DOUBLE:
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
      *p = argstring.toUInt(&k);
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
      /*because these values are already unsigned we
	need not be bother about the sign of the data*/
      *p=(qint16)(mod_value(argstring,&sign,&ok));
      *p *=sign;
      if(ok==false) echo_data_type_error("string","DL_TYPE_INT16");
    }
      break;
    case DL_TYPE_INT32:{
      ob_size = sizeof(qint32);
      qint32* p = (qint32*)(&buffer[buffer_index]);
      /*because these values are already unsigned we
	need not be bother about the sign of the data*/
      *p=(qint32)(mod_value(argstring,&sign,&ok));
      *p *=sign;
      if(ok==false) echo_data_type_error("string","DL_TYPE_INT32");
    }
      break;
    case DL_TYPE_INT64:{
      ob_size = sizeof(qint64);
      qint64* p = (qint64*)(&buffer[buffer_index]);
      /*because these values are already unsigned we
	need not be bother about the sign of the data*/
      *p=(qint64)(mod_value(argstring,&sign,&ok));
      *p *=sign;
      if(ok==false) echo_data_type_error("string","DL_TYPE_INT8");
    }
      break;
    case DL_TYPE_FLOAT:{
      /*Though we do have qreal type for coverting a float or 
       double value.But I am not sure aout the size of qreal. Therefore
       to be safe I am convert this value to raw float.*/
      ob_size = sizeof(float);
      float* p = (float)(&buffer[buffer_index]);
      /*because these values are already unsigned we
	need not be bother about the sign of the data*/
      *p = argstring.toFloat(&ok);
      if(ok==false) echo_data_type_error("string","DL_TYPE_INT8");
    }
      break;
    default:
      echo_switch_nomatch_error("fill_buffer_array");
    }
    if(!ok) return false;
    buffer_index+=object_size;
  }
    return true;
}
