#include "src/RoverMEMS3HUDApplication.hpp"
#include "src/Debug.hpp"

RoverMEMS3HUDApplication *app;

void setup()
{   
#if DEBUG
    Serial.begin(115200);
    Serial.println("Starting Rover MEMS3 HUD");
#endif

    app = new RoverMEMS3HUDApplication();

    delay(1000); // let some time to make sure everything is initialized
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
    Serial.printf("%ldms\n", 1000 / (millis() - prevTime));
#endif
}
