#ifndef GPS_TRACKER_H
#define GPS_TRACKER_H

#include <Arduino.h>
#include <TinyGPSPlus.h>

// Structure to hold all GPS data
struct GPSData {
    float latitude;
    float longitude;
    float altitude;
    float speed;
    float course;
    int satellites;
    int hdop;          // Horizontal Dilution of Precision
    bool locationValid;
    bool altitudeValid;
    bool speedValid;
    bool courseValid;
    bool satellitesValid;
    bool hdopValid;
    String date;
    String time;
    bool dateValid;
    bool timeValid;
};

void powerGPS(bool state);
int gpsHealthCheck();
GPSData getGPSData(unsigned long timeout = 10000);

#endif