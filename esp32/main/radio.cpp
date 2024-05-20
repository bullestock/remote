#include "defs.h"
#include "hw.h"
#include "protocol.h"
#include "radio.h"

#include <algorithm>
#include <stdint.h>

// Radio pipe addresses for the 2 nodes to communicate.
static const uint8_t pipes[][6] = { "1BULL", "2BULL" };

bool init_radio(NRF24_t& dev)
{
    Nrf24_init(&dev);
    const uint8_t payload_size = std::max(sizeof(ForwardAirFrame), sizeof(ReturnAirFrame));
    const uint8_t channel = CONFIG_RADIO_CHANNEL;

    Nrf24_SetOutputRF_PWR(&dev, 3); // 3 = 0 dBm (max)

    Nrf24_config(&dev, channel, payload_size);

    // Set my own address using 5 characters
    if (Nrf24_setRADDR(&dev, pipes[0]) != ESP_OK)
        return false;

    // Set destination address using 5 characters
    return Nrf24_setTADDR(&dev, pipes[1]) == ESP_OK;
}

bool send_frame(NRF24_t& dev,
                int64_t ticks,
                ForwardAirFrame& frame,
                bool& timeout)
{
    frame.magic = ForwardAirFrame::MAGIC_VALUE;
    frame.ticks = ticks;
    frame.left_x = 1023 - read_adc(LEFT_X_CHANNEL);
    frame.left_y = read_adc(LEFT_Y_CHANNEL);
    frame.right_x = 1023 - read_adc(RIGHT_X_CHANNEL);
    frame.right_y = read_adc(RIGHT_Y_CHANNEL);
    frame.left_pot = read_adc(POT1_CHANNEL);
    frame.right_pot = read_adc(POT2_CHANNEL);
    read_switches(frame);
    set_crc(frame);
    
#if 1
    printf("X %3d Y %3d X %3d Y %3d S %02X %02X %d P %02X %02X\n",
           frame.left_x, frame.left_y, frame.right_x, frame.right_y,
           frame.pushbuttons,
           frame.toggles,
           frame.slide,
           frame.left_pot, frame.right_pot);
#endif
    
    Nrf24_send(&dev, reinterpret_cast<uint8_t*>(&frame));

    return true;
}
