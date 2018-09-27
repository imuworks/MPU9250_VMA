# MPU9250_VMA
Originally developed by "Asuki Kono" https://github.com/asukiaaa, later forked myself, this library lets you read the sensor data from MPU6050 and AK8963.

MPU9250 is a 9-axis or 9 Degrees of Freedom (9 DOF) Inertial Measurement Unit IC from InventSense which incorporates 3-axis accelerometer, 3-axis gyro and a 3-axis magnetometer. The accelerometer and gyro are the same as the MPU6050 (6 DOF) sensor while the MPU9250 embeds an additional 3-axis magnetometer AK8963 from Asahi Kasei Microdevices (AKM) in a single package. AK8963 can be bought as a single IC.

# Installation
You can install to Arduino IDE with using library manager.

1. Select [Sketch -> Include Library -> Manage Libraries] to open library manager.
2. Search MPU9250 in library manager.
3. Select and install this project.

# Connection
For boards such as Arduino Uno, Nano etc,

| Arduino | MPU9250 |
|---------|---------|
| 3.3     | VCC     |
| A4(SDA) | SDA     |
| A5(SCL) | SCL     |
| GND     | GND     |

For other boards, please check [i2c pin assign](https://www.arduino.cc/en/Reference/Wire).

# Usage
Demonstration of basic functions can be found in the example [example project](https://github.com/asukiaaa/MPU9250_asukiaaa/blob/master/examples/GetData/GetData.ino).

## Concept
The concept of this library can be described in four steps.

1. The sensor has a set of registers with configuration and output data. We first configure the registers and initialize them.
2. Read the sensor data to a buffer array periodically. There are separate buffers such as accelBuffer, gyroBuffer, magBuffer and tempBuffer. The size of these arrays are determined by the number of registers the actual data resides.
3. Once reading is complete, we need to convert these 8-bit data to proper 16-bit values.
4. The 16-bit values in raw format are converted to sensible values after adjustments and compensations.

## Accelerometer
MPU6050 has a 3-axis accelerometer with 16-bit output with selectable ranges up to ±16g.

Example program:
```c
#include <MPU9250_VMA.h>

MPU9250 mySensor;
float aX, aY, aZ, aSqrt;

void setup() {
  Wire.begin();
  mySensor.setWire(&Wire);
  mySensor.beginAccel();
}

void loop() {
  mySensor.readAccel(); //reads the sensor data
  aX = mySensor.accelX(); //returns 16-bit axis specific value
  aY = mySensor.accelY();
  aZ = mySensor.accelZ();
  aSqrt = mySensor.accelSqrt();

  // Do what you want
}
```
### Functions Available

```c
1. void beginAccel(uint8_t mode);
```
Initializes the accelerometer with full scale range. The full scale range can be one of the following,

```c
VAL_ACCEL_FULL_SCALE_2_G
VAL_ACCEL_FULL_SCALE_4_G
VAL_ACCEL_FULL_SCALE_8_G
VAL_ACCEL_FULL_SCALE_16_G
```

```c
2. void readAccel();
```
Reads the accelerometer registers and store the values to a 6 byte array.

```c
3. float accelX();
4. float accelY();
5. float accelZ();
```
Returns the formatted values of acceleration of each axis.

```c
6. float accelSqrt();
```
This returns the average acceleration or net acceleration of all three axes combined. Individual axis values can't be extracted from this. It gives you an idea of overall acceleration a body is undergoing.

```c
7. float getAccel(uint8_t highIndex, uint8_t lowIndex);
```
Converts the 8-bit values in the buffer to proper 16-bit values.

## Gyrometer
MPU6050 has a 3-axis gyroscope with 16-bit output code with selectable ranges up to ±2000 °/S.

Example program:
```c
#include <MPU9250_VMA.h>

MPU9250 mySensor;
float gX, gY, gZ;

void setup() {
  Wire.begin();
  mySensor.setWire(&Wire);
  mySensor.beginGyro();
}

void loop() {
  mySensor.readGyro(); //reads the sensor data
  gX = mySensor.gyroX(); //returns 16-bit axis specific value
  gY = mySensor.gyroY();
  gZ = mySensor.gyroZ();

  // Do what you want
}
```

### Functions Available
```c
1. void beginGyro(uint8_t mode);
```
Initializes the gyro with full scale range required. The full scale range can be one of the following,

```c
VAL_GYRO_FULL_SCALE_250_DPS
VAL_GYRO_FULL_SCALE_500_DPS
VAL_GYRO_FULL_SCALE_1000_DPS
VAL_GYRO_FULL_SCALE_2000_DPS
```

```c
2. void readGyro();
```
Reads the gyro registers and store the values to a 6 byte array.

```c
3. float gyroX();
4. float gyroY();
5. float gyroZ();
```
Returns the formatted values of rotations of each axis.

```c
6. float getGyro(uint8_t highIndex, uint8_t lowIndex);
```
Converts the 8-bit values in the buffer to proper 16-bit values.

## Magnetometer
AK8963 is a 3-axis magnetometer or compass with 14-bit (0.6 μT/LSB typ) or 16-bit 90.15μT/LSB typ) selectable output and works based on hall effect. Default operation mode is continuous measurement at 8Hz and output length is 16-bit.

Example program:
```c
#include <MPU9250_VMA.h>

MPU9250 mySensor; //create an object
float mDirection; //horizontal direction determining direction
uint16_t mX, mY, mZ;

void setup() {
  Wire.begin();
  mySensor.setWire(&Wire);
  mySensor.beginMag();
}

void loop() {
  mySensor.readMag(); //reads the sensor data
  mX = mySensor.magX(); //returns 16-bit axis specific value
  mY = mySensor.magY();
  mZ = mySensor.magZ();
  mDirection = mySensor.magHorizDirection();

  // Do what you want
}
```

### Functions Available

```c
1. void beginMag(uint8_t operationMode = VAL_MAG_MODE_CONTINUOUS_8HZ, uint8_t outputLength = 16);
```
Initializes the magnetometer with operating mode and output word length. The output modes can be,

```c
VAL_MAG_MODE_POWERDOWN
VAL_MAG_MODE_SINGLE
VAL_MAG_MODE_CONTINUOUS_8HZ
VAL_MAG_MODE_EXTERNAL
VAL_MAG_MODE_CONTINUOUS_100HZ
VAL_MAG_MODE_SELFTEST
VAL_MAG_MODE_FUSEROM
```

More information on these modes can be found in the datasheet of AK8963. The output length can be 16 or 14. Defaults are continuous mode and 16-bit if you don't pass any arguments.

```c
2. void magSetMode(uint8_t operationMode);
3. void magSetMode(uint8_t operationMode, outputLength);
```
These writes to the control registers of the sensor.

```c
4. void readMag();
```
Reads the magnetometer registers and store the values to a 7 byte array. Even though the data consists of only 6 bytes, an additional register needs to be read in order to properly conclude the read operation so as to initiate further measurement cycles.

```c
5. float magX();
6. float magY();
7. float magZ();
```
Returns the formatted values of magnetic strengths of each axis.

```c
8. float magHorizDirection();
```
This returns the horizontal direction in degrees that you can use as a compass.

```c
7. float getAccel(uint8_t highIndex, uint8_t lowIndex);
```
Converts the 8-bit values in the buffer to proper 16-bit values.

If the output from the magnetometer are like these,

Name | Max | Min
-----|----:|----:
magX |  70 | -30
maxY | 110 |  10

You can calculate the offset values using following method.

```
maxXOffset = - (magXMax + magXMin) / 2
           = - (70 - 30) / 2
           = - 40 / 2
           = -20
```

```
magYOffset = - (magYMax + magYMin) / 2
           = - (110 + 10) / 2
           = - 120 / 2
           = -60
```

Then set the offset values like this,

```c
void setup() {
  mySensor.magXOffset = -20;
  mySensor.magYOffset = -60;
}
```

Then the results would look like,

Name | Max | Min
-----|----:|----:
magX |  50 | -50
maxY |  50 | -50

After setting offset values, the `magHorizDirection` will return more accurate output.

Warning: Offset values change by temperature or some reason. If you need high accuracy values, you should recheck the offset value.

Example about auto calibration (calculating offset values) is [here](https://github.com/asukiaaa/MPU9250_asukiaaa/blob/master/examples/GetMagOffset/GetMagOffset.ino).

## Temperature Sensor

The temperature sensor outputs digital value of the die temperature. This can be used for temperature compensation.

Example program:
```c
#include <MPU9250_VMA.h>
MPU9250 mySensor;
float dieTemp;

void setup() {
  Wire.begin();
  mySensor.setWire(&Wire);
  mySensor.beginTemp();
}

void loop() {
  mySensor.readTemp(); //reads the sensor data
  dieTemp = mySensor.getTemp(); //returns temperature in °C

  // Do what you want
}
```

### Functions Available

```c
1. void beginTemp(float o = VAL_TEMP_ROOM_OFFSET, float s = VAL_TEMP_SENSITIVITY);
```
Initializes the temperature sensor with offset and sensitivity. The sensitivity is given as 333.87 and the room temperature offset as 21°C.

```c
VAL_TEMP_ROOM_OFFSET
VAL_TEMP_SENSITIVITY
```

```c
2. void readTemp();
```
Reads the temperature registers to a 2 byte array.

```c
3. float getTemp();
```
Returns the temperature in °C.


## With customizable Wire
For ESP8266, ESP32 and so on.

```c
void setup() {
  Wire.begin(26, 25); //sda, scl
  mySensor.setWire(&Wire);
}
```

# License
MIT

# References
- [ArduinoでMPU9250（加速度センサ、磁気センサ）を使う方法](http://asukiaaa.blogspot.jp/2017/07/arduinompu9250.html)
- [MPU-9250 Register Map and Descriptions](https://cdn.sparkfun.com/assets/learn_tutorials/5/5/0/MPU-9250-Register-Map.pdf)
- [HaLakeKit-Library](https://github.com/nyampass/HaLakeKit-Library)
- [Aliexpress MPU9250](https://www.aliexpress.com/wholesale?catId=0&initiative_id=AS_20170706234529&SearchText=MPU9250)
- [AK8963 datasheet](https://strawberry-linux.com/pub/AK8963.pdf)
- [jrowberg/i2cdevlib/Arduino/AK8963](https://github.com/jrowberg/i2cdevlib/tree/master/Arduino/AK8963)
- [SparkFun_MPU-9250](https://github.com/sparkfun/SparkFun_MPU-9250_Breakout_Arduino_Library/blob/master/src/MPU9250.cpp)
- [「MPU-9250 ９軸センサモジュール (メーカー品番：MPU-9250)」を使う](https://qiita.com/boyaki_machine/items/915f7730c737f2a5cc79)
- [【PSoC】MPU-9250のデータを取得（加速度、ジャイロ編）](http://amamitokachi.com/2017/04/12/post-71/)
