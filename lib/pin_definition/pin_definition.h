#ifndef PIN_DEFINITIONS_H
#define PIN_DEFINITIONS_H

// Define pin numbers for various sensors and modules
// gsm related pins Serial1
#define GSM_POWER_SWITCH_PIN    A0 
#define RX1_PIN                 19
#define TX1_PIN                 18
#define GSM_RESET_PIN           10

// gps related pins
#define GPS_POWER_SWITCH_PIN    A11
#define RX2_PIN                 17
#define TX2_PIN                 16

//pms related pins
#define PMS_POWER_SWITCH_PIN    A2 
#define PMS_RX3_PIN             15 //SENSOR 1
#define PMS_TX3_PIN             14
#define PMS_SSRX_PIN            13 //SENSOR 2
#define PMS_SSTX_PIN            12

//dht and sht related pins
#define DHT_SHT_POWER_SWITCH_PIN    A8 
#define DHT_DATA_PIN                A3 
#define SHT_SDA_PIN                 20
#define SHT_SCL_PIN                 21
#define DHT_TYPE                    DHT11 

//sd card related pins
#define SD_POWER_SWITCH_PIN     30
#define SD_CS_PIN               28
#define SD_MOSI_PIN             51 
#define SD_MISO_PIN             50
#define SD_SCK_PIN              52

//RTC related pins
#define RTC_POWER_SWITCH_PIN    31
#define RTC_SDA_PIN            20
#define RTC_SCL_PIN            21

#endif // PIN_DEFINITIONS_H