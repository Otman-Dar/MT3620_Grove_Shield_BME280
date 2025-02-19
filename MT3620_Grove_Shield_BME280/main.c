
#include <errno.h>
#include <string.h>
#include <time.h>
#include <signal.h>
#include <curl/curl.h> // Include cURL

#include <applibs/log.h>
#include <applibs/gpio.h>
#include <hw/template_appliance.h>

#include "./MT3620_Grove_Shield_Library/Grove.h"
#include "./MT3620_Grove_Shield_Library/Sensors/GroveTempHumiBaroBME280.h"

#define SERVER_URL "http://172.20.10.7:9999" // Replace with your server IP & port

static volatile sig_atomic_t terminationRequested = false;

/// <summary>
///     Signal handler for termination requests. This handler must be async-signal-safe.
/// </summary>
static void TerminationHandler(int signalNumber)
{
    // Don't use Log_Debug here, as it is not guaranteed to be async signal safe
    terminationRequested = true;
}


static void SendSensorData(float temp, float humid, float press)
{
    CURL* curl;
    CURLcode res;

    // JSON payload buffer
    char jsonPayload[128];
    snprintf(jsonPayload, sizeof(jsonPayload),
        "{\"temperature\": %.2f, \"humidity\": %.2f, \"pressure\": %.2f}",
        temp, humid, press);

    Log_Debug("Sending JSON: %s\n", jsonPayload);

    curl_global_init(CURL_GLOBAL_ALL);
    curl = curl_easy_init();

    if (curl) {
        struct curl_slist* headers = NULL;
        headers = curl_slist_append(headers, "Content-Type: application/json");

        curl_easy_setopt(curl, CURLOPT_URL, SERVER_URL);
        curl_easy_setopt(curl, CURLOPT_POST, 1L);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, jsonPayload);
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 3L); // Timeout after 10s

        res = curl_easy_perform(curl);

        // somehow perform returns 28 (timeout) even if the op was successful
        //  && res != CURLE_OPERATION_TIMEDOUT
        if (res != CURLE_OK) {
            Log_Debug("cURL POST failed: %d %s\n", res, curl_easy_strerror(res));
        }
        else {
            Log_Debug("Data successfully sent to server.\n");
        }

        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
    }
    else {
        Log_Debug("Failed to initialize cURL\n");
    }

    curl_global_cleanup();
}


int main(int argc, char* argv[])
{
    Log_Debug("Application starting\n");

    // Register a SIGTERM handler for termination requests
    struct sigaction action;
    memset(&action, 0, sizeof(struct sigaction));
    action.sa_handler = TerminationHandler;
    sigaction(SIGTERM, &action, NULL);

    int i2cFd;
    int rc = GroveShield_Initialize(&i2cFd, 115200);
    if (rc) {
        Log_Debug("ERROR: GroveShield_Initialize failed");
        return -1;
    }

    void* bme280 = GroveTempHumiBaroBME280_Open(i2cFd);
    if (!bme280) {
        Log_Debug("ERROR: GroveTempHumiBaroBME280_Open returnd NULL");

        return -1;
    }

    Log_Debug("=================================================\n");
    Log_Debug("Hello from BME280\n");
    Log_Debug("=================================================\n");
    Log_Debug("\n");

    // Main loop
    while (!terminationRequested) {
        GroveTempHumiBaroBME280_ReadTemperature(bme280);
        GroveTempHumiBaroBME280_ReadPressure(bme280);
        GroveTempHumiBaroBME280_ReadHumidity(bme280);
        float temp = GroveTempHumiBaroBME280_GetTemperature(bme280);
        float humid = GroveTempHumiBaroBME280_GetHumidity(bme280);
        Log_Debug("\nTemperature: %.1fC\n", temp);
        Log_Debug("Humidity: %.1f\%c\n", humid, 0x25);
        float press = GroveTempHumiBaroBME280_GetPressure(bme280);
        Log_Debug("Pressure: %.1fhPa\n", press);

        SendSensorData(temp, humid, press);
        usleep(5000000);
    }

    Log_Debug("Application exiting\n");
    return 0;
}