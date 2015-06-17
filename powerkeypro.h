#ifndef POWERKEY_H_
#define POWERKEY_H_

#include <Arduino.h>
#include <due_canopen.h> //powerkeypro connects over canopen link

#define DEFAULT_ID	0x15

enum LEDTYPE
{
	OFF = 0,
	RED = 1,
	RED_BLINK = 2,
	RED_ALT_BLINK = 3,
	GREEN = 4,
	GREEN_BLINK = 5,
	GREEN_ALT_BLINK = 6,
	AMBER = 7,
	AMBER_BLINK = 8,
	AMBER_ALT_BLINK = 9,
	RED_GREEN_FLASH	= 10,
	AMBER_RED_FLASH = 12,
	GREEN_AMBER_FLASH = 14
};

class POWERKEYPRO
{
public:
	POWERKEYPRO(int); //canbus channel to use

	void begin(int speed = 125000, int id = DEFAULT_ID); //begin the comm, pass desired canbus speed - defaults to the default keybox speed
	bool isConnected(); //is a keypad responding to us?		
	void setDeviceCANSpeed(int); //set a new comm speed. Updates the keybox with this new value
	void setKeypadID(int id = 0x15); //that is the actual default ID they come with.
	int findKeypadID(); //listen on the bus to find the keybox and return it's ID
	void setKeyCallback(void (*cb)(int));
	bool getKeyState(int key); //true = pressed, false = unpressed
	void setLEDState(int LED, LEDTYPE newState);
	LEDTYPE getLEDState(int LED);
	void loop();
	void onPDOReceive(CAN_FRAME* frame);
	void onSDORequest(SDO_FRAME* frame);
	void onSDOResponse(SDO_FRAME* frame);
private:
	CANOPEN *channel;
	int keypadID; //which ID is the keybox on?
	bool buttonState[12];
	LEDTYPE LEDState[12]; //LED state for all 12 keys
	void (*keystateChange)(int keyStates); //callback used when the keypad state changes (button up or down)
};

extern POWERKEYPRO PowerKeyPro0;
extern POWERKEYPRO PowerKeyPro1;

#endif