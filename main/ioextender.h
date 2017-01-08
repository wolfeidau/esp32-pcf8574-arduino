# pragma once

#include "freertos/FreeRTOS.h"

#include <stdbool.h>
#include "freertos/semphr.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#include <pcf8574_esp.h>
#include <Wire.h>


void ioextender_init();