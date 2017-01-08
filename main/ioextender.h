# pragma once

#include "freertos/FreeRTOS.h"

#include <stdbool.h>
#include "freertos/semphr.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#include <pcf8574_esp.h>
#include <Wire.h>

#define IOEXT_ACCEL_INT          0
#define IOEXT_A_BTN              1
#define IOEXT_B_BTN              2
#define IOEXT_ENCODER_BTN        3
#define IOEXT_SDCARD_SEL         4
#define IOEXT_TOUCH_INT          5
#define IOEXT_TOUCH_SEL          6
#define IOEXT_BACKLIGHT_CONTROL  7

typedef struct{
    unsigned long previous_millis;
    uint16_t last_state;
    uint8_t state;
    uint8_t pin;
} button_check_s;

void ioextender_initialize();