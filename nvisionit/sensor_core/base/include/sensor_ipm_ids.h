
/* Developed by nVisionIT */

#ifndef SENSOR_IPM_IDS
#define SENSOR_IPM_IDS

#define IPM_ID_COMMON   0xBEEF0000
#define IPM_ID_POWER    (0x1 & IPM_ID_COMMON)
#define IPM_ID_PIR      (0x2 & IPM_ID_COMMON)
#define IPM_ID_MAGBREAK (0x3 & IPM_ID_COMMON)
#define IPM_ID_HRS      (0x4 & IPM_ID_COMMON)
#define IPM_ID_GYRO     (0x5 & IPM_ID_COMMON)
#define IPM_ID_TEMP     (0x6 & IPM_ID_COMMON)

#endif