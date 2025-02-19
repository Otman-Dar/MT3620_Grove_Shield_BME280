//GROVE_NAME        "Grove - Temp&Humi&Barometer Sensor (BME280)"
//SKU               101020193
//WIKI_URL          http://wiki.seeedstudio.com/Grove-Barometer_Sensor-BME280/

#pragma once
#include "../applibs_versions.h"
void* GroveTempHumiBaroBME280_Open(int i2cFd);
void GroveTempHumiBaroBME280_ReadTemperature(void* inst);
void GroveTempHumiBaroBME280_ReadPressure(void* inst);
void GroveTempHumiBaroBME280_ReadHumidity(void* inst);
float GroveTempHumiBaroBME280_GetTemperature(void* inst);
float GroveTempHumiBaroBME280_GetPressure(void* inst);
float GroveTempHumiBaroBME280_GetHumidity(void* inst);


