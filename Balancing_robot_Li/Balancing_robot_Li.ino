#include <Wire.h>                                            //Include the Wire.h library so we can communicate with the gyro
int MPU = 0x68;                                              //MPU-6050 I2C address (0x68 or 0x69)
int acc_calibration_value = 300;                            //Enter the accelerometer calibration value (IMPORTANT to find yours)
float pid_p_gain = 15;                                       //Gain setting for the P-controller (15)
float pid_i_gain = 1.1;                                      //Gain setting for the I-controller (1.5)
float pid_d_gain = 30;                                       //Gain setting for the D-controller (30)
long delayToBeAdded, counter_motor, gyro_pitch_calibration_value;
int receive_counter, gyro_pitch_data_raw, accelerometer_data_raw;
unsigned long loop_timer;
float angle_gyro, angle_acc, pid_error_temp, pid_i_mem, pid_output, pid_last_d_error;

void setup(){
  Wire.begin();                                                             //Start the I2C bus as master
  TWBR = 12;                                                                //Set the I2C clock speed to 400kHz
  //To create a variable pulse for controlling the stepper motors a timer is created that will execute a piece of code (subroutine) every 20us
  //This subroutine is called TIMER2_COMPA_vect
  TCCR2A = 0;                                                               //Make sure that the TCCR2A register is set to zero
  TCCR2B = 0;                                                               //Make sure that the TCCR2A register is set to zero
  TIMSK2 |= (1 << OCIE2A);                                                  //Set the interupt enable bit OCIE2A in the TIMSK2 register
  TCCR2B |= (1 << CS21);                                                    //Set the CS21 bit in the TCCRB register to set the prescaler to 8
  OCR2A = 39;                                                               //The compare register is set to 39 => 20us / (1s / (16.000.000MHz / 8)) - 1
  TCCR2A |= (1 << WGM21);                                                   //Set counter 2 to CTC (clear timer on compare) mode
  
  //By default the MPU-6050 sleeps. So we have to wake it up.
  Wire.beginTransmission(MPU);                                              //Start communication with the address found during search.
  Wire.write(0x6B);                                                         //We want to write to the PWR_MGMT_1 register (6B hex)
  Wire.write(0x00);                                                         //Set the register bits as 00000000 to activate the gyro
  Wire.endTransmission();                                                   //End the transmission with the gyro.
  //Set the full scale of the gyro to +/- 250 degrees per second
  Wire.beginTransmission(MPU);                                              //Start communication with the address found during search.
  Wire.write(0x1B);                                                         //We want to write to the GYRO_CONFIG register (1B hex)
  Wire.write(0x00);                                                         //Set the register bits as 00000000 (250dps full scale)
  Wire.endTransmission();                                                   //End the transmission with the gyro
  //Set the full scale of the accelerometer to +/- 4g.
  Wire.beginTransmission(MPU);                                              //Start communication with the address found during search.
  Wire.write(0x1C);                                                         //We want to write to the ACCEL_CONFIG register (1A hex)
  Wire.write(0x08);                                                         //Set the register bits as 00001000 (+/- 4g full scale range)
  Wire.endTransmission();                                                   //End the transmission with the gyro
  //Set some filtering to improve the raw data. CRITICAL!!
  Wire.beginTransmission(MPU);                                              //Start communication with the address found during search
  Wire.write(0x1A);                                                         //We want to write to the CONFIG register (1A hex)
  Wire.write(0x03);                                                         //Set the register bits as 00000011 (Set Digital Low Pass Filter to ~43Hz)
  Wire.endTransmission();                                                   //End the transmission with the gyro 

  pinMode(2, OUTPUT);                                                       //Configure digital poort 2 as output
  pinMode(3, OUTPUT);                                                       //Configure digital poort 3 as output
  pinMode(4, OUTPUT);                                                       //Configure digital poort 4 as output
  pinMode(5, OUTPUT);                                                       //Configure digital poort 5 as output
  pinMode(13, OUTPUT);                                                      //Configure digital poort 13 as output

  for(receive_counter = 0; receive_counter < 500; receive_counter++){       //Create 500 loops
    if(receive_counter % 15 == 0)digitalWrite(13, !digitalRead(13));        //Change the state of the LED every 15 loops to make the LED blink fast
    Wire.beginTransmission(MPU);                                            //Start communication with the gyro
    Wire.write(0x45);                                                       //Gyro Y_OUT
    Wire.endTransmission();                                                 //End the transmission
    Wire.requestFrom(MPU, 2);                                               //Request 2 bytes from the gyro
    gyro_pitch_calibration_value += Wire.read()<<8|Wire.read();             //Combine the two bytes to make one integer
    delayMicroseconds(3700);                                                //Wait for 3700 microseconds to simulate the main program loop time, CRITICALL!!
  }
  gyro_pitch_calibration_value /= 500;                                      //Divide the total value by 500 to get the avarage gyro offset
  loop_timer = micros() + 4000;                                             //Set the loop_timer variable at the next end loop time
}

void loop(){
  Wire.beginTransmission(MPU);                                              //Start communication with the gyro
  Wire.write(0x3F);                                                         //Start reading at register 3F
  Wire.endTransmission();                                                   //End the transmission
  Wire.requestFrom(MPU, 2);                                                 //Request 2 bytes from the gyro
  accelerometer_data_raw = Wire.read()<<8|Wire.read();                      //Combine the two bytes to make one integer
  accelerometer_data_raw += acc_calibration_value;                          //Add the accelerometer calibration value
  if(accelerometer_data_raw > 8192)accelerometer_data_raw = 8192;           //Limiting the acc data to +/-8192;
  if(accelerometer_data_raw < -8192)accelerometer_data_raw = -8192;         
  angle_acc = asin((float)accelerometer_data_raw/8192.0)* 57.296;           //Calculate the current angle according to the accelerometer
  Wire.beginTransmission(MPU);                                              //Start communication with the gyro
  Wire.write(0x45);                                                         //Start reading at register 45
  Wire.endTransmission();                                                   //End the transmission
  Wire.requestFrom(MPU, 2);                                                 //Request 2 bytes from the gyro
  gyro_pitch_data_raw = Wire.read()<<8|Wire.read();                         //Combine the two bytes to make one integer
  gyro_pitch_data_raw -= gyro_pitch_calibration_value;                      //Add the gyro calibration value
  angle_gyro += gyro_pitch_data_raw * 0.000031;                             //Calculate the traveled during this loop angle and add this to the angle_gyro variable
  angle_gyro = angle_gyro * 0.9996 + angle_acc * 0.0004;                    //Correct the drift of the gyro angle with the accelerometer angle
  pid_error_temp = angle_gyro-0;                                            //Find the error between the current angle and 0
  
  pid_i_mem += pid_i_gain * pid_error_temp;                                 //Calculate the I-controller value and add it to the pid_i_mem variable
  if(pid_i_mem > 400)pid_i_mem = 400;                                       //Limit the I-controller to the maximum controller output
  else if(pid_i_mem < -400)pid_i_mem = -400;
  //Calculate the PID output value
  pid_output = pid_p_gain * pid_error_temp + pid_i_mem + pid_d_gain * (pid_error_temp - pid_last_d_error);
  if(pid_output > 400)pid_output = 400;                                     //Limit the PI-controller to the maximum controller output
  else if(pid_output < -400)pid_output = -400;
  pid_last_d_error = pid_error_temp;                                        //Store the error for the next loop
  if(pid_output < 5 && pid_output > -5)pid_output = 0;                      //Create a dead-band to stop the motors when the robot is balanced
  if(pid_output==0){ // if the car is balanced, do not output any power to the motor
    delayToBeAdded=0;
  }
  else {
    delayToBeAdded = 5500/(abs(pid_output) + 9);
  }
  //The angle calculations are tuned for a loop time of 4 milliseconds. To make sure every loop is exactly 4 milliseconds a wait loop
  //is created by setting the loop_timer variable to +4000 microseconds every loop.
  while(loop_timer > micros()); // while loop_timer > micros(), hold there and wait
  loop_timer += 4000;           // once loop_timer < micros(), add 4000 us to loop_timer.
}

ISR(TIMER2_COMPA_vect){
  counter_motor ++; 
  if (counter_motor > delayToBeAdded){
    counter_motor=0;
    if (pid_output>0){
      PORTD &= 0b00000000; //0b00DSDS00 (D: Dir, S: Step), make all zeros is for the next line to let the two wheels rototae in the different directions. 
      PORTD |= 0b00001000; //Two different directions for the two steppers. 
    }
    else {
      PORTD &= 0b00000000; 
      PORTD |= 0b00100000; // Change the directions
    }
  }
  else if(counter_motor == 1)PORTD |= 0b00010100;             //Set output 2 high to create a pulse for the stepper controller
  else if(counter_motor == 2)PORTD &= 0b11101011;             //Set output 2 low because the pulse only has to last for 20us 
}
