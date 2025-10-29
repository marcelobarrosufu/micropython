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

#if MICROPY_PY_IEEE80254

#include "py/obj.h"
#include "py/runtime.h"

static mp_obj_t ieee154_init(void) 
{
    return mp_const_none;
}

static mp_obj_t ieee154_deinit(void) 
{
    return mp_const_none;
}

static MP_DEFINE_CONST_FUN_OBJ_0(ieee154_init_obj, ieee154_init);
static MP_DEFINE_CONST_FUN_OBJ_0(ieee154_deinit_obj, ieee154_deinit);

static const mp_rom_map_elem_t ieee154_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_ieee154) },
    { MP_ROM_QSTR(MP_QSTR_init), MP_ROM_PTR(&ieee154_init_obj) },
    { MP_ROM_QSTR(MP_QSTR_deinit), MP_ROM_PTR(&ieee154_deinit_obj) },
};

static MP_DEFINE_CONST_DICT(ieee154_module_globals, ieee154_module_globals_table);

const mp_obj_module_t ieee154_user_cmodule = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&ieee154_module_globals,
};

// Registro no MicroPython
MP_REGISTER_MODULE(MP_QSTR_ieee154, ieee154_user_cmodule);


#endif // MICROPY_PY_IEEE80254
