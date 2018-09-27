//=============================================================================//
//                                                                             //
//                 ---- MPU9250 IMU Library for Arduino ----                   //
//                                                                             //
//  Originally developed by "Asuki Kono" https://github.com/asukiaaa, later    //
//  forked myself, this library lets you read the sensor data from MPU6050     //
//  and AK8963.                                                                //
//                                                                             //
//  Filename : MPU9250_VMA.cpp                                                 //
//  Description : main file of the library.                                    //
//  Library version : 1.4.4                                                    //
//  Author : Vishnu M Aiea (Original author : Asuki Kono)                      //
//  Source : https://github.com/vishnumaiea/MPU9250_asukiaaa                   //
//  Author's Website : www.vishnumaiea.in                                      //
//  Initial release : +05:30 7:37:12 PM, 25-09-2018, Tuesday                   //
//  License : MIT                                                              //
//                                                                             //
//  File last modified : +05:30 2:57:47 PM, 27-09-2018, Thursday               //
//                                                                             //
//=============================================================================//

#include "MPU9250_VMA.h"
#include <math.h>

//=============================================================================//
//reads data from slave to a data buffer

void MPU9250::i2cRead(uint8_t slaveAddress, uint8_t regAddress, uint8_t byteCount, uint8_t* readBuffer) {
  myWire->beginTransmission(slaveAddress); //send slave address
  myWire->write(regAddress); //send required register starting address
  myWire->endTransmission(); //end I2C write operation

  myWire->requestFrom(slaveAddress, byteCount); //start reading bytes

  uint8_t index = 0;

  while (myWire->available()) {
    readBuffer[index++] = myWire->read(); //save to buffer
  }
}

//=============================================================================//
//writes a single byte to the slave

void MPU9250::i2cWriteByte(uint8_t slaveAddress, uint8_t regAddress, uint8_t incomingData) {
  myWire->beginTransmission(slaveAddress);
  myWire->write(regAddress);
  myWire->write(incomingData);
  myWire->endTransmission();
}

//=============================================================================//
//lets you use cutom wire object

void MPU9250::setWire(TwoWire* wire) {
  myWire = wire;
}

//=============================================================================//
//reads the sensor IDs and info bytes

uint8_t MPU9250::readId(uint8_t slaveAddress) {
  uint8_t id;
  if(slaveAddress == MPU9250_SLAVE_ADDRESS_LOW) {
    i2cRead(slaveAddress, REG_MPU9250_WHOAMI, 1, &id); //returns 0x71 or 113
  }
  else if(slaveAddress == AK8963_SLAVE_ADDRESS) {
    i2cRead(slaveAddress, REG_AK8963_WHOAMI, 1, &id); //returns 0x48 or 72
  }
  else if(slaveAddress == (AK8963_SLAVE_ADDRESS + 1)) {
    i2cRead(slaveAddress, (REG_AK8963_WHOAMI + 1), 1, &id); //returns device infor byte of AK8963
  }
  return id;
}

//=============================================================================//
//reads the factory set adjustment values from the magnetometer.
//use the equation givn in the datasheet to calculate

void MPU9250::magReadAdjustValues() {
  magSetMode(VAL_MAG_MODE_POWERDOWN); //first transit to power down mode in case if it is not already
  magSetMode(VAL_MAG_MODE_FUSEROM); //adjustment data can only be read in fuse mode
  uint8_t dataBuffer[3];
  i2cRead(AK8963_SLAVE_ADDRESS, REG_AK8963_ASAX, 3, dataBuffer); //read from the factory written ROM
  magXAdjust = dataBuffer[0];
  magYAdjust = dataBuffer[1];
  magZAdjust = dataBuffer[2];
}

//=============================================================================//
//initializes the magnetometer with specific mode

void MPU9250::beginMag(uint8_t operationMode, uint8_t outputLength) {
  i2cWriteByte(address, 0x37, 0x02); //IDK why you need to do this when you can access the AK8963 directly
  delay(10);

  magReadAdjustValues();
  magSetMode(VAL_MAG_MODE_POWERDOWN); //first transit to power down mode in case if it is not already
  magSetMode(operationMode, outputLength); //enter specific mode
  delay(10);
}

//=============================================================================//
//sets the operation mode of the AK8963 magnetometer

void MPU9250::magSetMode(uint8_t operationMode) {
  i2cWriteByte(AK8963_SLAVE_ADDRESS, REG_AK8963_CNTL1, operationMode); //set the mode only and reset the output length to 14-bit
  delay(10);
}

//-----------------------------------------------------------------------------//

void MPU9250::magSetMode(uint8_t operationMode, uint8_t outputLength) {
  if(outputLength == 14) {
    i2cWriteByte(AK8963_SLAVE_ADDRESS, REG_AK8963_CNTL1, (operationMode & VAL_MAG_14BIT_OUT_A));
  }
  else {
    i2cWriteByte(AK8963_SLAVE_ADDRESS, REG_AK8963_CNTL1, (operationMode | VAL_MAG_16BIT_OUT_O));
  }

  delay(10);
}

//=============================================================================//
//read the magnetometer sesnor data

void MPU9250::readMag() {
  i2cRead(AK8963_SLAVE_ADDRESS, REG_AK8963_HXL, 7, magBuffer); //why 7 because we need to read the ST2 register inorder initiate another data acquisition cycle
}

//=============================================================================//
//formats the 8-bit values in the buffer to proper 16 bit ones

int16_t MPU9250::getMag(uint8_t highIndex, uint8_t lowIndex) {
  return (((int16_t) magBuffer[highIndex]) << 8) | magBuffer[lowIndex];
}

//=============================================================================//
//calculates the actual values as per given in the datasheet
//refer to page 53 of the Register Description file

float adjustMagValue(int16_t value, uint8_t adjust) {
  return ((float) value * (((((float) adjust - 128) * 0.5) / 128) + 1));
}

//=============================================================================//
//returns the final magnetometer X value

float MPU9250::magX() {
  return adjustMagValue(getMag(1, 0), magXAdjust) + magXOffset; //you can add your custom offsets
}

//=============================================================================//
//returns the final magnetometer Y value

float MPU9250::magY() {
  return adjustMagValue(getMag(3, 2), magYAdjust) + magYOffset;
}

//=============================================================================//
//returns the final magnetometer Z value

float MPU9250::magZ() {
  return adjustMagValue(getMag(5, 4), magZAdjust) + magZOffset;
}

//=============================================================================//
//calulates the horizontal direction in degrees

const float Pi = 3.14159;

float MPU9250::magHorizDirection() {
  return atan2(magX(), magY()) * 180 / Pi;
}

//=============================================================================//
//sets the full scale range of the accelerometer

void MPU9250::beginAccel(uint8_t mode) {
  switch(mode) {
    case VAL_ACCEL_FULL_SCALE_2_G:
      accelRange = 2.0; //reset value
      break;
    case VAL_ACCEL_FULL_SCALE_4_G:
      accelRange = 4.0;
      break;
    case VAL_ACCEL_FULL_SCALE_8_G:
      accelRange = 8.0;
      break;
    case VAL_ACCEL_FULL_SCALE_16_G:
      accelRange = 16.0;
      break;
    default:
      return; // Return without writing invalid mode
  }

  i2cWriteByte(address, REG_ACCEL_CONFIG, mode); //write to register
  delay(10);
}

//=============================================================================//
//reads the accelerometer values to the buffer

void MPU9250::readAccel() {
  i2cRead(address, REG_ACCEL_XOUT_H, 6, accelBuffer); //read the accelerometer values
}

//=============================================================================//
//formats the 8-bit values in the buffer to proper 16-bit values

float MPU9250::getAccel(uint8_t highIndex, uint8_t lowIndex) {
  int16_t v = - ((accelBuffer[highIndex] << 8) | accelBuffer[lowIndex]);
  return ((float) v) * accelRange / (float) 0x8000; // (float) 0x8000 == 32768.0
}

//=============================================================================//
//returns the final X axis value

float MPU9250::accelX() {
  return getAccel(0, 1);
}

//=============================================================================//
//returns the final Y axis value

float MPU9250::accelY() {
  return getAccel(2, 3);
}

//=============================================================================//
//returns the final Z axis value

float MPU9250::accelZ() {
  return getAccel(4, 5);
}

//=============================================================================//

float MPU9250::accelSqrt() {
  return sqrt(pow(getAccel(0, 1), 2) +
              pow(getAccel(2, 3), 2) +
              pow(getAccel(4, 5), 2));
}

//=============================================================================//
//initializes the gyro by setting the full scale range

void MPU9250::beginGyro(uint8_t mode) {
  switch (mode) {
    case VAL_GYRO_FULL_SCALE_250_DPS:
      gyroRange = 250.0;
      break;
    case VAL_GYRO_FULL_SCALE_500_DPS:
      gyroRange = 500.0;
      break;
    case VAL_GYRO_FULL_SCALE_1000_DPS:
      gyroRange = 1000.0;
      break;
    case VAL_GYRO_FULL_SCALE_2000_DPS:
      gyroRange = 2000.0;
      break;
    default:
      return; // Return without writing invalid mode
  }

  i2cWriteByte(address, REG_GYRO_CONFIG, mode);
  delay(10);
}

//=============================================================================//
//reads the gyro values to the buffer

void MPU9250::readGyro() {
  i2cRead(address, REG_GYRO_XOUT_H, 6, gyroBuffer);
}

//=============================================================================//
//formats the 8-bit data into proper 16-bit values and returns the final gyro value

float MPU9250::getGyro(uint8_t highIndex, uint8_t lowIndex) {
  int16_t v = - ((gyroBuffer[highIndex] << 8) | gyroBuffer[lowIndex]);
  return ((float) v) * gyroRange / (float) 0x8000;
}

//=============================================================================//
//returns the final values for X axis

float MPU9250::gyroX() {
  return getGyro(0, 1);
}

//=============================================================================//
//returns the final value for Y axis

float MPU9250::gyroY() {
  return getGyro(2, 3);
}

//=============================================================================//
//returns the final vlaue of Z axis

float MPU9250::gyroZ() {
  return getGyro(4, 5);
}

//=============================================================================//
//initializes the offset and sensitivity values for temperature sensor

void MPU9250::beginTemp(float o = 0.0, float s = 1.0) {
  roomTempOffset = o;
  tempSensitivity = s;
}

//=============================================================================//
//reads the temperature data from the sensor

float MPU9250::readTemp() {
  i2cRead(address, REG_TEMP_OUT_H, 2, tempBuffer);
}

//=============================================================================//
//returns temperature in degrees celsius

float MPU9250::getTemp() {
  int16_t t = ((tempBuffer[0] << 8) | tempBuffer[1]);
  return ((((float) t) - roomTempOffset) / tempSensitivity) + 21.0; //default offset and sensitivity are given in the datasheet
}

//=============================================================================//
