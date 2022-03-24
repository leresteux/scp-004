//10kOhm pull up 
#include <Servo.h>

byte pos=0;
Servo servo;


void setup() {
  servo.attach(4);

}
void loop() {
  for ( pos; pos < 175; pos++) {
    servo.write(pos);
    delay(100);
  }
  for ( pos; pos > 5; pos--) {
    servo.write(pos);
    delay(100);
  }
}
