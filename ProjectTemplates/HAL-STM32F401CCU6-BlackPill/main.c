#include "hal.h"

int main(void) {

  halInit();
  osalSysEnable();

  while (true)
  {
      palTogglePad(GPIOC, 13);
      osalThreadSleepMilliseconds(100);
  }
}
