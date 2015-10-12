#ifndef POWERKEY_H_
#define POWERKEY_H_

#include <Arduino.h>
#include <due_can.h>

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

enum KEYTYPE
{
	MOMENTARY = 0,
	TOGGLING = 1,
	LATCHING = 2
};

class KeyListener
{
public:
	virtual void keyEvent(int keyStates);
};

class POWERKEYPRO : public CANListener
{
public:
	POWERKEYPRO(int); //canbus channel to use

	void begin(int speed, int id = DEFAULT_ID); //begin the comm, pass desired canbus speed - defaults to the default keybox speed
	bool isConnected(); //is a keypad responding to us?		
	void setSpeed(int); //set a new comm speed. Updates the keybox with this new value
	void setKeypadID(int id = 0x15); //that is the actual default ID they come with.
	int findKeypadID(); //listen on the bus to find the keybox and return it's ID
	void setDeviceCANSpeed(int speed);
	bool getKey(int key); //true = pressed, false = unpressed
	void setLED(int LED, LEDTYPE newState);
	LEDTYPE getLED(int LED);
	void setLEDBrightness(int bright);
	void setBacklightBrightness(int bright);
	void setMomentary(int key);
	void setToggle(int key);
	void setLatching(int key);
	void gotFrame(CAN_FRAME *frame, int mailbox);
	void registerListener(KeyListener *listener);
	void removeListener();

private:
	CANRaw *channel;
	int keypadID; //which ID is the keybox on?
	boolean buttonState[15]; //reported state of each key/input. Could be a lie depending on key type
	boolean actualState[15]; //the current actual state of each button
	boolean toggleState[15]; //used by any inputs set to TOGGLING
	LEDTYPE LEDState[15]; //LED state for all keys
	KEYTYPE keyType[15]; //what type is each input?
	KeyListener *listener;
	int usedMailbox;

	void startKeypad();
};

extern POWERKEYPRO PowerKeyPro0;
extern POWERKEYPRO PowerKeyPro1;

#endif
