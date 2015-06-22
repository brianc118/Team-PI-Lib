#include <pwmMotor.h>

PMOTOR MOTORA(2,23,25);
PMOTOR MOTORB(3,27,29);
PMOTOR MOTORC(4,31,33);

void setup(){
  
}
void loop(){
  for(int i = 0; i < 256; i++){
    MOTORA.move(i);
    MOTORB.move(i);
    MOTORC.move(i);
    delay(10);
  }
}
