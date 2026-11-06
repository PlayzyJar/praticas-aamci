#include <stdio.h>
#include <assert.h>
#include "../Core/Inc/serial_protocol.h"

void test_process_received_byte_user_cmd(void)
{
    uint8_t ack = 0;
    ProtocolAction action = process_received_byte(CMD_USER_BTN, &ack);
    assert(action == ACTION_TOGGLE_LD1);
    assert(ack == ACK_BYTE);
    printf("test_process_received_byte_user_cmd: PASSED\n");
}

void test_process_received_byte_ext_cmd(void)
{
    uint8_t ack = 0;
    ProtocolAction action = process_received_byte(CMD_EXT_BTN, &ack);
    assert(action == ACTION_TOGGLE_LD2);
    assert(ack == ACK_BYTE);
    printf("test_process_received_byte_ext_cmd: PASSED\n");
}

void test_process_received_byte_ack(void)
{
    uint8_t ack = 0;
    ProtocolAction action = process_received_byte(ACK_BYTE, &ack);
    assert(action == ACTION_ACK_RECEIVED);
    printf("test_process_received_byte_ack: PASSED\n");
}

void test_process_received_byte_invalid(void)
{
    uint8_t ack = 0;
    ProtocolAction action = process_received_byte(0x55, &ack);
    assert(action == ACTION_NONE);
    printf("test_process_received_byte_invalid: PASSED\n");
}

void test_process_received_byte_null_ptr(void)
{
    ProtocolAction action = process_received_byte(CMD_USER_BTN, NULL);
    assert(action == ACTION_TOGGLE_LD1);
    printf("test_process_received_byte_null_ptr: PASSED\n");
}

int main(void)
{
    test_process_received_byte_user_cmd();
    test_process_received_byte_ext_cmd();
    test_process_received_byte_ack();
    test_process_received_byte_invalid();
    test_process_received_byte_null_ptr();
    printf("ALL TESTS PASSED!\n");
    return 0;
}
