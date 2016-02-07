/*
    ChibiOS - Copyright (C) 2006..2015 Giovanni Di Sirio

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
*/

#include "ch.h"
#include "hal.h"
#include "test.h"
#include "chprintf.h"

static THD_WORKING_AREA(waThread1, 256);

static THD_FUNCTION(Thread1, arg) {
  (void)arg;
  chRegSetThreadName("Blinker");
  while (true) {
    chprintf((BaseSequentialStream *)&SD1, "%u\r\n", BOARD_MCK);
    palTogglePad(IOPORT2, 27);

    chThdSleepMilliseconds(1000);
  }
}

int main(void) {
  halInit();
  chSysInit();

  sdStart(&SD1, NULL);

  palSetPadMode(IOPORT2, 27, PAL_MODE_OUTPUT_PUSHPULL);
  palTogglePad(IOPORT2, 27);

  /*
   * Starts the LED blinker thread.
   */
  chThdCreateStatic(waThread1, sizeof(waThread1), NORMALPRIO, Thread1, NULL);

  uint8_t buf[] = "Hello, World!\r\n";

  while(TRUE) {
    uint8_t c = sdGet(&SD1);

    sdWrite(&SD1, buf, 15);
    sdPut(&SD1, c);
    sdPut(&SD1, '\r');
    sdPut(&SD1, '\n');
  }
}
