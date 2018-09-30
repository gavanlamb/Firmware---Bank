/*
 * Copyright (c) 2014-2018 Cesanta Software Limited
 * All rights reserved
 *
 * Licensed under the Apache License, Version 2.0 (the ""License"");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an ""AS IS"" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "mgos.h"
#include "mgos_gpio.h"
#include "mgos_mqtt.h"
#include "mgos_sys_config.h"
#include <algorithm>
#include <vector>
#include <string>
#include <stdint.h>
#include <sstream>
#include <iostream>
using namespace std;

//SETUP SECTION
static vector<uint16_t> relays;
static uint8_t number_of_relays_to_control;

void setup_switches()
{
    istringstream string_relays(mgos_sys_config_get_bank_relay_pins());

    string relay;

    while (getline(string_relays, relay, ',')) {
        int relay_pin_number;
        istringstream convert(relay);
        convert >> relay_pin_number;
        mgos_gpio_set_mode(relay_pin_number, MGOS_GPIO_MODE_OUTPUT);
        relays.push_back(relay_pin_number);
    }

    number_of_relays_to_control = relays.size();
}

void setup()
{
    setup_switches();
}

enum mgos_app_init_result mgos_app_init(void)
{
    setup();
    return MGOS_APP_INIT_SUCCESS;
}