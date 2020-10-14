#include<Wire.h>
const int MPU = 0x68; //Device address
float gyro_pitch_calibration_value;

void setup() {
  Serial.begin(9600);
  Wire.begin();
  Wire.beginTransmission(MPU);
  Wire.write(0x6B);//Reset the device, very important
  Wire.write(0x00);
  Wire.endTransmission(true);
  // Configure Gyroscope Sensitivity
  Wire.beginTransmission(MPU);
  Wire.write(0x1B);// Talk to the GYRO_CONFIG register
  //Wire.write(0x1C);//Talk to the ACCEL_CONFIG register (1C hex)
  Wire.write(0x08);// Set the register bits as 0000 1000 (500 deg/s full scale)
  Wire.endTransmission(true);
}

void loop() {
  for (int receive_counter = 0; receive_counter < 500; receive_counter++) { //Create 500 loops
    if (receive_counter % 15 == 0) digitalWrite(13, !digitalRead(13)); //Change the state of the LED every 15 loops to make the LED blink fast
    Wire.beginTransmission(MPU); //Start communication with the gyro
    Wire.write(0x45); //Gyro Y_OUT
    Wire.endTransmission(); //End the transmission
    Wire.requestFrom(MPU, 2); //Request 2 bytes from the gyro
    gyro_pitch_calibration_value += Wire.read() << 8 | Wire.read(); //Combine the two bytes to make one integer
    delayMicroseconds(3700);  //Wait for 3700 microseconds to simulate the main program loop time, CRITICAL!!
  }
  gyro_pitch_calibration_value /= 500;
  Serial.println(gyro_pitch_calibration_value);
  delay(500);
}
