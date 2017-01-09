#include "ioextender.h"

#define IOEXT_INTERRUPT_PIN GPIO_NUM_25
#define IOEXT_POLL_INTERVAL_MILLIS 50
#define IOEXT_DEBOUNCE_MILLIS 100

TwoWire testWire(1);
PCF857x pcf8574(0x20, &testWire);

volatile bool PCFInterruptFlag = false;

static void pcf8574_check_task(void *pvParameter);
void setup_pcf8574();
void PCFInterrupt();
bool check_button(button_check_s* button);

void ioextender_initialize() {
  xTaskCreatePinnedToCore(pcf8574_check_task, "pcf8574_check_task", 4096, NULL, 1, NULL, 1);
}

static void pcf8574_check_task(void *pvParameter)
{

  button_check_s buttonA = {0, 0, HIGH, IOEXT_A_BTN};
  button_check_s buttonB = {0, 0, HIGH, IOEXT_B_BTN};
  button_check_s encoderButton = {0, 0, HIGH, IOEXT_ENCODER_BTN};

  setup_pcf8574();

  while(1) {
    if(PCFInterruptFlag){
      if (check_button(&buttonA)) Serial.printf("Button A is now: %3d\n", buttonA.state);
      if (check_button(&buttonB)) Serial.printf("Button B is now: %3d\n", buttonB.state);
      if (check_button(&encoderButton)) Serial.printf("Encoder Button is now: %3d\n", encoderButton.state);

      pcf8574.resetInterruptPin();
      PCFInterruptFlag = false;
    }

    vTaskDelay(IOEXT_POLL_INTERVAL_MILLIS / portTICK_PERIOD_MS);
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
    return true;
  }

  return false;
}

void PCFInterrupt() {
  PCFInterruptFlag = true;
}

