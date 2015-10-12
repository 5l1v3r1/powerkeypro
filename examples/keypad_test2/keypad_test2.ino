//Quick demo of the powerkeypro library and how to use it to turn the light on for a button when it is pressed.
#include "variant.h"'
#include <due_can.h>
#include <powerkeypro.h>

//Leave defined if you use native port, comment if using programming port
#define Serial SerialUSB

class ExampleClass : public KeyListener
{
public:
	void keyEvent(int keyStates);
	ExampleClass();
private:
	int buttons[12];
};

ExampleClass::ExampleClass()
{
  for (int x = 0; x < 12; x++)
  {
    buttons[x] = 0;
  }  
}
 
void ExampleClass::keyEvent(int keyStates)
{
  for (int x = 0; x < 12; x++)
  {
     if ((keyStates & (1 << x)) == (1 << x)) 
     {
       Serial.print("Key ");
       Serial.print(x);
       Serial.println(" pressed");
       buttons[x] = (buttons[x] + 1) % 11;
       PowerKeyPro0.setLED(x + 1, (LEDTYPE)buttons[x]);
     }     
  }
}

ExampleClass theClass;

void setup()
{
  Serial.begin(115200);
  PowerKeyPro0.begin(500000, 0x15);
  PowerKeyPro0.registerListener(&theClass);
}

void loop()
{
  delay(100);
}
