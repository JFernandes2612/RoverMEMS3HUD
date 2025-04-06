#include "src/RoverMEMS3HUDApplication.hpp"
#include "src/Debug.hpp"

RoverMEMS3HUDApplication *app;

void setup()
{
#if DEBUG || SHOW_FRAMERATE
    Serial.begin(115200);
    Serial.println("Starting Rover MEMS3 HUD");
#endif
    app = new RoverMEMS3HUDApplication();
}

#if SHOW_FRAMERATE
uint64_t prevTime = 0;
#endif

void loop()
{
#if SHOW_FRAMERATE
    prevTime = millis();
#endif
    app->loop();
#if SHOW_FRAMERATE
    Serial.printf("%ffps\n", 1000.0 / ((double)millis() - (double)prevTime));
#endif
}
