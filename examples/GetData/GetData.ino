//=============================================================================//
//                                                                             //
//  This program reads the accelerometer, gyro, magnetometer and temperature   //
//  data from the sensor. Sensors are initialized with default values.         //
//                                                                             //
//  Library version : 1.4.4                                                    //
//  Author : Vishnu M Aiea (Original author : Asuki Kono)                      //
//  Source : https://github.com/vishnumaiea/MPU9250_asukiaaa                   //
//  Author's Website : www.vishnumaiea.in                                      //
//  Initial release : +05:30 7:37:12 PM, 25-09-2018, Tuesday                   //
//  License : MIT                                                              //
//                                                                             //
//  File last modified : +05:30 2:57:58 PM, 27-09-2018, Thursday               //
//                                                                             //
//=============================================================================//
//includes and defines

#include "MPU9250_VMA.h"

#ifdef _ESP32_HAL_I2C_H_
  #define SDA_PIN 26
  #define SCL_PIN 25
#endif

//=============================================================================//
//global objects and variables

MPU9250 mySensor; //create an object

uint8_t sensorId;
float aX, aY, aZ, aSqrt, gX, gY, gZ, mDirection, mX, mY, mZ, dieTemp;

//=============================================================================//
//executed once

void setup() {
  while(!Serial);
  Serial.begin(115200);
  Serial.println("Serial established.");

  #ifdef _ESP32_HAL_I2C_H_ // For ESP32
    Wire.begin(SDA_PIN, SCL_PIN); // SDA, SCL
  #else
    Wire.begin();
  #endif

  mySensor.setWire(&Wire);
  mySensor.beginAccel();
  mySensor.beginGyro();
  mySensor.beginMag();
  mySensor.beginTemp();

  // You can set your own offset for mag values
  // mySensor.magXOffset = -50;
  // mySensor.magYOffset = -55;
  // mySensor.magZOffset = -10;

  sensorId = mySensor.readId();
}

//=============================================================================//
//inifinite loop

void loop() {
  Serial.println("sensorId: " + String(sensorId));

  mySensor.readAccel(); //start reading the sensor
  aX = mySensor.accelX();
  aY = mySensor.accelY();
  aZ = mySensor.accelZ();
  aSqrt = mySensor.accelSqrt();
  Serial.println("accelX: " + String(aX));
  Serial.println("accelY: " + String(aY));
  Serial.println("accelZ: " + String(aZ));
  Serial.println("accelSqrt: " + String(aSqrt));

  mySensor.readGyro();
  gX = mySensor.gyroX();
  gY = mySensor.gyroY();
  gZ = mySensor.gyroZ();
  Serial.println("gyroX: " + String(gX));
  Serial.println("gyroY: " + String(gY));
  Serial.println("gyroZ: " + String(gZ));

  mySensor.readMag();
  mX = mySensor.magX();
  mY = mySensor.magY();
  mZ = mySensor.magZ();
  mDirection = mySensor.magHorizDirection();
  Serial.println("magX: " + String(mX));
  Serial.println("maxY: " + String(mY));
  Serial.println("magZ: " + String(mZ));
  Serial.println("horizontal direction: " + String(mDirection));

  mySensor.readTemp();
  dieTemp = mySensor.getTemp();
  Serial.println("Temp: " + String(dieTemp));


  Serial.println("at " + String(millis()) + "ms");
  Serial.println(""); // Add an empty line
  delay(100);
}

//=============================================================================//
