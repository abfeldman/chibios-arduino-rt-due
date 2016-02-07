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
 * @file    serial_lld.c
 * @brief   SAM3X8E serial subsystem low level driver source.
 *
 * @addtogroup SERIAL
 * @{
 */

#include "hal.h"

#if (HAL_USE_SERIAL == TRUE) || defined(__DOXYGEN__)

/*===========================================================================*/
/* Driver local definitions.                                                 */
/*===========================================================================*/

/*===========================================================================*/
/* Driver exported variables.                                                */
/*===========================================================================*/

/** @brief UART1 serial driver identifier.*/
#if (SAM3X8E_SERIAL_USE_UART1 == TRUE) || defined(__DOXYGEN__)
SerialDriver SD1;
#endif

/*===========================================================================*/
/* Driver local variables and types.                                         */
/*===========================================================================*/

/**
 * @brief   Driver default configuration.
 */
static const SerialConfig default_config = {
  38400
};

/*===========================================================================*/
/* Driver local functions.                                                   */
/*===========================================================================*/

/*===========================================================================*/
/* Driver interrupt handlers.                                                */
/*===========================================================================*/

/*===========================================================================*/
/* Driver exported functions.                                                */
/*===========================================================================*/

OSAL_IRQ_HANDLER(UART_Handler) {
  OSAL_IRQ_PROLOGUE();

  uint16_t sr = UART->UART_SR;

  /* Data available.*/
  if ((UART->UART_IMR & UART_IMR_RXRDY) && (sr & UART_SR_RXRDY)) {
    osalSysLockFromISR();
    sdIncomingDataI(&SD1, UART->UART_RHR);
    osalSysUnlockFromISR();
  }

  /* Transmission buffer empty.*/
  if ((UART->UART_IMR & UART_IER_TXRDY) && (sr & UART_SR_TXRDY)) {
    msg_t b;
    osalSysLockFromISR();
    b = oqGetI(&SD1.oqueue);
    if (b < Q_OK) {
      chnAddFlagsI(&SD1, CHN_OUTPUT_EMPTY);
      UART->UART_IDR |= UART_IER_TXRDY;
      UART->UART_IER |= UART_IER_ENDTX;
    }
    else
        UART->UART_THR = b;
    osalSysUnlockFromISR();
  }
  /* Physical transmission end.*/
  if ((UART->UART_IMR & UART_IER_ENDTX) && (sr & UART_SR_ENDTX)) {
    osalSysLockFromISR();
    if (oqIsEmptyI(&SD1.oqueue))
      chnAddFlagsI(&SD1, CHN_TRANSMISSION_END);
    UART->UART_IDR |= UART_IER_ENDTX;
    osalSysUnlockFromISR();
  }

  OSAL_IRQ_EPILOGUE();
}

#if SAM3X8E_SERIAL_USE_UART1 || defined(__DOXYGEN__)
static void notify1(io_queue_t *qp) {
  (void)qp;
  UART->UART_IER |= UART_IER_TXRDY;
}
#endif

/**
 * @brief   Low level serial driver initialization.
 *
 * @notapi
 */
void sd_lld_init(void) {
#if SAM3X8E_SERIAL_USE_UART1 == TRUE
  sdObjectInit(&SD1, NULL, notify1);
#endif

  uint32_t ul_sr;

  // Disable interrupts on Rx and Tx
  PIOA->PIO_IDR = PIO_PA8A_URXD | PIO_PA9A_UTXD;

  // Disable the PIO of the Rx and Tx pins so that the peripheral controller can use them
  PIOA->PIO_PDR = PIO_PA8A_URXD | PIO_PA9A_UTXD;

  // Read current peripheral AB select register and set the Rx and Tx pins to 0 (Peripheral A function)
  ul_sr = PIOA->PIO_ABSR;
  PIOA->PIO_ABSR &= ~(PIO_PA8A_URXD | PIO_PA9A_UTXD) & ul_sr;
 
  // Enable the pull up on the Rx and Tx pin
  PIOA->PIO_PUER = PIO_PA8A_URXD | PIO_PA9A_UTXD;
 
  // ==> Actual uart configuration
  // Enable the peripheral uart controller
  PMC->PMC_PCER0 = 1 << ID_UART;
 
  // Reset and disable receiver & transmitter
  UART->UART_CR = UART_CR_RSTRX | UART_CR_RSTTX | UART_CR_RXDIS | UART_CR_TXDIS;
 
  // Set the baudrate to 115200
  UART->UART_BRGR = 45; // 84000000 / 16 * x = BaudRate (write x into UART_BRGR)
 
  // No Parity
  UART->UART_MR = UART_MR_PAR_NO;
 
  // Disable PDC channel requests
  UART->UART_PTCR = UART_PTCR_RXTDIS | UART_PTCR_TXTDIS;

  // Disable / Enable interrupts on end of receive
  UART->UART_IDR = 0xFFFFFFFF;
  NVIC_EnableIRQ((IRQn_Type)ID_UART);
  UART->UART_IER = UART_IER_RXRDY;
 
  // Enable receiver and transmitter
  UART->UART_CR = UART_CR_RXEN | UART_CR_TXEN;
}

/**
 * @brief   Low level serial driver configuration and (re)start.
 *
 * @param[in] sdp       pointer to a @p SerialDriver object
 * @param[in] config    the architecture-dependent serial driver configuration.
 *                      If this parameter is set to @p NULL then a default
 *                      configuration is used.
 *
 * @notapi
 */
void sd_lld_start(SerialDriver *sdp, const SerialConfig *config) {

  if (config == NULL) {
    config = &default_config;
  }

  if (sdp->state == SD_STOP) {
#if SAM3X8E_SERIAL_USE_UART1 == TRUE
    if (&SD1 == sdp) {

    }
#endif
  }
  /* Configures the peripheral.*/
}

/**
 * @brief   Low level serial driver stop.
 * @details De-initializes the UART, stops the associated clock, resets the
 *          interrupt vector.
 *
 * @param[in] sdp       pointer to a @p SerialDriver object
 *
 * @notapi
 */
void sd_lld_stop(SerialDriver *sdp) {

  if (sdp->state == SD_READY) {
#if SAM3X8E_SERIAL_USE_UART1 == TRUE
    if (&SD1 == sdp) {

    }
#endif
  }
}

#endif /* HAL_USE_SERIAL == TRUE */

/** @} */
