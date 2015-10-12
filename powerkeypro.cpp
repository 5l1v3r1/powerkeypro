#include "powerkeypro.h"

POWERKEYPRO::POWERKEYPRO(int chan)
{
	if (chan == 0)
	{
		channel = &Can0;
	}
	else
	{
		channel = &Can1;
	}
	for (int x = 0; x < 15; x++) 
	{
		buttonState[x] = false;
		actualState[x] = false;
		toggleState[x] = false;
		keyType[x] = MOMENTARY;
	}
	keypadID = 0x15; //the default ID
	listener = NULL;
	usedMailbox = -1;
}

void POWERKEYPRO::begin(int speed, int id)
{	
	keypadID = id;
	channel->begin(speed);
	usedMailbox = channel->watchFor(0x180 + keypadID);
	channel->attachObj(this);
	attachMBHandler(usedMailbox);
}

void POWERKEYPRO::startKeypad()
{
	CAN_FRAME outFrame;
	outFrame.id = 0;
	outFrame.rtr = 0;
	outFrame.length = 8;
	outFrame.extended = false;
	outFrame.data.bytes[0] = 01; //start the node
	outFrame.data.bytes[1] = keypadID;
	outFrame.data.bytes[2] = 0;
	outFrame.data.bytes[3] = 0;
	outFrame.data.bytes[4] = 0;
	outFrame.data.bytes[5] = 0;
	outFrame.data.bytes[6] = 0;
	outFrame.data.bytes[7] = 0;
	channel->sendFrame(outFrame);

	CAN_FRAME out2;
	out2.id = 600 + keypadID;
	out2.rtr = 0;
	out2.length = 8;
	out2.extended = false;
	out2.data.bytes[0] = 0x23;
	out2.data.bytes[1] = 0;
	out2.data.bytes[2] = 0x65;
	out2.data.bytes[3] = 1;	
	out2.data.bytes[4] = 0x10;
	out2.data.bytes[5] = 1;
	out2.data.bytes[6] = 0;
	out2.data.bytes[7] = 0;
	channel->sendFrame(out2);
}

bool POWERKEYPRO::isConnected()
{
}

void POWERKEYPRO::setKeypadID(int id)
{
	CAN_FRAME outFrame;
	outFrame.id = 0x600 + keypadID;
	outFrame.rtr = 0;
	outFrame.extended = false;
	outFrame.length = 8;
	outFrame.data.byte[0] = 0x23;
	outFrame.data.byte[1] = 0;
	outFrame.data.byte[2] = 0x65;
	outFrame.data.byte[3] = 1;
	outFrame.data.byte[4] = 0x70;
	outFrame.data.byte[5] = id;
	outFrame.data.byte[6] = 0;
	outFrame.data.byte[7] = 0;
	channel->sendFrame(outFrame);

	keypadID = id;
	//set the filter to the new ID so we still get traffic.
	channel->setRXFilter(usedMailbox, 0x180 + keypadID, 0x7FF, false);
}

//Not actually in existence yet...
int POWERKEYPRO::findKeypadID()
{
	return -1;
}

void POWERKEYPRO::registerListener(KeyListener *listener)
{
    this->listener = listener;
}

void POWERKEYPRO::setMomentary(int key)
{
	if (key < 0) return;
	if (key > 14) return;
	keyType[key] = MOMENTARY;
}

void POWERKEYPRO::setToggle(int key)
{
	if (key < 0) return;
	if (key > 14) return;
	keyType[key] = TOGGLING;
}

void POWERKEYPRO::setLatching(int key)
{
	if (key < 0) return;
	if (key > 14) return;
	keyType[key] = LATCHING;
}

bool POWERKEYPRO::getKey(int key)
{
	boolean retVal = buttonState[key];
	if (key < 0) return false;
	if (key > 14) return false;
	if (keyType[key] == LATCHING) buttonState[key] = actualState[key];
	return retVal;
}

void POWERKEYPRO::setLED(int LED, LEDTYPE newState)
{
	CAN_FRAME frame;	
	frame.id = 0x600 + keypadID;
	frame.extended = false;
	frame.rtr = 0;
	frame.length = 8;
	frame.data.bytes[0] = 0x23;
	frame.data.bytes[1] = 0;
	frame.data.bytes[2] = 0x65;
	frame.data.bytes[3] = 1;
	frame.data.bytes[4] = 1;
	frame.data.bytes[5] = LED;
	frame.data.bytes[6] = (int)newState;
	frame.data.bytes[7] = 0;
	
	channel->sendFrame(frame);
}

LEDTYPE POWERKEYPRO::getLED(int LED)
{
	if (LED < 0) return OFF;
	if (LED > 14) return OFF;
	return LEDState[LED];
}

void POWERKEYPRO::setLEDBrightness(int bright)
{
	if (bright < 0) return;
	if (bright > 0x3F) return;
	CAN_FRAME frame;	
	frame.id = 0x600 + keypadID;
	frame.extended = false;
	frame.rtr = 0;
	frame.length = 8;
	frame.data.bytes[0] = 0x23;
	frame.data.bytes[1] = 0;
	frame.data.bytes[2] = 0x65;
	frame.data.bytes[3] = 1;
	frame.data.bytes[4] = 2;
	frame.data.bytes[5] = bright;
	frame.data.bytes[6] = 0;
	frame.data.bytes[7] = 0;
	
	channel->sendFrame(frame);
}

void POWERKEYPRO::setBacklightBrightness(int bright)
{
	if (bright < 0) return;
	if (bright > 0x3F) return;
	CAN_FRAME frame;	
	frame.id = 0x600 + keypadID;
	frame.extended = false;
	frame.rtr = 0;
	frame.length = 8;
	frame.data.bytes[0] = 0x23;
	frame.data.bytes[1] = 0;
	frame.data.bytes[2] = 0x65;
	frame.data.bytes[3] = 1;
	frame.data.bytes[4] = 3;
	frame.data.bytes[5] = bright;
	frame.data.bytes[6] = 0;
	frame.data.bytes[7] = 0;
	
	channel->sendFrame(frame);
}

void POWERKEYPRO::setDeviceCANSpeed(int speed)
{
	uint8_t selection = 0;
	int baudrate = 125000;
	if (speed == 250000 || speed == 250 || speed == 1)
	{
		selection = 1;
		baudrate = 250000;
	}
	if (speed == 500000 || speed == 500 || speed == 2)
	{
		selection = 2;
		baudrate = 500000;
	}
	CAN_FRAME frame;	
	frame.id = 0x600 + keypadID;
	frame.extended = false;
	frame.rtr = 0;
	frame.length = 8;
	frame.data.bytes[0] = 0x23;
	frame.data.bytes[1] = 0;
	frame.data.bytes[2] = 0x65;
	frame.data.bytes[3] = 1;
	frame.data.bytes[4] = 0x11;
	frame.data.bytes[5] = selection;
	frame.data.bytes[6] = 0;
	frame.data.bytes[7] = 0;
	
	channel->sendFrame(frame);
	
	delay(5);
	
	channel->set_baudrate(baudrate);
}

void POWERKEYPRO::gotFrame(CAN_FRAME* frame, int mailbox)
{
	int outputValue;
	if (frame->id == (0x180 + keypadID)) //notification that key state has changed
	{		
		for (int bit = 0; bit < 15; bit++)
		{
			if (frame->data.byte[bit / 8] & (1 << (bit % 8)))
			{
				actualState[bit] = true;
				if (keyType[bit] == MOMENTARY || keyType[bit] == LATCHING) buttonState[bit] = true;
				if (keyType[bit] == TOGGLING)
				{
					if (!toggleState[bit])
					{
						 toggleState[bit] = true;
						 buttonState[bit] = !buttonState[bit];
					}
				}
			}
			else
			{
				actualState[bit] = false;
				if (keyType[bit] == MOMENTARY) buttonState[bit] = false;
				if (keyType[bit] == TOGGLING) toggleState[bit] = false;
			}
		}
		if (listener != NULL) 
		{
			outputValue = frame->data.byte[0] + (frame->data.byte[1] * 256) + (frame->data.byte[2] * 65536);
			listener->keyEvent(outputValue);
		}
	}
}

//empty function to give the linker something to look for as a default implementation
void KeyListener::keyEvent(int keyStates)
{

}

POWERKEYPRO PowerKeyPro0(0);
POWERKEYPRO PowerKeyPro1(1);
