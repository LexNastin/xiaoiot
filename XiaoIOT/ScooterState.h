#ifndef ScooterState_h
#define ScooterState_h

struct ScooterState {
  public: // prevent any sudden unexpected drops...
    float batteryVoltage = 0; // check [before sending]/[post receiving]
    float speed = 0;
    bool inMotion = false;
    bool locked = false; // custom lock
    int batteryPercentage = 0; // check (perhaps add 1 and then subtract one on server)
    int throttle = 0;
    int brake = 0;
    int amperage = 0;
    float ecuTemperature = 0; // check (perhaps add 0.1 and then sub)
    float batteryTemperature = 0; // check
    int remainingMah = 0; // check (add 1)
    int uptime = 0; // check
    bool isCharging = false;
    int tripKm = 0;
    bool lightOn = false;
    int mode = 0;
    int displayValue = 0; // don't send
    int leds = 0; // don't send
};

#endif