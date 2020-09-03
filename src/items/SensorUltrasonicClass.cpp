#include "items/ItemsGlobal.h"
#include "items/SensorUltrasonicClass.h"
//#ifdef SensorUltrasonicEnabled
//=========================================Модуль ультрозвукового дальномера==================================================================
//ultrasonic-cm;id;anydata;Сенсоры;Расстояние;order;pin[12,13];map[1,100,1,100];c[1]
//=========================================================================================================================================
SensorUltrasonic mySensorUltrasonic;
void ultrasonicCm() {
    mySensorUltrasonic.update();
    String key = mySensorUltrasonic.gkey();
    String pin = mySensorUltrasonic.gpin();
    sCmd.addCommand(key.c_str(), ultrasonicReading);
    mySensorUltrasonic.init();
    mySensorUltrasonic.clear();
}

void ultrasonicReading() {
    String key = sCmd.order();
    mySensorUltrasonic.SensorUltrasonicRead(key);
}
//#endif