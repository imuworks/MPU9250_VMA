//=============================================================================//
//                                                                             //
//                 ---- MPU9250 IMU Library for Arduino ----                   //
//                                                                             //
//  Originally developed by "Asuki Kono" https://github.com/asukiaaa, later    //
//  forked myself, this library lets you read the sensor data from MPU6050     //
//  and AK8963.                                                                //
//                                                                             //
//  Filename : MPU9250_VMA.h                                                   //
//  Description : header file of the library.                                  //
//  Library version : 1.4.6                                                    //
//  Author : Vishnu M Aiea (Original author : Asuki Kono)                      //
//  Source : https://github.com/vishnumaiea/MPU9250_asukiaaa                   //
//  Author's Website : www.vishnumaiea.in                                      //
//  Initial release : +05:30 7:37:12 PM, 25-09-2018, Tuesday                   //
//  License : MIT                                                              //
//                                                                             //
//  File last modified : +05:30 6:39:23 PM, 27-09-2018, Thursday               //
//                                                                             //
//=============================================================================//

#ifndef MPU9250_VMA_H
#define MPU9250_VMA_H

#include <Arduino.h>
#include <Wire.h>


//"REG" in defines mean "Register Address", rest are values or slave addresses
//"VAL" means value.

//MPU6050 register addresses
#define MPU9250_SLAVE_ADDRESS_LOW        0x68 //address depends on the AD0 pin state
#define MPU9250_SLAVE_ADDRESS_HIGH       0x69
#define REG_MPU9250_WHOAMI               0x75  //device signature register

#define REG_ACCEL_CONFIG                 0x1C  //accelerometer config register address
#define REG_ACCEL_XOUT_H                 0x3B
#define REG_ACCEL_YOUT_H                 0x3D
#define REG_ACCEL_ZOUT_H                 0x40

#define REG_GYRO_CONFIG                  0x1B  //gyro configuration register
#define REG_GYRO_XOUT_H                  0x43
#define REG_GYRO_YOUT_H                  0x45
#define REG_GYRO_ZOUT_H                  0x47

#define REG_TEMP_OUT_H                   0x41
#define VAL_TEMP_ROOM_OFFSET             0.0
#define VAL_TEMP_SENSITIVITY             333.7

//MPU6050 register values
#define VAL_ACCEL_FULL_SCALE_2_G         0x00
#define VAL_ACCEL_FULL_SCALE_4_G         0x08
#define VAL_ACCEL_FULL_SCALE_8_G         0x10
#define VAL_ACCEL_FULL_SCALE_16_G        0x18

#define VAL_GYRO_FULL_SCALE_250_DPS      0x00
#define VAL_GYRO_FULL_SCALE_500_DPS      0x08
#define VAL_GYRO_FULL_SCALE_1000_DPS     0x10
#define VAL_GYRO_FULL_SCALE_2000_DPS     0x18

//magnetometer defines
#define AK8963_SLAVE_ADDRESS             0x0C  //slave addresses
#define REG_AK8963_WHOAMI                0x00
#define REG_MAG_HXL                      0x03  //X-axis data
#define REG_MAG_HYL                      0x05  //Y-axis data
#define REG_MAG_HZL                      0x07  //Y-axis data
#define REG_MAG_CNTL1                    0x0A  //control register
#define REG_MAG_CNTL2                    0x0B  //control register
#define REG_MAG_ASAX                     0x10  //X-axis sensitivity adjustment

//magenetometer register vlaues
#define VAL_MAG_MODE_POWERDOWN           0x0
#define VAL_MAG_MODE_SINGLE              0x1
#define VAL_MAG_MODE_CONTINUOUS_8HZ      0x2
#define VAL_MAG_MODE_EXTERNAL            0x4
#define VAL_MAG_MODE_CONTINUOUS_100HZ    0x6
#define VAL_MAG_MODE_SELFTEST            0x8
#define VAL_MAG_MODE_FUSEROM             0xF

//following values need to combined with other values
#define VAL_MAG_16BIT_OUT_O              0x10  //OR operate this with CNTL1 register
#define VAL_MAG_14BIT_OUT_A              0xEF  //AND operate this with CNTL1 register

//=============================================================================//

class MPU9250 {
  public:
    int16_t magXOffset, magYOffset, magZOffset;
    float roomTempOffset, tempSensitivity; //default offset and sensitivity are given in the datasheet

    MPU9250(uint8_t address = MPU9250_SLAVE_ADDRESS_LOW):
      address(address),
      accelRange(0),
      gyroRange(0),
      magXOffset(0),
      magYOffset(0),
      magZOffset(0) {}; //initialize all to 0s

    void setWire(TwoWire *wire); //lets you custom wire object
    uint8_t readId(uint8_t slaveAddress = MPU9250_SLAVE_ADDRESS_LOW); //returns device signature

    void beginAccel(uint8_t mode = VAL_ACCEL_FULL_SCALE_16_G); //set the full scale range of the accel
    void readAccel(); //reads the accelerometer data
    float accelX(); //returns the final raw X axis value
    float accelY(); //returns the final raw Y axis value
    float accelZ(); //returns the final raw Z axis value
    float accelSqrt();

    void beginGyro(uint8_t mode = VAL_GYRO_FULL_SCALE_2000_DPS); //set the full scale range of gyro
    void readGyro(); //reads the gyro sensor data
    float gyroX(); //retuns the final raw X axis value
    float gyroY(); //returns the final raw Y axis value
    float gyroZ(); //returns the final raw Z axis value

    void beginMag(uint8_t operationMode = VAL_MAG_MODE_CONTINUOUS_8HZ, uint8_t outputLength = 16); //initializes the AK8963 in continuous mode
    void magSetMode(uint8_t operationMode); //set the operation mode only and reset output length to 14-bit
    void magSetMode(uint8_t operationMode, uint8_t outputLength); //set the operation mode and output length
    void readMag(); //reads the magnetometer data
    float magX(); //returns the final raw X axis value
    float magY(); //returns the final raw Y axis value
    float magZ(); //returns the final raw Z axis value
    float magHorizDirection(); //converts the magnetometer values to horizontal rotation

    void beginTemp(float offset = VAL_TEMP_ROOM_OFFSET, float sensitivity = VAL_TEMP_SENSITIVITY); //default offset and sensitivity as per datasheet
    void readTemp(); //reads the temperature registers of sensor
    float getTemp(); //returns formatted temperature in Celsius

  private:
    TwoWire* myWire; //pointer to custom Wire object
    uint8_t address; //I2C slave address
    uint8_t tempBuffer[2]; //buffer for temperature values
    uint8_t accelBuffer[6]; //buffer for accel data
    float accelRange; //full scale range of accelerometer
    uint8_t gyroBuffer[6]; //buffer for gyro data
    float gyroRange; //full range scale of gyro
    uint8_t magBuffer[7]; //buffer for magnetometer data
    uint8_t magXAdjust, magYAdjust, magZAdjust; //factory set adjustment values
    float getAccel(uint8_t highIndex, uint8_t lowIndex); //formats the 8-bit data into proper 16-bit values and returns the final value
    float getGyro(uint8_t highIndex, uint8_t lowIndex); //same as above
    int16_t getMag(uint8_t highIndex, uint8_t lowIndex); //same as above
    void magReadAdjustValues(); //read the factory set adjustment values from the magnetometer
    void i2cRead(uint8_t slaveAddress, uint8_t Register, uint8_t Nbytes, uint8_t* Data); //reads n bytes from the salve to an 8-bit array buffer
    void i2cWriteByte(uint8_t slaveAddress, uint8_t Register, uint8_t Data); //writes a single byte to the slave
};

//=============================================================================//

#endif

//=============================================================================//
