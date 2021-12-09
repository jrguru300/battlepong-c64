#include "sound.h"

void ball_sound (void)
{
	SID.v1.ad    = 0x58; 	// Attack/decay voice 1
	SID.v1.sr    = 0x59; 	// Sustain/release voice 1
	SID.amp      = 0x1F; 	// Volume
	SID.v1.pw	 = 0x0F0F; 	// Pulse width voice 1
	SID.v1.freq  = 0x224B;
	SID.v1.ctrl  = 0x41;	// Control voice 1
	SID.v1.ctrl  = 0x40;
}
