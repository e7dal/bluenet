/**
 * Author: Crownstone Team
 * Copyright: Crownstone (https://crownstone.rocks)
 * Date: 10 May., 2019
 * License: LGPLv3+, Apache License 2.0, and/or MIT (triple-licensed)
 */

#pragma once

#include <cstdint>
#include <cstddef>

typedef uint8_t* buffer_ptr_t;
typedef uint16_t cs_buffer_size_t;
typedef uint16_t cs_ret_code_t;
typedef uint16_t cs_control_cmd_t;
typedef uint8_t  stone_id_t;
typedef uint16_t device_id_t;
typedef uint16_t size16_t;
//! Boolean with fixed size.
typedef uint8_t BOOL;
typedef uint8_t cs_state_id_t;

// Actually wanted something like: typedef uint24_t cs_tracked_device_token_t;
#define TRACKED_DEVICE_TOKEN_SIZE 3

typedef struct {
  uint8_t uuid128[16];
} cs_uuid128_t;
