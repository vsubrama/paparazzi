/**
 * @file main.cpp
 * @brief Main file.
 * @author Micha? Policht
 */
#include <QtCore/QList>
#include <QtCore/QDebug>
#include <QtExtSerialPort/qextserialenumerator.h>
#include <QtExtSerialPort/qextserialport.h>
int enumerateserialports(){
    QList<QextPortInfo> ports = QextSerialEnumerator::getPorts();
    qDebug() << "List of ports:";
    foreach (QextPortInfo info, ports) {
      qDebug() << "port name:"       << info.portName;
      qDebug() << "friendly name:"   << info.friendName;
      qDebug() << "physical name:"   << info.physName;
      qDebug() << "enumerator name:" << info.enumName;
      qDebug() << "vendor ID:"       << info.vendorID;
      qDebug() << "product ID:"      << info.productID;
      
      qDebug() << "===================================";
    }
    return 0;
}

void setserialconf(PortSettings& portSettings){
  portSettings.BaudRate=BAUD38400;
  portSettings.DataBits=DATA_8;
  portSettings.Parity=PAR_NONE;
  portSettings.StopBits=STOP_1;
  portSettings.FlowControl=FLOW_OFF;
  portSettings.Timeout_Millisec=2000;
}
