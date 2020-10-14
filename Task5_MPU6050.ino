// Example 6, Gyro only
#include<Wire.h>
const int MPU = 0x68; //Device address
float GyroX, GyroY, GyroZ;// X, Y, and Z angluar speed values
float gyroAngleX, gyroAngleY, gyroAngleZ;
float previousTime, currentTime, elapsedTime;
float AccX, AccY, AccZ;// X, Y, and Z acceleration values 

void setup() {
  Serial.begin(9600);
  Wire.begin();
  Wire.beginTransmission(MPU);
  Wire.write(0x6B);//Reset the device, very important
  Wire.write(0x00);
  Wire.endTransmission(true);
  // Configure Gyroscope Sensitivity
  Wire.beginTransmission(MPU);
  //Wire.write(0x1B);// Talk to the GYRO_CONFIG register
  Wire.write(0x1C);//Talk to the ACCEL_CONFIG register (1C hex)
  Wire.write(0x08);// Set the register bits as 0000 1000 (500 deg/s full scale)
  Wire.endTransmission(true);
}

void loop() {
  Wire.beginTransmission(MPU);//Start communication with the gyro
  Wire.write(0x3F);//Start reading at register 3F
  Wire.endTransmission(); //End the transmission
  Wire.requestFrom(MPU, 2);//Request 2 bytes from the gyro
  float  accelerometer_data_raw = Wire.read() << 8 | Wire.read(); //Combine the two bytes to make one integer
  accelerometer_data_raw -= 450;//Add the accelerometer calibration value
  float  angle_acc = asin((float)accelerometer_data_raw / 8192.0) * 57.296;        //Calculate the current angle according to the accelerometer
  Serial.println(angle_acc);
  delay(500);
}

/*
  //// Example 5, MPU6050 test, accel only
  #include<Wire.h>
  const int MPU=0x68;//Device address
  float AccX;// X acceleration value

  void setup(){
  Serial.begin(9600);
  Wire.begin();
  // Configure Accelerometer Sensitivity
  Wire.beginTransmission(MPU);
  Wire.write(0x6B);//Reset the device, very important
  Wire.write(0x00);
  Wire.endTransmission(true);
  Wire.beginTransmission(MPU);
  Wire.write(0x1C);//Talk to the ACCEL_CONFIG register (1C hex)
  Wire.write(0x08);//Set the register bits as 00001000 (+/- 4g full scale range)
  Wire.endTransmission(true);
  }

  void loop(){
  Wire.beginTransmission(MPU);
  Wire.write(0x3B);//Start with register 0x3B
  Wire.endTransmission(false);
  Wire.requestFrom(MPU,2,true);// Read 2 registers total
  AccX=Wire.read()<<8 | Wire.read();
  Wire.endTransmission(true);
  Serial.println(AccX);
  delay(500);
  }
*/
