#include "wifi.h"
#include "ioextender.h"

extern "C" void app_main()
{
    Serial.begin(115200);
    
    wifi_initialize();
    ioextender_initialize();
}

