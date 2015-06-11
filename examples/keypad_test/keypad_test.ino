//Sets up to become the CANOpen master node on CAN0 and utilize a Powerkey Pro 2600
#include "variant.h"'
#include <due_can.h>
#include <due_canopen.h>
#include <powerkeypro.h>

//Leave defined if you use native port, comment if using programming port
#define Serial SerialUSB

void keypresses(int keys)
{
	Serial.print("Keystate changes! ");
	Serial.println(keys, HEX);
}

void setup()
{

	Serial.begin(115200);
	PowerKeyPro0.begin(500000, 0x15);
	PowerKeyPro0.setKeyCallback(keypresses);
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
	PowerKeyPro0.setLEDState(led + 1, (LEDTYPE)state);
}
