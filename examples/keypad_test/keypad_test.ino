//Set up for CAN0 and using a PowerKeyPro 2600 (though it should work fine on other models as well)
#include "variant.h"
#include <due_can.h>
#include <powerkeypro.h>

//Leave defined if you use native port, comment if using programming port
#define Serial SerialUSB

class ExampleClass : public KeyListener
{
public:
	void keyEvent(int keyStates);
};
 
void ExampleClass::keyEvent(int keyStates)
{
	Serial.print("New key state: ");
	Serial.println(keyStates, HEX);
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
	static int led, state;

	delay(200);
	led++;
	if (led > 11)
	{
		led = 0;
		state++;
		if (state > 11) state = 0;
	}
	PowerKeyPro0.setLED(led + 1, (LEDTYPE)state);
}
