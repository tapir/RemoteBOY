#ifndef BUTTON_H
#define BUTTON_H

#include <stdint.h>

class Button {
public:
	Button(void);
	void setup(int buttonID, uint16_t stateMask, void (*stateChangeCallback)(int, bool), int (*readPinCallback)(int));
	void loop(void);
	bool isPressed(void);

private:
	int		 buttonID;
	bool	 state;
	uint16_t samples;
	uint16_t stateMask;
	void (*stateChangeCallback)(int buttonID, bool state);
	int (*readPinCallback)(int buttonID);
};

#endif // BUTTON_H