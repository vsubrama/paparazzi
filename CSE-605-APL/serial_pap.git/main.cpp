#include <QtCore/QThread>
#include <iostream>
#include "listener.h"
int main(int argc, char *argv[]){
  int baudrate=230400;
  int time = 4*60*60*1000;// 4 hour
  if(argc>1+2){
    std::cout<<"Too many arguments"<<std::endl;
  }
  int index=1;
  std::cout <<"Number of arguments are:"<<argc<<std::endl;
  while(index<argc){
    if(argv[index][0]!='-' || argv[index][2]!='=' ){
      std::cout << "Invalid characters in:"<< argv[index]<<std::endl;
      return 0;
    }
    std::cout << argv[index]<<std::endl;
    QString argstring(argv[index]+3); 
    bool bresult = true;
    if(argv[index][1]=='b'){
      baudrate = argstring.toInt(&bresult);
    }else if(argv[index][1]=='t'){
      time = argstring.toInt(&bresult)*1000*60;
      std::cout <<"time out period is :"<< time/1000/60<< " min"<< std::endl;
    }else{
      std::cout << "Invalid command in::"<<argv[index]<<std::endl;
      return 0;
    }
    if(bresult==false){
      std::cout << "non-numeric value in::"<< argstring.toStdString()<<std::endl;
      return 0;
    }

    index++;
  }
  ListenApp a(argc, argv);
  if(a.setup(baudrate,time)==false){
    return 0;
  }
    return a.exec();
}
