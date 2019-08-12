#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <LoRa.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <SparkFunLSM9DS1.h>
#include <SparkFun_MAG3110.h>
#include <SparkFun_RFD77402_Arduino_Library.h>
#include <SparkFun_SCD30_Arduino_Library.h>

Adafruit_BME280 bme;
MAG3110 mag = MAG3110();
LSM9DS1 imu;
RFD77402 dist;
SCD30 airSensor;

byte address=0;
byte addresses[128];
int nDevices=0;
int period=10000;
String blob="{ ";

void setup() {
  Serial.begin(9600);
  LoRa.begin(915E6);
  nDevices = 0;
  Wire.begin();
  for(address = 1; address < 127; address++ ){
    Wire.beginTransmission(address);
    if(Wire.endTransmission()==0){
      addresses[nDevices]=address;
      nDevices++;
    }
  }
  mag.initialize(); //Initializes the mag sensor
  mag.start();      //Puts the sensor in active mode
  bme.begin();
  imu.begin();
  dist.begin();
  airSensor.begin();
}

void loop() {
  for(int i=0; i<nDevices+1; i++){
    switch(addresses[i]){
      case(14): //0x0E
      //MAG3110
      int Bx, By, Bz;
      mag.readMag(&Bx, &By, &Bz);
      blob+="\"MAG3110_BX\": ";
      blob+=Bx;  //BX
      blob+="\"MAG3110_BY\": ";
      blob+=By;  //BY
      blob+="\"MAG3110_BZ\": ";
      blob+=Bz;  //BZ
      blob+=" ";
      break;
      case(30): //0x1E
      //LSM9DS1 Magnetometer
      blob+="\"LSMDS1_BX\": ";
      blob+=imu.mx; //BX
      blob+=" \"LSM9DS1_BY\": ";
      blob+=imu.my; //BY
      blob+=" \"LSM9DS1_BZ\": ";
      blob+=imu.mz; //BZ
      blob+=" ";
      break;
      case(76): //0x4C
      //RFD77402
      dist.takeMeasurement();
      blob+="\"RFD88402_L\": ";
      blob+=dist.getDistance(); //L
      blob+=" ";
      break;
      case(97): //0x61
      //SCD30
      blob+="\"SCD30_CO2\": ";
      blob+=airSensor.getCO2(); //CO2
      blob+=" \"SCD30_T\": ";
      blob+=airSensor.getTemperature(); //T
      blob+=" \"SCD30_RH\": ";
      blob+=airSensor.getHumidity();  //RH
      blob+=" ";
      break;
      case(107):  //0x6B
      //LSM9DS1 Accelerometer and Gyroscope
      blob+="\"LSM9DS1_AX\": ";
      blob+=imu.ax; //AX
      blob+=" \"LSM9DS1_AY\": ";
      blob+=imu.ay;  //AY
      blob+=" \"LSM9DS1_AZ\": ";
      blob+=imu.az; //AZ
      blob+=" \"LSM9DS1_GX\": ";
      blob+=imu.gx;  //GX
      blob+=" \"LSM9DS1_GY\": ";
      blob+=imu.gy; //GY
      blob+=" \"LSM9DS1_GZ\": ";
      blob+=imu.gz; //GZ
      blob+=" ";
      break;
      case(118):  //0x76
      //BME280
      blob+="\"BME280_T\": ";
      blob+=bme.readTemperature();  //T
      blob+=" \"BME280_RH\": ";
      blob+=bme.readHumidity(); //RH
      blob+=" \"BME280_P\": ";
      blob+=bme.readPressure(); //P
      blob+=" ";
      break;
    }
  }
  blob+=" }";
  Serial.println(blob);
  LoRa.beginPacket();
  LoRa.print(blob);
  LoRa.endPacket();
  delay(period);
  blob="{ ";
}
