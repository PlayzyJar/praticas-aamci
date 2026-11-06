#ifndef SERIAL_PROTOCOL_H_
#define SERIAL_PROTOCOL_H_

#include <stdint.h>

#define CMD_USER_BTN  0xAA
#define CMD_EXT_BTN   0xBB
#define ACK_BYTE      0xCC

typedef enum {
    ACTION_NONE = 0,
    ACTION_TOGGLE_LD1,
    ACTION_TOGGLE_LD2,
    ACTION_ACK_RECEIVED
} ProtocolAction;

/**
 * @brief Processa um byte recebido pela USART2.
 * @param byte: Byte recebido
 * @param ack_to_send: Ponteiro para armazenar o byte de ACK se necessário responder
 * @return Ação a ser executada
 */
ProtocolAction process_received_byte(uint8_t byte, uint8_t *ack_to_send);

#endif /* SERIAL_PROTOCOL_H_ */
