/**
* @file uart_io.c
*
* Created on 26.04.2026.
* @author Marek Godlowicz https://github.com/MarekGodlo
* @brief
*/

#include "uart_io.h"
#include <stdbool.h>

static bool buffer_send(const UART_IO_HandleTypeDef *huartio, const RingBuffer_t *buffer);
static bool buffer_write(RingBuffer_t *buffer, const char *data, size_t len);

static UART_IO_HandleTypeDef *p_handler;
static bool is_busy;
static uint16_t last_buffer_length;

static char raw_buffer[UART_IO_BUFFER_SIZE] ;
static RingBuffer_t UART_IO_BUFFER = {
    raw_buffer,
    UART_IO_BUFFER_SIZE
};

void UART_IO_Init(UART_IO_HandleTypeDef *huartio) {
    p_handler = huartio;
}

void UART_IO_TransferCallback(void) {
    is_busy = false;

    UART_IO_BUFFER.tail = (UART_IO_BUFFER.tail + last_buffer_length) % UART_IO_BUFFER.size;
    buffer_send(p_handler, &UART_IO_BUFFER);
}

void UART_IO_Write(const char *str, const uint16_t len) {
    HAL_NVIC_DisableIRQ(p_handler->UART_IRQn);

    if (buffer_write(&UART_IO_BUFFER, str, len)) {
        if (!is_busy) {
            buffer_send(p_handler, &UART_IO_BUFFER);
        }
    }

    HAL_NVIC_EnableIRQ(p_handler->UART_IRQn);
}

static bool buffer_send(const UART_IO_HandleTypeDef *huartio, const RingBuffer_t *buffer) {
    const uint16_t head = buffer->head;
    const uint16_t tail = buffer->tail;

    if (is_busy || head == tail) return false;

    int len;
    if (head > tail) {
        len = head - tail;
    } else {
        len = buffer->size - tail;
    }

    last_buffer_length = len;
    is_busy = true;
    HAL_UART_Transmit_DMA(huartio->huart, (uint8_t*) &buffer->buffer[buffer->tail], len);

    return true;
}

static uint16_t buffer_get_free_space(const RingBuffer_t *buffer) {
    if (buffer->head >= buffer->tail) {
        return buffer->size - (1 + buffer->head - buffer->tail);
    }

    return buffer->tail - buffer->head - 1;
}

static bool buffer_write(RingBuffer_t *buffer, const char *data, const size_t len) {
    const uint16_t free_space = buffer_get_free_space(buffer);

    if (free_space < len) return false;

    for (size_t i = 0; i < len; i++) {
        const int next_index = (buffer->head + 1) % buffer->size;

        if (next_index == buffer->tail) return false;

        buffer->buffer[buffer->head] = data[i];
        buffer->head = next_index;
    }

    return true;
}