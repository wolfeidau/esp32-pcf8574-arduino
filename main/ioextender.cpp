#include "ioextender.h"

TwoWire testWire(1);
PCF857x pcf8574(0x20, &testWire);

bool PCFInterruptFlag = false;

static void pcf8574_check_task(void *pvParameter);
void setup_pcf8574();
void poll_pcf8574();
void PCFInterrupt();

void ioextender_initialize() {
    xTaskCreate(pcf8574_check_task, "pcf8574_check_task", 4096, NULL, 1, NULL);
}

static void pcf8574_check_task(void *pvParameter)
{
    setup_pcf8574();

    while(1) {
        poll_pcf8574();
    }
}

void setup_pcf8574() {

  testWire.begin(GPIO_NUM_21, GPIO_NUM_22);
  testWire.setClock(100000L);
  pcf8574.begin();
  pinMode(GPIO_NUM_25, INPUT_PULLUP);
  pcf8574.resetInterruptPin();
  attachInterrupt(digitalPinToInterrupt(GPIO_NUM_25), PCFInterrupt, FALLING);

}

void poll_pcf8574() {
  
  if(PCFInterruptFlag){
    detachInterrupt(digitalPinToInterrupt(GPIO_NUM_25));
    Serial.println("Got an interrupt: ");
    
    if(pcf8574.read(1)==HIGH) Serial.println("Button A is HIGH!");
    else Serial.println("Button A is LOW!"); // it is currently down
    pcf8574.write(7, pcf8574.read(1));
    
    if(pcf8574.read(2)==HIGH) Serial.println("Button B is HIGH!");
    else Serial.println("Button B is LOW!"); // it is currently down
    pcf8574.write(7, pcf8574.read(2));

    if(pcf8574.read(3)==HIGH) Serial.println("Encoder Button is HIGH!");
    else Serial.println("Encoder Button is LOW!"); // it is currently down
    pcf8574.write(7, pcf8574.read(3));

    PCFInterruptFlag=false;
    pcf8574.resetInterruptPin();
    attachInterrupt(digitalPinToInterrupt(GPIO_NUM_25), PCFInterrupt, FALLING);
  }

  delay(50);
}

void PCFInterrupt() {
  PCFInterruptFlag = true;
}

