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

#include "hal.h"

#include "sam3x8e.h"

/* Clock settings (84MHz) */
#define SYS_BOARD_OSCOUNT       (CKGR_MOR_MOSCXTST(0x8))
#define SYS_BOARD_PLLAR         (CKGR_PLLAR_ONE | \
                                 CKGR_PLLAR_MULA(0xdUL) | \
                                 CKGR_PLLAR_PLLACOUNT(0x3fUL) | \
                                 CKGR_PLLAR_DIVA(0x1UL))
#define SYS_BOARD_MCKR          (PMC_MCKR_PRES_CLK_2 | PMC_MCKR_CSS_PLLA_CLK)

/* Clock Definitions */
#define SYS_UTMIPLL             (480000000UL)   /* UTMI PLL frequency */

#define SYS_CKGR_MOR_KEY_VALUE  CKGR_MOR_KEY(0x37) /* Key to unlock MOR register */

#if HAL_USE_PAL || defined(__DOXYGEN__)
/**
 * @brief   PAL setup.
 * @details Digital I/O ports static configuration as defined in @p board.h.
 *          This variable is used by the HAL when initializing the PAL driver.
 */
const PALConfig pal_default_config = {
};
#endif

/**
 * @brief   Early initialization code.
 * @details This initialization must be performed just after stack setup
 *          and before any other initialization.
 */
void __early_init(void) {
  /* Set FWS according to SYS_BOARD_MCKR configuration */
  EFC0->EEFC_FMR = EEFC_FMR_FWS(4);
  EFC1->EEFC_FMR = EEFC_FMR_FWS(4);

  /* Initialize main oscillator */
  if (!(PMC->CKGR_MOR & CKGR_MOR_MOSCSEL)) {
    PMC->CKGR_MOR = SYS_CKGR_MOR_KEY_VALUE |
                    SYS_BOARD_OSCOUNT |
                    CKGR_MOR_MOSCRCEN |
                    CKGR_MOR_MOSCXTEN;
    while (!(PMC->PMC_SR & PMC_SR_MOSCXTS)) {
    }
  }

  /* Switch to 3-20MHz Xtal oscillator */
  PMC->CKGR_MOR = SYS_CKGR_MOR_KEY_VALUE |
                  SYS_BOARD_OSCOUNT |
                  CKGR_MOR_MOSCRCEN |
                  CKGR_MOR_MOSCXTEN |
                  CKGR_MOR_MOSCSEL;

  while (!(PMC->PMC_SR & PMC_SR_MOSCSELS)) {
  }
  PMC->PMC_MCKR = (PMC->PMC_MCKR & ~(uint32_t)PMC_MCKR_CSS_Msk) |
                   PMC_MCKR_CSS_MAIN_CLK;
  while (!(PMC->PMC_SR & PMC_SR_MCKRDY)) {
  }

  /* Initialize PLLA */
  PMC->CKGR_PLLAR = SYS_BOARD_PLLAR;
  while (!(PMC->PMC_SR & PMC_SR_LOCKA)) {
  }

  /* Switch to main clock */
  PMC->PMC_MCKR = (SYS_BOARD_MCKR & ~PMC_MCKR_CSS_Msk) |
                   PMC_MCKR_CSS_MAIN_CLK;
  while (!(PMC->PMC_SR & PMC_SR_MCKRDY)) {
  }

  /* Switch to PLLA */
  PMC->PMC_MCKR = SYS_BOARD_MCKR;
  while (!(PMC->PMC_SR & PMC_SR_MCKRDY)) {
  }
}

/*
 * Board-specific initialization code.
 */
void boardInit(void) {
}
