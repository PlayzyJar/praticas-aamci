#include "serial_protocol.h"

ProtocolAction process_received_byte(uint8_t byte, uint8_t *ack_to_send)
{
    if (byte == CMD_USER_BTN)
    {
        if (ack_to_send != 0)
        {
            *ack_to_send = ACK_BYTE;
        }
        return ACTION_TOGGLE_LD1;
    }
    else if (byte == CMD_EXT_BTN)
    {
        if (ack_to_send != 0)
        {
            *ack_to_send = ACK_BYTE;
        }
        return ACTION_TOGGLE_LD2;
    }
    else if (byte == ACK_BYTE)
    {
        return ACTION_ACK_RECEIVED;
    }

    return ACTION_NONE;
}
