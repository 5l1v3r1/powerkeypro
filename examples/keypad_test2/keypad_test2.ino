//Sets up to become the CANOpen master node on CAN0 and utilize a Powerkey Pro 2600
#include "variant.h"'
#include <due_can.h>
#include <due_canopen.h>
#include <powerkeypro.h>

//Leave defined if you use native port, comment if using programming port
#define Serial SerialUSB

int buttons[12];

void keypresses(int keys)
{
  for (int x = 0; x < 12; x++)
  {
     if ((keys & (1 << x)) == (1 << x)) 
     {
       Serial.print("Key ");
       Serial.print(x);
       Serial.println(" pressed");
       buttons[x] = (buttons[x] + 1) % 11;
       PowerKeyPro0.setLEDState(x + 1, (LEDTYPE)buttons[x]);
     }     
  }
}

void setup()
{
  Serial.begin(115200);
  PowerKeyPro0.begin(500000, 0x15);
  PowerKeyPro0.setKeyCallback(keypresses);

  for (int x = 0; x < 12; x++)
  {
    buttons[x] = 0;
  }  
}

void loop()
{
  delay(100);
}
