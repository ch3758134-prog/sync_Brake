#ifndef BLE_SERIAL_H
#define BLE_SERIAL_H
#include <stdbool.h>
#include <stdint.h>
#include "bs_status.h"
typedef enum {
    BLE_LINK_UNKNOWN = 0, BLE_LINK_DISCONNECTED,
    BLE_LINK_CONNECTED, BLE_LINK_FAULT
} ble_link_state_t;
typedef struct {
    ble_link_state_t link;
    uint32_t last_update_ms;
    uint32_t protocol_errors;
} ble_serial_status_t;
typedef struct {
    uint16_t size;
    uint8_t *buffer; /* 由调用方提供，非 BLE 设备层分配 */
    uint32_t received_at_ms;
} ble_serial_message_t;
/* 实际 WLT8016 协议未审核前，不能把下面接口称为已经支持 AT/GATT。 */
bs_status_t ble_serial_init(void);
bs_status_t ble_serial_process(uint32_t now_ms);
bs_status_t ble_serial_take_message(ble_serial_message_t *message,
                                    uint16_t capacity);
bs_status_t ble_serial_get_status(ble_serial_status_t *status);
#endif
