#ifndef PM_SENSOR_H
#define PM_SENSOR_H

#include <Arduino.h>

int8_t pmSensorHealthCheck();
void pmSensorTurnOn();
void pmSensorTurnOff();
bool readPMSdata(Stream *s);

#endif