unsigned long loop_timer; // 32-bit
bool mode = 0;

void setup() {
  pinMode(8, OUTPUT);
  loop_timer = micros() + 500000;
}

void loop() {
  if (mode == 0) {
    digitalWrite(8, HIGH);
    mode = 1;
  }
  else {
    digitalWrite(8, LOW);
    mode = 0;
  }

  while (micros() < loop_timer); // while micros() < loop_timer, hold there and wait
  loop_timer += 500000; // once micros() > loop_timer, add 500000 us to loop_timer.
}
