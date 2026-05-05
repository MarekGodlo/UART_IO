/**
* @file uart_io.h
*
* Created on 26.04.2026.
* @author Marek Godlowicz https://github.com/MarekGodlo
* @brief This file provides non-blocking UART Tx management via DMA and ring buffer.
*
* @note This module enables redirection of printf output.
*/

#ifndef UART_IO_H
#define UART_IO_H
#include "main.h"

#define UART_IO_BUFFER_SIZE 256

/**
 * @brief Ring Buffer structure
 */
typedef struct {
    char *buffer;   /*!< Pointer to the data buffer */
    uint16_t size;  /*!< Buffer length */
    uint16_t head;  /*!< Buffer writing index */
    uint16_t tail;  /*!< Buffer reading index */
} RingBuffer_t;

/**
 * @brief UART IO Module configuration structure
 */
typedef struct {
    UART_HandleTypeDef *huart;  /*!< Pointer to the UART HAL handle */
    IRQn_Type UART_IRQn;        /*!< UART interrupt number (IRQ) */
} UART_IO_HandleTypeDef;

/**
 * @brief Initializes UART IO module
 * @param huartio Pointer to the configuration structure
 */
void UART_IO_Init(UART_IO_HandleTypeDef *huartio);

/**
 * @brief Sends buffer data into transmission.
 * @warning This function must be called by UART DMA tx complete callback.
 */
void UART_IO_TransferCallback(void);

/**
 * @brief Writes data into the buffer.
 * @param str Pointer to the character array (aka string), data to be written.
 * @param len Number of characters to write.
 *
 * @note This function is supposed to be called by the _write() function (C standard library) to enable the printf() output redirection.
 */
void UART_IO_Write(const char *str, uint16_t len);

#endif // UART_IO_H