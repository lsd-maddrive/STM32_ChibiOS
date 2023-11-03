#include "ch.h"
#include "hal.h"

int main(void) {

  halInit();
  chSysInit();

  while (true)
  {
      palTogglePad(GPIOC, 13);
      chThdSleepMilliseconds(500);
  }
}
