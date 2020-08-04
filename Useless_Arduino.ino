#include <SoftRcPulseOut.h>
//#include <SimpleServo.h>

//Board used: Digispark Tiny 85
//Select Board: Digispark (Default - 16.5mhz)

#define SERVO_PIN 0

#define SWITCH_PIN 2
#define SWITCH_OUT 3

#define POS_START 180
#define POS_END 50

SoftRcPulseOut myservo;
int switchState;
int pos = POS_START;

void setup() {
  pinMode(SWITCH_PIN, INPUT_PULLUP);
  pinMode(SWITCH_OUT, OUTPUT);
  digitalWrite(SWITCH_OUT, LOW);

  myservo.attach(SERVO_PIN);
  myservo.write(POS_START);
  delay(25);
  SoftRcPulseOut::refresh(1);
}

void loop() {
  switchState = digitalRead(SWITCH_PIN);
  if(switchState==LOW) {

    for (pos = POS_START; pos >= POS_END; pos -= 4) {
      myservo.write(pos);
      delay(10);
      SoftRcPulseOut::refresh(1);    // generates the servo pulse Now
    }
    delay(10);
    for (pos = POS_END; pos <= POS_START; pos += 2) {
      myservo.write(pos);
      delay(10);
      SoftRcPulseOut::refresh(1);    // generates the servo pulse Now
    }

  } else {
    delay(50);
  }
}
