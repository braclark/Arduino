#include <SoftRcPulseOut.h>

//Board used: Digispark Tiny 85
//Select Board: Digispark (Default - 16.5mhz)

//Connect servo orange wire to pin 0, red to 5v, and brown to GND
//Also connect a capacitor in between red and brown to be able to move the servo faster
//Without the cap, more than 3 steps per 10 ms may cause a reset
#define SERVO_PIN 0

//Connect the switch accross pins 2 and 3
#define SWITCH_PIN 2
#define SWITCH_OUT 3

//Define the built in LED
#define LED_BUILTIN 1

//Defines the range of motion for the servo, starting(resting) and ending(hitting the switch)
#define POS_START 180
#define POS_END 45

//Once it get's mad (more than 4 switches within 10 seconds of eachother), should the responses be random?
//to set to sequential, start up with the switch on, otherwise the responses will be random


SoftRcPulseOut myservo;
int switchState;
int pos = POS_START;
int target = POS_START;
int seq = 0;
int rapidCount = -1;
long lastInput = 0;
bool goRandom;

void setup() {
  pinMode(SWITCH_PIN, INPUT_PULLUP);
  pinMode(SWITCH_OUT, OUTPUT);
  digitalWrite(SWITCH_OUT, LOW);

  pinMode(LED_BUILTIN,OUTPUT);   //Built in LED, flashes when ready, on when in attitude mode
  
  myservo.attach(SERVO_PIN);

  digitalWrite(LED_BUILTIN,HIGH);  // flash the led to show it's ready.
  gotoPercent(50,1,0);  // do a small initialization to show it's ready
  gotoPercent(0,1,0);
  digitalWrite(LED_BUILTIN,LOW);

  switchState = digitalRead(SWITCH_PIN);
  if (switchState==LOW){ //Reads the switch at startup to set random
    goRandom = false; 
  } else {
    goRandom = true;
  }

  randomSeed(analogRead(5)); //Seed the random function by reading an unconnected analog in pin
}

void loop() {
  switchState = digitalRead(SWITCH_PIN);
  if(switchState==LOW) {
    if ((millis()-lastInput)<10000) {  // if the last switch ended less than 10 seconds ago, increase the number of rapid switches
      rapidCount++;
      } else {
      rapidCount = 0;
      }
    lastInput = millis();  // set the last switch to now
    if(rapidCount < 4){   // if there have been fewer than 4 switches in a row with 10 seconds between them, just do the basic sequence.
      seq = 0;
      } else {
      if (goRandom == true) {
        seq = int(random(1,10));
        } else {
        seq++;
        if (seq>9){ seq=1; }
        }
      }
    if (seq == 0)Seq00();
    if (seq == 1)Seq01();
    if (seq == 2)Seq02();
    if (seq == 3)Seq03();
    if (seq == 4)Seq04();
    if (seq == 5)Seq05();
    if (seq == 6)Seq06();
    if (seq == 7)Seq07();
    if (seq == 8)Seq08();
    if (seq == 9)Seq09();
    lastInput = millis();  // set the last switch to now
    } else {
    delay(50);
    }
  if (millis()-lastInput < 10000 && rapidCount >= 3 ) {  //illuminate the builtin LED when it's mad or about to be mad
    digitalWrite(LED_BUILTIN,HIGH);
    } else {
    digitalWrite(LED_BUILTIN,LOW);
    }
}

void Seq00(){
  gotoPercent(100,3,0);
  gotoPercent(0,3,0);
}

void Seq01(){
  gotoPercent(100,3,0);
  gotoPercent(0,1,0);
}

void Seq02(){
  gotoPercent(100,1,2000);
  gotoPercent(0,7,0);
}

void Seq03(){
  gotoPercent(100,10,0);
  gotoPercent(75,1,1000);
  gotoPercent(100,10,0);
  gotoPercent(75,10,0);
  gotoPercent(100,10,0);
  gotoPercent(0,5,0);
}

void Seq04(){
  gotoPercent(25,1,500);
  gotoPercent(50,1,500);
  gotoPercent(75,1,2000);
  gotoPercent(100,10,0);
  gotoPercent(0,5,0);
}

void Seq05(){
  gotoPercent(50,5,0);
  gotoPercent(0,1,300);
  gotoPercent(75,5,0);
  gotoPercent(0,1,300);
  gotoPercent(80,5,1000);
  gotoPercent(70,5,0);
  gotoPercent(100,10,0);
  gotoPercent(0,3,0);
}

void Seq06(){
  gotoPercent(100,3,0);
  gotoPercent(80,1,500);
  gotoPercent(100,8,500);
  gotoPercent(80,1,500);
  gotoPercent(100,8,500);
  gotoPercent(80,1,500);
  gotoPercent(50,1,500);
  gotoPercent(25,1,500);
  gotoPercent(0,1,0);
}

void Seq07(){
  gotoPercent(100,2,0);
  for (int i=0; i<10; i++){
    gotoPercent(80,3,0);
    gotoPercent(100,6,0);
    }
  gotoPercent(100,6,2000);
  gotoPercent(50,1,500);
  gotoPercent(0,1,0);
}

void Seq08(){
  for (int i=30; i<=100; i+=10){
    gotoPercent(i,7,0);
    gotoPercent(i-30,4,0);
    }
  gotoPercent(0,5,0);
}

void Seq09(){
  for (int i=0; i<random(10,20); i++){
    gotoPercent(random(0,80),random(1,10),random(0,500));
  }
  gotoPercent(100,random(1,10),random(0,500));
  gotoPercent(0,random(1,10),0);
}

// Required parameters:
//   targetPC   - Target percent (as an integer) 0-100, 0 is starting position, 100 is hitting the switch
//   steps      - Steps per 10 ms update, higher numbers may cause the aurduino to reset without a capacitior
//                3 and under seems to work well without a cap, 11 seems to be the maximum otherwise
//   afterDelay - Adds a delay after completing the move
void gotoPercent(int targetPC, int steps, int afterDelay){
  target = POS_START + ((POS_END - POS_START) * targetPC * .01);
  while(pos != target){
    if (target < pos){
      if (pos-target < steps){
        pos=target;
        } else {
        pos -= steps;
        }
      } else {
      if (target-pos < steps){
        pos=target;
        } else {
        pos += steps;
        }
      }
    myservo.write(pos);
    SoftRcPulseOut::refresh(1);    // generate the servo pulse
    delay(10);
    }
  for (int delayTimer = 0; delayTimer<=afterDelay; delayTimer+=50){ // Refresh the location every 50 ms to keep it alive
    delay(50);
    SoftRcPulseOut::refresh(1);
    }
}
