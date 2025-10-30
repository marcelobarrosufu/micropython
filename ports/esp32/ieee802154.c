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

#if MICROPY_PY_IEEE802154

#include "py/obj.h"
#include "py/runtime.h"

typedef struct ieee802154_config_s
{
    uint8_t channel;
    uint16_t panid;
    uint16_t short_address;
    bool enabled;
} ieee802154_config_t;

static ieee802154_config_t ieee802154_config = 
{
    .channel = 11,
    .panid = 1000,
    .short_address = 10000,
    .enabled = false,
};

static void ieee802154_check_if_enabled(void)
{
    if (!ieee802154_config.enabled) 
    {
        mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("IEEE 802.15.4 is not initialized"));
    }
}

static mp_obj_t ieee802154_init(void) 
{
    if(!ieee802154_config.enabled) 
    {
        if(esp_ieee802154_enable() == ESP_OK)
        {
            ieee802154_config.enabled = true;
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
    if (ieee802154_config.enabled) 
    {
        if(esp_ieee802154_disable() == ESP_OK)
        {
            ieee802154_config.enabled = false;
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
        ieee802154_config.channel = (uint8_t)channel;
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
        ieee802154_config.panid = (uint16_t)panid;
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
        ieee802154_config.short_address = (uint16_t)short_addr;
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

static mp_obj_t ieee802154_send_msg(mp_obj_t payload_obj, mp_obj_t dst_addr, mp_obj_t timeout_ms)
{
    ieee802154_check_if_enabled();

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
};

static MP_DEFINE_CONST_DICT(ieee802154_module_globals, ieee802154_module_globals_table);

const mp_obj_module_t ieee802154_user_cmodule = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&ieee802154_module_globals,
};

// Registro no MicroPython
MP_REGISTER_MODULE(MP_QSTR_ieee154, ieee802154_user_cmodule);


#endif // MICROPY_PY_IEEE802154
