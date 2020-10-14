// Example 3 NEMA17 stepper motor, drive the motor (A4988 driver)
// defines pins numbers
const int stepPin = 5; 
const int dirPin = 4;

const int speed = A0;
 
void setup() {
  // Sets the two pins as Outputs
  pinMode(stepPin,OUTPUT); 
  pinMode(dirPin,OUTPUT);
  digitalWrite(dirPin,HIGH); // Enables the motor to move in a particular direction
  Serial.begin(9600);
}
void loop() {
  // Makes 200 pulses for making one full cycle rotation
  int num = analogRead(speed);
  //Serial.println(num);
  if (num < 350) {
    for(int x = 0; x < 200; x++) {
      digitalWrite(stepPin,HIGH); 
      delayMicroseconds(350); 
      digitalWrite(stepPin,LOW); 
      delayMicroseconds(350);
    }     
  }
  else {
    for(int x = 0; x < 200; x++) {
      digitalWrite(stepPin,HIGH); 
      delayMicroseconds(num); 
      digitalWrite(stepPin,LOW); 
      delayMicroseconds(num); 
    }
  }
}
