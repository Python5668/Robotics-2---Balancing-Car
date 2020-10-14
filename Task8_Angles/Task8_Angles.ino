// Task 8 Combining Acceleration and Gyrocope values
#include<Wire.h>
const int MPU = 0x68; //Device address
float gyro_pitch_calibration_value, angle_gyro;

void setup() {
  Serial.begin(9600);
  Wire.begin();
  // Configure Accelerometer Sensitivity
  Wire.beginTransmission(MPU);
  Wire.write(0x6B);             //Reset the device, very important
  Wire.write(0x00);
  Wire.endTransmission(true);
  // Acceleraction Configuration
  Wire.beginTransmission(MPU);
  Wire.write(0x1C);             //Talk to the ACCEL_CONFIG register (1C hex)
  Wire.write(0x08);             //Set the register bits as 00001000 (+/- 4g full scale range)
  Wire.endTransmission(true);
  // Gyroscope Configuration
  Wire.beginTransmission(MPU);
  Wire.write(0x1B);             //Talk to the ACCEL_CONFIG register (1C hex)
  Wire.write(0x08);             //Set the register bits as 00001000 (+/- 4g full scale range)
  Wire.endTransmission(true);
}

void loop() {
  Wire.beginTransmission(MPU);    //Start communication with the gyro
  Wire.write(0x3F);               //Start reading at register 3F
  Wire.endTransmission(true);         //End the transmission
  Wire.requestFrom(MPU, 2);
  float accelerometer_data_raw = Wire.read() << 8 | Wire.read();            //Combine the two bytes to make one integer
  accelerometer_data_raw += 150;                                            //Add the accelerometer calibration value
  float angle_acc = asin((float)accelerometer_data_raw / 8192.0) * 57.296;  //Calculate the current angle according to the accelerometer
  
  for (int receive_counter = 0; receive_counter < 500; receive_counter++) { //Create 500 loops
    if (receive_counter % 15 == 0) digitalWrite(13, !digitalRead(13)); //Change the state of the LED every 15 loops to make the LED blink fast
    Wire.beginTransmission(MPU); //Start communication with the gyro
    Wire.write(0x45); //Gyro Y_OUT
    Wire.endTransmission(true); //End the transmission
    Wire.requestFrom(MPU, 2); //Request 2 bytes from the gyro
    gyro_pitch_calibration_value += Wire.read() << 8 | Wire.read(); //Combine the two bytes to make one integer
    delayMicroseconds(3700);  //Wait for 3700 microseconds to simulate the main program loop time, CRITICAL!!
  }
  gyro_pitch_calibration_value /= 500;

  Wire.beginTransmission(MPU);    //Start communication with the gyro
  Wire.write(0x45);               //Start reading at GYRO_YOUT[15:3]
  Wire.endTransmission(true);     //End the transmission
  Wire.requestFrom(MPU, 2);       //Request 2 bytes from the gyro

  float gyro_pitch_data_raw = Wire.read() << 8 | Wire.read();
  gyro_pitch_data_raw -= gyro_pitch_calibration_value;  //Subtract the gyro calibration value
  angle_gyro += gyro_pitch_data_raw * 0.000031;         //Calculate the traveled angle during this loop angle and add this to the angle_gyro variable

  angle_gyro = angle_gyro * 0.9996 + angle_acc * 0.0004;      //Correct the drift of the gyro angle with the accelerometer angle

  Serial.println(angle_gyro);
}
