#include "radio.h"

#include <stdint.h>

using namespace std;

// Radio pipe addresses for the 2 nodes to communicate.
static const uint8_t pipes[][6] = { "1BULL", "2BULL" };

bool init_radio(NRF24_t& dev)
{
    Nrf24_init(&dev);
    uint8_t payload = 32;
    uint8_t channel = CONFIG_RADIO_CHANNEL;
    Nrf24_config(&dev, channel, payload);

    // Set my own address using 5 characters
    if (Nrf24_setRADDR(&dev, pipes[0]) != ESP_OK)
        return false;

    // Set destination address using 5 characters
    return Nrf24_setTADDR(&dev, pipes[1]) == ESP_OK;
}
