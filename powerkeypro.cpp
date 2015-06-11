#include "powerkeypro.h"

//trampoline functions to bounce back into a class member. Not as pretty or 
//proper as using a delegate scheme but nothing about delegates is that much fun
//in C++ so this is much easier to maintain.
void onPDOReceive0(CAN_FRAME *frame)
{
	PowerKeyPro0.onPDOReceive(frame);
}

void onSDORxRequest0(SDO_FRAME *frame)
{
	PowerKeyPro0.onSDORequest(frame);
}

void onSDORxResponse0(SDO_FRAME *frame)
{
	PowerKeyPro0.onSDOResponse(frame);
}

void onPDOReceive1(CAN_FRAME *frame)
{
	PowerKeyPro1.onPDOReceive(frame);
}

void onSDORxRequest1(SDO_FRAME *frame)
{
	PowerKeyPro1.onSDORequest(frame);
}

void onSDORxResponse1(SDO_FRAME *frame)
{
	PowerKeyPro1.onSDOResponse(frame);
}

POWERKEYPRO::POWERKEYPRO(int chan)
{
	if (chan == 0)
	{
		channel = &CanOpen0;
		channel->setPDOCallback(onPDOReceive0);
		channel->setSDOReqCallback(onSDORxRequest0);
		channel->setSDOReplyCallback(onSDORxResponse0);
	}
	else
	{
		channel = &CanOpen1;
		channel->setPDOCallback(onPDOReceive1);
		channel->setSDOReqCallback(onSDORxRequest1);
		channel->setSDOReplyCallback(onSDORxResponse1);
	}
	for (int x = 0; x < 12; x++) buttonState[x] = false;
	keypadID = 0x15; //the default ID
}

void POWERKEYPRO::begin(int speed, int id)
{
	if (!channel->isInitialized())
	{
		channel->setMasterMode();
		channel->begin(speed, id);
    
		channel->sendNodeReset(0); //reset all nodes
		delay(500);
		channel->sendNodeStart(0); //make all connected nodes go active
		delay(500);
	}
	keypadID = id;
}

bool POWERKEYPRO::isConnected()
{
}

void POWERKEYPRO::setKeypadID(int id)
{
	keypadID = id;
}

//Not actually in existence yet...
int POWERKEYPRO::findKeypadID()
{
	return -1;
}

void POWERKEYPRO::setKeyCallback(void (*cb)(int))
{
	keystateChange = cb;
}

bool POWERKEYPRO::getKeyState(int key)
{
	if (key < 0) return false;
	if (key > 11) return false;
	return buttonState[key];
}

void POWERKEYPRO::setLEDState(int LED, LEDTYPE newState)
{
	SDO_FRAME frame;	
	frame.nodeID = 0x600 + keypadID;
	frame.cmd = SDO_WRITE;
	frame.index = 0x6500;
	frame.subIndex = 1;
	frame.dataLength = 4;
	frame.data[0] = 1;
	frame.data[1] = LED;
	frame.data[2] = (int)newState;
	frame.data[3] = 0;
	
	channel->sendSDORequest(&frame);
}

LEDTYPE POWERKEYPRO::getLEDState(int LED)
{
	if (LED < 0) return OFF;
	if (LED > 11) return OFF;
	return LEDState[LED];
}

void POWERKEYPRO::setDeviceCANSpeed(int speed)
{
}

void POWERKEYPRO::loop()
{
	channel->loop();
}

void POWERKEYPRO::onPDOReceive(CAN_FRAME* frame)
{
	int outputValue;
	if (frame->id == (0x180 + keypadID)) //notification that key state has changed
	{		
		for (int bit = 0; bit < 12; bit++)
		{
			if (frame->data.byte[bit / 8] & (1 << (bit % 8)))
			{
				buttonState[bit] = true;
			}
			else
			{
				buttonState[bit] = false;
			}
		}
		if (keystateChange != NULL) 
		{
			outputValue = frame->data.byte[0] + (frame->data.byte[1] * 256);			
			keystateChange(outputValue); //call the callback function
		}
	}
}

void POWERKEYPRO::onSDORequest(SDO_FRAME* frame)
{
}

void POWERKEYPRO::onSDOResponse(SDO_FRAME* frame)
{
}

POWERKEYPRO PowerKeyPro0(0);
POWERKEYPRO PowerKeyPro1(1);
