// Tyrone's Acceleration and Gyroscope calibration Code Reference
// The original code was provided by Dr. Li @ http://yilectronics.com/Courses/CE432_RoboticsII/lectures/balancedCar/docs/ex5_ex6.txt

// Comment/Uncomment = Ctrl+/

// Example 6, Gyro only
#include<Wire.h>
const int MPU=0x68;//Device address
float GyroX, GyroY, GyroZ;// X, Y, and Z angluar speed values 
float gyroAngleX, gyroAngleY, gyroAngleZ;
float previousTime, currentTime, elapsedTime;
void setup(){
  Serial.begin(9600);
  Wire.begin();
  Wire.beginTransmission(MPU);
  Wire.write(0x6B);//Reset the device, very important
  Wire.write(0x00);
  Wire.endTransmission(true);
// Configure Gyroscope Sensitivity
  Wire.beginTransmission(MPU);
  Wire.write(0x1B);// Talk to the GYRO_CONFIG register
  Wire.write(0x08);// Set the register bits as 0000 1000 (500 deg/s full scale)
  Wire.endTransmission(true);
}
void loop(){
  previousTime = currentTime;// Previous time is stored before the actual time read
  currentTime = millis();// Current time actual time read
  elapsedTime = (currentTime - previousTime) / 1000; // Divide by 1000 to get seconds
  Wire.beginTransmission(MPU);
  Wire.write(0x43); // Gyro data first register address 0x43
  Wire.endTransmission(false);
  Wire.requestFrom(MPU, 6, true); // Read 6 registers total, each axis value is stored in 2 registers
  GyroX = (Wire.read() << 8 | Wire.read()) / 65.5; // For a 500 deg/s range 
  GyroY = (Wire.read() << 8 | Wire.read()) / 65.5;
  GyroZ = (Wire.read() << 8 | Wire.read()) / 65.5;
  gyroAngleX=GyroX*elapsedTime;
  gyroAngleY=GyroY*elapsedTime;
  gyroAngleZ=GyroZ*elapsedTime;  
  Serial.print(gyroAngleX);
  Serial.print(" ");
  Serial.print(gyroAngleY);
  Serial.print(" ");
  Serial.println(gyroAngleZ);
}

// Example 5, MPU6050 test, accel only
#include<Wire.h>
const int MPU=0x68;//Device address
float AccX, AccY, AccZ;// X, Y, and Z acceleration values 
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
  Wire.requestFrom(MPU,6,true);// Read 6 registers total
  AccX=(Wire.read()<<8 | Wire.read())/8192.0;
  AccY=(Wire.read()<<8 | Wire.read())/8192.0;
  AccZ=(Wire.read()<<8 | Wire.read())/8192.0;
  Wire.endTransmission(true);
  Serial.print(AccX);
  Serial.print(" ");
  Serial.print(AccY);
  Serial.print(" ");
  Serial.println(AccZ);
}
