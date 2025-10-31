/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2025 Marcelo Barros de Almeida <marcelobarrosalmeida@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */


#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "esp_system.h"
#include "esp_log.h"
#include "esp_ieee802154.h"
#include "esp_console.h"
#include "esp_phy_init.h"
#include "soc/soc.h"
#include "soc/ieee802154_reg.h"

#include "py/obj.h"
#include "py/objstr.h"
#include "py/runtime.h"

#include "mpconfigport.h"

#include "py/obj.h"
#include "py/runtime.h"

#if MICROPY_PY_IEEE802154


typedef struct ieee802154_ctrl_s
{
    uint8_t channel;
    uint16_t panid;
    uint16_t short_addr;
    bool enabled;
    uint8_t seq_num;
    uint8_t tx[128];
} ieee802154_ctrl_t;
static const char *TAG = "mp_ieee802154";

static ieee802154_ctrl_t ieee802154_ctrl = 
{
    .channel = 11,
    .panid = 1000,
    .short_addr = 10000,
    .enabled = false,
    .seq_num = 0,
};

static void ieee802154_check_if_enabled(void)
{
    if (!ieee802154_ctrl.enabled) 
    {
        mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("IEEE 802.15.4 is not initialized"));
    }
}

static mp_obj_t ieee802154_init(void) 
{
    if(!ieee802154_ctrl.enabled) 
    {
        if(esp_ieee802154_enable() == ESP_OK)
        {
            ieee802154_ctrl.enabled = true;
        }
        else
        {
            mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("Failed to enable IEEE 802.15.4"));
        }
    }

    return mp_const_none;
}

static mp_obj_t ieee802154_deinit(void) 
{
    if (ieee802154_ctrl.enabled) 
    {
        if(esp_ieee802154_disable() == ESP_OK)
        {
            ieee802154_ctrl.enabled = false;
        }
        else
        {
            mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("Failed to disable IEEE 802.15.4"));
        }
    }

    return mp_const_none;
}

static mp_obj_t ieee802154_set_channel(mp_obj_t channel_obj) 
{
    ieee802154_check_if_enabled();

    mp_int_t channel = mp_obj_get_int(channel_obj);
    if (channel < 11 || channel > 26) 
    {
        mp_raise_msg(&mp_type_ValueError, MP_ERROR_TEXT("Channel must be between 11 and 26"));
    }

    esp_err_t ret = esp_ieee802154_set_channel((uint8_t)channel);
    if (ret == ESP_OK)
    {
        ieee802154_ctrl.channel = (uint8_t)channel;
    } 
    else
    {
        mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("Failed to set channel"));
    }

    return mp_const_none;
}

static mp_obj_t ieee802154_get_channel(void) 
{
    ieee802154_check_if_enabled();

    uint8_t channel = esp_ieee802154_get_channel();

    return mp_obj_new_int(channel);
}

static mp_obj_t ieee802154_set_panid(mp_obj_t panid_obj) 
{
    ieee802154_check_if_enabled();

    mp_int_t panid = mp_obj_get_int(panid_obj);
    if (panid < 0 || panid > 0xFFFF) 
    {
        mp_raise_msg(&mp_type_ValueError, MP_ERROR_TEXT("PANID must be between 0 and 65535"));
    }

    esp_err_t ret = esp_ieee802154_set_panid((uint16_t)panid);
    if (ret == ESP_OK) 
    {
        ieee802154_ctrl.panid = (uint16_t)panid;
    } 
    else
    {
        mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("Failed to set PANID"));
    }

    return mp_const_none;
}

static mp_obj_t ieee802154_get_panid(void) 
{
    ieee802154_check_if_enabled();

    uint16_t panid = esp_ieee802154_get_panid();

    return mp_obj_new_int(panid);
}

static mp_obj_t ieee802154_set_short_addr(mp_obj_t short_addr_obj) 
{
    ieee802154_check_if_enabled();

    mp_int_t short_addr = mp_obj_get_int(short_addr_obj);
    if (short_addr < 0 || short_addr > 0xFFFF) 
    {
        mp_raise_msg(&mp_type_ValueError, MP_ERROR_TEXT("Short address must be between 0 and 65535"));
    }

    esp_err_t ret = esp_ieee802154_set_short_address((uint16_t)short_addr);
    if (ret == ESP_OK) 
    {
        ieee802154_ctrl.short_addr = (uint16_t)short_addr;
    } 
    else
    {
        mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("Failed to set short address"));
    }

    return mp_const_none;
}

static mp_obj_t ieee802154_get_short_addr(void) 
{
    ieee802154_check_if_enabled();

    uint16_t short_addr = esp_ieee802154_get_short_address();

    return mp_obj_new_int(short_addr);
}

static uint8_t ieee802154_get_next_seq_number(bool retry)
{
    if (!retry) 
    {
        ieee802154_ctrl.seq_num++;
    }
    return ieee802154_ctrl.seq_num;
}

void esp_ieee802154_transmit_done(const uint8_t *frame, const uint8_t *ack, esp_ieee802154_frame_info_t *ack_frame_info)
{
    ESP_LOGI(TAG,"Transmit done callback called");
}

void esp_ieee802154_transmit_failed(const uint8_t *frame, esp_ieee802154_tx_error_t error)
{
    ESP_LOGI(TAG,"Transmit failed callback called");
}

static mp_obj_t ieee802154_send_msg(mp_obj_t payload_obj, mp_obj_t dst_addr_obj, mp_obj_t retry_obj)
{
    ieee802154_check_if_enabled();

    // retrieve payload buffer
    mp_buffer_info_t b_info;
    mp_get_buffer_raise(payload_obj, &b_info, MP_BUFFER_READ);
    const uint8_t *payload = (const uint8_t *)b_info.buf;
    size_t len = b_info.len;
    // retrieve destination address and timeout
    uint16_t dst_addr = (uint16_t)mp_obj_get_int(dst_addr_obj);
    bool retry = mp_obj_is_true(retry_obj);

    if(len == 0 || len > 116)
    {
        mp_raise_msg(&mp_type_ValueError, MP_ERROR_TEXT("Payload size must be between 1 and 116 bytes"));
    }

    // full size: FC(2)+SN(1)+DPANID(2)+DADDR(2)+SADDR(2) + PAYLOAD + FCS(2)
    ieee802154_ctrl.tx[0] = len + 9; // FC(2)+SN(1)+DPANID(2)+DADDR(2)+SADDR(2)
    ieee802154_ctrl.tx[1] = 0x61; // Frame Control: Data Frame, Ack Request, PAN ID Compression
    ieee802154_ctrl.tx[2] = 0x88; // Frame Control: Src and Dst Addr are short (16 bit)
    ieee802154_ctrl.tx[3] = ieee802154_get_next_seq_number(retry);
    ieee802154_ctrl.tx[4] = (uint8_t)(ieee802154_ctrl.panid & 0xFF);         // Dest PAN ID LSB
    ieee802154_ctrl.tx[5] = (uint8_t)(ieee802154_ctrl.panid >> 8);           // Dest PAN ID MSB
    ieee802154_ctrl.tx[6] = (uint8_t)(dst_addr & 0xFF);                      // Dest Address LSB
    ieee802154_ctrl.tx[7] = (uint8_t)(dst_addr >> 8);                        // Dest Address MSB
    ieee802154_ctrl.tx[8] = (uint8_t)(ieee802154_ctrl.short_addr & 0xFF); // Src Address LSB
    ieee802154_ctrl.tx[9] = (uint8_t)(ieee802154_ctrl.short_addr >> 8);   // Src Address MSB

    memcpy(&ieee802154_ctrl.tx[10], payload, len);

    if(esp_ieee802154_transmit(ieee802154_ctrl.tx, true) != ESP_OK)
    {
        mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("Failed to transmit message"));
    }

    return mp_const_none;
}

static MP_DEFINE_CONST_FUN_OBJ_0(ieee802154_init_obj, ieee802154_init);
static MP_DEFINE_CONST_FUN_OBJ_0(ieee802154_deinit_obj, ieee802154_deinit);
static MP_DEFINE_CONST_FUN_OBJ_1(ieee802154_set_channel_obj, ieee802154_set_channel);
static MP_DEFINE_CONST_FUN_OBJ_0(ieee802154_get_channel_obj, ieee802154_get_channel);
static MP_DEFINE_CONST_FUN_OBJ_1(ieee802154_set_panid_obj, ieee802154_set_panid);
static MP_DEFINE_CONST_FUN_OBJ_0(ieee802154_get_panid_obj, ieee802154_get_panid);
static MP_DEFINE_CONST_FUN_OBJ_1(ieee802154_set_short_addr_obj, ieee802154_set_short_addr);
static MP_DEFINE_CONST_FUN_OBJ_0(ieee802154_get_short_addr_obj, ieee802154_get_short_addr);
static MP_DEFINE_CONST_FUN_OBJ_3(ieee802154_send_msg_obj, ieee802154_send_msg);

static const mp_rom_map_elem_t ieee802154_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_ieee154) },
    { MP_ROM_QSTR(MP_QSTR_init), MP_ROM_PTR(&ieee802154_init_obj) },
    { MP_ROM_QSTR(MP_QSTR_deinit), MP_ROM_PTR(&ieee802154_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_channel), MP_ROM_PTR(&ieee802154_set_channel_obj) },
    { MP_ROM_QSTR(MP_QSTR_get_channel), MP_ROM_PTR(&ieee802154_get_channel_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_panid), MP_ROM_PTR(&ieee802154_set_panid_obj) },
    { MP_ROM_QSTR(MP_QSTR_get_panid), MP_ROM_PTR(&ieee802154_get_panid_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_short_addr), MP_ROM_PTR(&ieee802154_set_short_addr_obj) },
    { MP_ROM_QSTR(MP_QSTR_get_short_addr), MP_ROM_PTR(&ieee802154_get_short_addr_obj) },
    { MP_ROM_QSTR(MP_QSTR_send_msg), MP_ROM_PTR(&ieee802154_send_msg_obj) },
};

static MP_DEFINE_CONST_DICT(ieee802154_module_globals, ieee802154_module_globals_table);

const mp_obj_module_t ieee802154_user_cmodule = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&ieee802154_module_globals,
};

// Registro no MicroPython
MP_REGISTER_MODULE(MP_QSTR_ieee154, ieee802154_user_cmodule);


#endif // MICROPY_PY_IEEE802154
