#define PIN_BIT(x) (1ULL<<x)

#define BUTTON_DOWN (1)
#define BUTTON_UP (2)

typedef struct {
	uint8_t pin;
    uint8_t event;
} button_event_t;

QueueHandle_t * button_init(unsigned long long pin_select);


/*
//https://github.com/craftmetrics/esp32-button

button_event_t ev;
QueueHandle_t button_events = button_init(PIN_BIT(BUTTON_1) | PIN_BIT(BUTTON_2));
while (true) {
    if (xQueueReceive(button_events, &ev, 1000/portTICK_PERIOD_MS)) {
        if ((ev.pin == BUTTON_1) && (ev.event == BUTTON_DOWN)) {
            // ...
        }
        if ((ev.pin == BUTTON_2) && (ev.event == BUTTON_DOWN)) {
            // ...
        }
    }
}

*/