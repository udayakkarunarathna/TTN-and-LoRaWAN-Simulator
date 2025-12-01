#include "mbed.h"
#include "mbed_trace.h"
#include "mbed_events.h"
#include "LoRaWANInterface.h"
#include "Sht31.h"
#include "SX1276_LoRaRadio.h"

// ---------------------------------------------------------
// TTN DEVICE CREDENTIALS (OTAA)
// ---------------------------------------------------------
static uint8_t DEV_EUI[] = { 0x70, 0xB3, 0xD5, 0x7E, 0xD0, 0x07, 0x46, 0x72 };
static uint8_t APP_EUI[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
static uint8_t APP_KEY[] = { 
    0xD5, 0x14, 0x0E, 0x15,
    0x3A, 0xE4, 0xD4, 0xDA,
    0xF0, 0x73, 0x77, 0x86,
    0x9F, 0xDB, 0x91, 0x37
};

#define LORA_APP_PORT 15

// ---------------------------------------------------------
// Hardware
// ---------------------------------------------------------
SX1276_LoRaRadio radio(
    D11, D12, D13, D10,
    A0, D2, D3, D4, D5, D8, D9,
    NC, NC, NC, NC, A4, NC, NC
);

Sht31 sht31(I2C_SDA, I2C_SCL);
InterruptIn btn(BUTTON1);

// LED output on p5
PwmOut led(p5);

// LED brightness slider (you must add slider → p20 in UI)
AnalogIn led_slider(p20);

// ---------------------------------------------------------
static EventQueue ev_queue;
static LoRaWANInterface lorawan(radio);
static lorawan_app_callbacks_t callbacks;

// ---------------------------------------------------------
// SEND MESSAGE
// ---------------------------------------------------------
static void send_message() 
{
    uint8_t tx_buffer[50] = { 0 };

    float temp = sht31.readTemperature();
    float hum  = sht31.readHumidity();
    float led_value = led_slider.read();

    // Update real LED brightness
    led.write(led_value);

    int len = sprintf(
        (char*)tx_buffer,
        "Temp:%.1f, Hum: %.1f, LED: %.2f",
        temp, hum, led_value
    );

    printf("Sending %d bytes: \"%s\"\n", len, tx_buffer);

    int16_t ret = lorawan.send(
        LORA_APP_PORT,
        tx_buffer,
        len,
        MSG_UNCONFIRMED_FLAG
    );

    if (ret < 0)
        printf("send() failed: %d\n", ret);
    else
        printf("%d bytes scheduled for transmission\n", ret);
}

// ---------------------------------------------------------
// RECEIVE DOWNLINK MESSAGE
// ---------------------------------------------------------
static void receive_message()
{
    uint8_t rx_buffer[50] = { 0 };

    int16_t ret = lorawan.receive(
        LORA_APP_PORT,
        rx_buffer,
        sizeof(rx_buffer),
        MSG_CONFIRMED_FLAG | MSG_UNCONFIRMED_FLAG
    );

    if (ret < 0) {
        printf("receive() - Error code %d\n", ret);
        return;
    }

    // EXACT OUTPUT FORMAT YOU REQUESTED:
    printf("Data received on port %d (length %d): ", LORA_APP_PORT, ret);

    for (int i = 0; i < ret; i++)
        printf("%02X ", rx_buffer[i]);

    printf("\n");

    // Handle brightness override
    if (ret >= 1) {
        int val = rx_buffer[0];       // byte 0
        float brightness = val / 100.0f;
        led.write(brightness);
        printf("LED is now %.2f\n", brightness);
    }
}

// ---------------------------------------------------------
// EVENT HANDLER
// ---------------------------------------------------------
static void lora_event_handler(lorawan_event_t event)
{
    switch (event)
    {
        case CONNECTED:
            printf("Connection - Successful\n");
            break;

        case TX_DONE:
            printf("Message Sent to Network Server\n");
            break;

        case RX_DONE:
            printf("Received message from Network Server\n");
            receive_message();
            break;

        case JOIN_FAILURE:
            printf("OTAA Failed - Check Keys\n");
            break;

        default:
            printf("LoRa event: %d\n", event);
            break;
    }
}

// ---------------------------------------------------------
// MAIN
// ---------------------------------------------------------
int main() 
{
    printf("LoRaWAN + SHT31 + LED(p5) + Slider(p20) Demo\n");
    printf("Press BUTTON1 to send data.\n");

    led.period_ms(20);
    led.write(0.2f);

    mbed_trace_init();

    if (lorawan.initialize(&ev_queue) != LORAWAN_STATUS_OK) {
        printf("LoRa initialization failed!\n");
        return -1;
    }

    btn.fall(ev_queue.event(&send_message));

    callbacks.events = mbed::callback(lora_event_handler);
    lorawan.add_app_callbacks(&callbacks);

    lorawan.disable_adaptive_datarate();
    lorawan.set_datarate(5);

    lorawan_connect_t connect_params;
    connect_params.connect_type = LORAWAN_CONNECTION_OTAA;
    connect_params.connection_u.otaa.dev_eui = DEV_EUI;
    connect_params.connection_u.otaa.app_eui = APP_EUI;
    connect_params.connection_u.otaa.app_key = APP_KEY;
    connect_params.connection_u.otaa.nb_trials = 3;

    lorawan_status_t ret = lorawan.connect(connect_params);

    if (ret != LORAWAN_STATUS_OK 
     && ret != LORAWAN_STATUS_CONNECT_IN_PROGRESS) 
    {
        printf("Connection error: %d\n", ret);
        return -1;
    }

    printf("Connection - In Progress ...\r\n");

    ev_queue.dispatch_forever();
    return 0;
}
