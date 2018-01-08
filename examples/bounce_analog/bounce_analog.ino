#include <Bounce2.h>
/* Four button on one analog input.
 * Example for Arduino Nano:                      |------------
 *                  ____     ____     ____        |
 * +5V ------------|____|---|____|---|____|--     | Arduino
 *               |        |        |         |    |
 *         ____  /        /        /         /    |
 * GND ---|____|-|--------|--------|---------|----| A3
 *                                                |
 * All resistors 220 ohm.                         |     
 */

#define GPIO_ANALOG  3
#define MAX_ANALOG   1024
#define HYSTERESE    50
#define NO_BUTTOS    4

BounceAnalog deBouncedButtons[NO_BUTTOS];

void setup() {
  for(int i = 0; i<NO_BUTTOS; i++)
  {
    deBouncedButtons[i] = BounceAnalog();
    int analogLevel = (MAX_ANALOG-HYSTERESE)/(i+1);
    //deBouncedButtons[i].attach(GPIO_ANALOG);
    deBouncedButtons[i].attachAnalog(GPIO_ANALOG, analogLevel, HYSTERESE);
  }

  Serial.begin(115200);
  Serial.println("Setup done");
}

void loop() {
  for(int i = 0; i<NO_BUTTOS; i++)
  {
    deBouncedButtons[i].update();
    //Serial.println(analogRead(GPIO_ANALOG));
    
    if( deBouncedButtons[i].rose() )
    {
      Serial.print(i);
      Serial.print(": rose() state ");
      Serial.println(deBouncedButtons[i].read());
    }
    if( deBouncedButtons[i].fell() )
    {
      Serial.print(i);
      Serial.print(": fell() state ");
      Serial.println(deBouncedButtons[i].read());
    }
  }
  delay(1);
}
