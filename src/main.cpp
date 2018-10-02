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
#include <string>
#include <sstream>
using namespace std;

/*
 * Handler to turn on relay
 */
void relay_on_handler(
    struct mg_connection *c,
    const char *topic,
    int topic_len,
    const char *msg,
    int msg_len,
    void *pin)
{
    (void)c;
    (void)topic;
    (void)topic_len;
    (void)msg;
    (void)msg_len;
    int x = *(int *)pin;
    mgos_gpio_write(x, true);
}

/*
 * Handler to turn off relay
 */
void relay_off_handler(
    struct mg_connection *c,
    const char *topic,
    int topic_len,
    const char *msg,
    int msg_len,
    void *pin)
{
    (void)c;
    (void)topic;
    (void)topic_len;
    (void)msg;
    (void)msg_len;
    int x = *(int *)pin;
    mgos_gpio_write(x, false);
}

/*
 * Subscribes to on and off topics for the particular pin.
 */
void subscribe_to_topics(int *alias, int *pin)
{
    //on topic
    stringstream ss;
    ss << "devices/switch/on/" << *alias;
    mgos_mqtt_sub(ss.str().c_str(), relay_on_handler, (void*)pin);

    //off topic
    ss.str("");
    ss << "devices/switch/off/" << *alias;
    mgos_mqtt_sub(ss.str().c_str(), relay_off_handler, (void*)pin);

}

/*
 * Gets the output pin from the string parameter.
 */
uint16_t get_output_pin(string relay_pin_number)
{
    uint16_t pin;
    istringstream convert(relay_pin_number);
    convert >> pin;
    return pin;
}

/*
 * Sets pin to output mode and turns it off.
 */
void set_output_pin(int *pin)
{
    mgos_gpio_set_mode(*pin, MGOS_GPIO_MODE_OUTPUT);
    mgos_gpio_write(*pin, false);
}

/*
 * Main method called by mongoose os.
 * Sets the pins up.
 */
enum mgos_app_init_result mgos_app_init(void)
{
    istringstream string_relays(mgos_sys_config_get_bank_relay_pins());
    string relay_pin_number;
    for (int tracker=1; getline(string_relays, relay_pin_number, ','); tracker++)
    {
        int *pin = new int(get_output_pin(relay_pin_number));
        int *alias = new int(tracker);
        set_output_pin(pin);
        subscribe_to_topics(alias, pin);
    }

    return MGOS_APP_INIT_SUCCESS;
}