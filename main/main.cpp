#include "ioextender.h"
#include "freertos/task.h"

extern "C" void app_main()
{
    Serial.begin(115200);
    
    ioextender_init();
}

