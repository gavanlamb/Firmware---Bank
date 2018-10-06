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
#include "Relay.h"
#include <string>
#include <sstream>
using namespace std;

int string_to_int(string string_number)
{
    int number;
    istringstream convert(string_number);
    convert >> number;
    return number;
}

static void relay_off_cb(void *relay) {
    Relay *r = (Relay *)relay;
    mgos_gpio_write(r->pin, false);
    mgos_clear_timer(r->timer);
}

/*
 * Handler to turn on relay
 */
void relay_on_handler(
    struct mg_connection *c,
    const char *topic,
    int topic_len,
    const char *msg,
    int msg_len,
    void *relay)
{
    (void)c;
    (void)topic;
    (void)topic_len;

    Relay *r = (Relay *)relay;
    mgos_gpio_write(r->pin, true);
    mgos_clear_timer(r->timer);

    if(msg_len > 0)
    {
        int length = string_to_int(msg);
        if(length > 200)
        {
            r->timer = mgos_set_timer(length,MGOS_TIMER_REPEAT,relay_off_cb,relay);
        }
    }
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
    void *relay)
{
    (void)c;
    (void)topic;
    (void)topic_len;
    (void)msg;
    (void)msg_len;
    Relay *r = (Relay *)relay;
    mgos_gpio_write(r->pin, false);
}

/*
 * Subscribes to on and off topics for the particular pin.
 */
void subscribe_to_topics(Relay *relay)
{
    //on topic
    stringstream ss;
    ss << "devices/switch/on/" << relay->alias;
    mgos_mqtt_sub(ss.str().c_str(), relay_on_handler, (void*)relay);

    //off topic
    ss.str("");
    ss << "devices/switch/off/" << relay->alias;
    mgos_mqtt_sub(ss.str().c_str(), relay_off_handler, (void*)relay);
}



/*
 * Sets pin to output mode and turns it off.
 */
void set_output_pin(uint16_t pin)
{
    mgos_gpio_set_mode(pin, MGOS_GPIO_MODE_OUTPUT);
    mgos_gpio_write(pin, false);
}

/*
 * Main method called by mongoose os.
 * Sets the pins up.
 */
enum mgos_app_init_result mgos_app_init(void)
{
    istringstream string_relays(mgos_sys_config_get_bank_relay_pins());
    string relay_pin_number;
    for (uint16_t tracker = 1; getline(string_relays, relay_pin_number, ','); tracker++)
    {
        Relay *relay = new Relay();
        relay->pin = string_to_int(relay_pin_number);
        relay->alias = tracker;

        set_output_pin(relay->pin);
        subscribe_to_topics(relay);
    }

    return MGOS_APP_INIT_SUCCESS;
}