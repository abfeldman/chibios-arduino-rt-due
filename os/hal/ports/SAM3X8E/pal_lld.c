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

/**
 * @file    pal_lld.c
 * @brief   PLATFORM PAL subsystem low level driver source.
 *
 * @addtogroup PAL
 * @{
 */

#include "hal.h"

#include "sam3x8e.h"

#if (HAL_USE_PAL == TRUE) || defined(__DOXYGEN__)

/*===========================================================================*/
/* Driver local definitions.                                                 */
/*===========================================================================*/

/*===========================================================================*/
/* Driver exported variables.                                                */
/*===========================================================================*/

/*===========================================================================*/
/* Driver local variables and types.                                         */
/*===========================================================================*/

/*===========================================================================*/
/* Driver local functions.                                                   */
/*===========================================================================*/

/*===========================================================================*/
/* Driver interrupt handlers.                                                */
/*===========================================================================*/

/*===========================================================================*/
/* Driver exported functions.                                                */
/*===========================================================================*/

/**
 * @brief   STM32 I/O ports configuration.
 * @details Ports A-D(E, F, G, H) clocks enabled.
 *
 * @param[in] config    the STM32 ports configuration
 *
 * @notapi
 */
void _pal_lld_init(const PALConfig *config) {
  (void)config;

  // Enable direct write to ODSR
  PIOA->PIO_OWER = 0xffffffff;
  PIOB->PIO_OWER = 0xffffffff;
  PIOC->PIO_OWER = 0xffffffff;
  PIOD->PIO_OWER = 0xffffffff;
}

/**
 * @brief   Pads mode setup.
 * @details This function programs a pads group belonging to the same port
 *          with the specified mode.
 *
 * @param[in] port      the port identifier
 * @param[in] mask      the group mask
 * @param[in] mode      the mode
 *
 * @notapi
 */
void _pal_lld_setgroupmode(ioportid_t port,
                           ioportmask_t mask,
                           iomode_t mode) {

  // Enable PIO function
  (port)->PIO_PER = mask;

  switch (mode)
      {
      case PAL_MODE_RESET:
      case PAL_MODE_UNCONNECTED:
      case PAL_MODE_INPUT_PULLUP:
          (port)->PIO_ODR = mask;       // Output disabled
          (port)->PIO_PUER = mask;	    // Pull-up enabled
          (port)->PIO_MDER = mask;      // Open-drain enabled
          break;

      case PAL_MODE_INPUT_ANALOG:
      case PAL_MODE_INPUT:
          (port)->PIO_ODR = mask;		// Output disabled
          (port)->PIO_PUDR = mask;	    // Pull-up disabled
          (port)->PIO_MDDR = mask;      // Open-drain disabled
          break;

      case PAL_MODE_OUTPUT_PUSHPULL:
          (port)->PIO_OER = mask;		// Output enabled
          (port)->PIO_PUDR = mask;	    // Pull-up disabled
          (port)->PIO_MDDR = mask;      // Open-drain disabled
          break;

      case PAL_MODE_OUTPUT_OPENDRAIN:
          (port)->PIO_OER = mask;		// Output enabled
          (port)->PIO_PUER = mask;	    // Pull-up enabled
          (port)->PIO_MDER = mask;      // Open-drain enabled
          break;
      }
}

#endif /* HAL_USE_PAL == TRUE */

/** @} */
