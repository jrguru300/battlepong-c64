#include <c64.h>

#include "sound.h"

void sid_play_effect( char effectNumber) {
  int timeIndex;

  switch( effectNumber) {
    case SOUND_SHOT:
      for ( timeIndex = 15; timeIndex >= 0; timeIndex--) {
      	SID.amp = timeIndex;
      	SID.v1.freq = 10440;
      	SID.v1.pw = 129;
      	SID.v1.ctrl = 15;
      	SID.v1.ad = 0;
      }
      SID.amp = 0;
      SID.v1.pw = 0;
    break;
  }
}
