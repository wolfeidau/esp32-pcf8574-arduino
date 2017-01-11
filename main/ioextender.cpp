#include "ioextender.h"
#include "esp_log.h"

static const char *TAG = "ioextender";

TwoWire testWire(1);
PCF857x pcf8574(0x20, &testWire);

static QueueHandle_t *subscriptions;
static size_t num_subscriptions;

static SemaphoreHandle_t interrupt_sem;

static void pcf8574_check_task(void *pvParameter);
void setup_pcf8574();
static void PCFInterrupt();
bool check_button(button_check_s* button);

void ioextender_initialize() {
  interrupt_sem = xSemaphoreCreateBinary();
  xTaskCreate(pcf8574_check_task, "pcf8574_check_task", 4096, NULL, 1, NULL);
}

bool buttons_subscribe(QueueHandle_t queue)
{
  void *new_subscriptions = realloc(subscriptions, (num_subscriptions + 1) * sizeof(QueueHandle_t));
  if (!new_subscriptions) {
	ESP_LOGE(TAG, "Failed to allocate new subscription #%d", (num_subscriptions+1));
	return false;
  }

  num_subscriptions++;
  subscriptions = (QueueHandle_t *)new_subscriptions;
  subscriptions[num_subscriptions-1] = queue;
  return true;
}

static void pcf8574_check_task(void *pvParameter)
{

  button_check_s buttonA = {0, 0, HIGH, IOEXT_A_BTN, "ButtonA"};
  button_check_s buttonB = {0, 0, HIGH, IOEXT_B_BTN, "ButtonB"};
  button_check_s encoderButton = {0, 0, HIGH, IOEXT_ENCODER_BTN, "EncoderButton"};

  setup_pcf8574();

  while(1) {
	xSemaphoreTake(interrupt_sem, portMAX_DELAY); /* Wait for interrupt */
	pcf8574.resetInterruptPin(); /* reset before reading state, so we don't miss any events */

	check_button(&buttonA);
	check_button(&buttonB);
	check_button(&encoderButton);

  }
}

void setup_pcf8574() {

  testWire.begin(GPIO_NUM_21, GPIO_NUM_22);
  testWire.setClock(100000L);

  pcf8574.begin();
  pinMode(IOEXT_INTERRUPT_PIN, INPUT_PULLUP);
  pcf8574.resetInterruptPin();
  attachInterrupt(digitalPinToInterrupt(IOEXT_INTERRUPT_PIN), PCFInterrupt, FALLING);

}

// this is a simple lockout debounce function
bool check_button(button_check_s* button) {

  uint8_t readState = pcf8574.read(button->pin);

  if (readState == button->state) {
    return false; // no state change
  }

  if (millis() - button->previous_millis >= IOEXT_DEBOUNCE_MILLIS) {
    // We have passed the time threshold, so a new change of state is allowed.
    button->previous_millis = millis();
    button->state = readState;

    button_reading_t reading = {
	  .label = button->label,
	  .state = stringFromState(button->state),
    };

    for (int i = 0; i < num_subscriptions; i++) {
      xQueueSendToBack(subscriptions[i], &reading, 0);
    }

    return true;
  }

  return false;
}

static void PCFInterrupt() {
  portBASE_TYPE higher_task_awoken;
  xSemaphoreGiveFromISR(interrupt_sem, &higher_task_awoken);
  if (higher_task_awoken) {
	portYIELD_FROM_ISR();
  }
}
