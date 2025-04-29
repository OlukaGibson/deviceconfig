#ifndef PM_SENSOR_H
#define PM_SENSOR_H

#include <Arduino.h>
#include <TinyGPSPlus.h>


void powerPMS(bool state);
bool readPMSdata(Stream *s);
int pmSensorHealthCheck(String sensor);

#endif