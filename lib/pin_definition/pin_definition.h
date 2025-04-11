#ifndef PIN_DEFINITIONS_H
#define PIN_DEFINITIONS_H

// Define pin numbers for various sensors and modules
// gsm related pins Serial1
#define GSM_POWER_SWITCH_PIN    A0 
#define RX1_PIN                 D19
#define TX1_PIN                 D18
#define GSM_RESET_PIN           D10

// gps related pins
#define GPS_POWER_SWITCH_PIN    A11
#define RX2_PIN                 D17
#define TX2_PIN                 D16

//pms related pins
#define PMS_POWER_SWITCH_PIN    A2 
#define PMS_RX3_PIN             D15
#define PMS_TX3_PIN             D14
#define PMS_SSRX_PIN            D13
#define PMS_SSTX_PIN            D12

//dht and sht related pins
#define DHT_SHT_POWER_SWITCH_PIN    A8 
#define DHT_DATA_PIN                A3 
#define SHT_SDA_PIN                 D20
#define SHT_SCL_PIN                 D21

//sd card related pins
#define SD_POWER_SWITCH_PIN     D30
#define SD_CS_PIN               D28
#define SD_MOSI_PIN             D51 
#define SD_MISO_PIN             D50
#define SD_SCK_PIN              D52

//RTC related pins
#define RTC_POWER_SWITCH_PIN    D31
#define RTC_SDA_PIN            D20
#define RTC_SCL_PIN            D21

#endif // PIN_DEFINITIONS_H