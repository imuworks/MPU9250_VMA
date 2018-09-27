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

## Accelerometer
MPU6050 has a 3-axis accelerometer with 16-bit output with selectable ranges up to ±16g.
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

## Gyrometer
MPU6050 has a 3-axis gyroscope with 16-bit output code with selectable ranges up to ±2000 °/S.

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

## Magnetometer
AK8963 is a 3-axis magnetometer or compass with 14-bit (0.6 μT/LSB typ) or 16-bit 90.15μT/LSB typ) selectable output and works based on hall effect. Default operation mode is continuous measurement at 8Hz and output length is 16-bit.
```c
#include <MPU9250_VMA.h>
MPU9250 mySensor;
float mDirection;
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

If you get values of sensor like this..

Name | Max | Min
-----|----:|----:
magX |  70 | -30
maxY | 110 |  10

You can calculate offset values like this.

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

Then set the offset values like this.

```c
void setup() {
  mySensor.magXOffset = -20;
  mySensor.magYOffset = -60;
}
```

Then you can get like this.

Name | Max | Min
-----|----:|----:
magX |  50 | -50
maxY |  50 | -50

After setting offset value, you can get `magHorizDirection` as you expected.

Warning: Offset value changes by temperature or some reason. If you want to get high accuracy value, you should recheck the offset value.

Example about auto calibration (calculating offset values) is [here](https://github.com/asukiaaa/MPU9250_asukiaaa/blob/master/examples/GetMagOffset/GetMagOffset.ino).

## Temperature Sensor

The temperature sensor outputs digital value of the die temperature. This can be used for temperature compensation.

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
