/* I2C example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/


#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <esp_wifi.h>
#include <esp_event.h>
#include <nvs_flash.h>
#include "esp_netif.h"
#include <lwip/err.h>
#include <lwip/sys.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#include "esp_log.h"
#include "esp_system.h"
#include "esp_err.h"

#include "driver/adc.h"
#include "driver/hw_timer.h"

#include <esp_http_server.h>
#include "common.h"
#include "math.h"

#include "I2Cdev.h"

#include "MPU6050_6Axis_MotionApps20.h"
#include "Wire.h"

#define HW_TIMER_IN_US 10000

void wifi_init_sta(void);
void wifi_init_softap();
httpd_handle_t start_webserver(void);
int wifi_mode_not_setted = 1;

int init_accel_speed_counter = -1;

static const char *TAG = "main";

httpd_handle_t server = NULL;

double magnetoField = 1000000;
double magnetoFieldMean = 0;
double magnetoFieldMax = -1000000;
double magnetoFieldMin = 1000000;
bool magnetoFieldInit = 0;

XYZT gyro = {0, 0, 0, 0};
XYZT accelI = {0, 0, 0, 0};
XYZT accelN = {0, 0, 0, 0};
XYZT accelP = {0, 0, 0, 0};
XYZT speed = {0, 0, 0, 0};
double Temp;
uint8_t who_am_i;

double time_in_s = 0;

MPU6050 mpu;

#define OUTPUT_READABLE_WORLDACCEL

// MPU control/status vars
bool dmpReady = false;  // set true if DMP init was successful
uint8_t mpuIntStatus;   // holds actual interrupt status byte from MPU
uint8_t devStatus;      // return status after each device operation (0 = success, !0 = error)
uint16_t packetSize;    // expected DMP packet size (default is 42 bytes)
uint16_t fifoCount;     // count of all bytes currently in FIFO
uint8_t fifoBuffer[64]; // FIFO storage buffer

// orientation/motion vars
Quaternion q;        // [w, x, y, z]         quaternion container
VectorInt16 aa;      // [x, y, z]            accel sensor measurements
VectorInt16 aaReal;  // [x, y, z]            gravity-free accel sensor measurements
VectorInt16 aaWorld; // [x, y, z]            world-frame accel sensor measurements
VectorFloat gravity; // [x, y, z]            gravity vector

bool mpuInterrupt = false; // indicates whether MPU interrupt pin has gone high


int isRecording = 1;
char data[64]; // MPU data to be written to SD card //#

char dataTime[64];              // A separator for millis() data  //#
unsigned long ellapsedTime = 0; // Contais millis() data //#

bool fifoOverflow = false; // Red light.
bool canRecord = false;    // Blue light. Timer will confirm if it is ok to start recording
                        // isRecording is green light


static void hw_timer_callback()
{
    time_in_s += (double)HW_TIMER_IN_US / 1000000;
    int ret;
    

        uint16_t val;
        ret = adc_read (&val);
        if (ret == ESP_OK) {
            double valD = (double)val;
            valD *= 3;
            valD *= 3.3;
            valD /= 2048;
            valD /= 18;
            if (magnetoFieldInit == 0) {
                magnetoFieldMin = 1000000;
                magnetoFieldMax = -1000000;
            } else {
                if (magnetoFieldMax < valD) magnetoFieldMax = valD;
                if (magnetoFieldMin > valD) magnetoFieldMin = valD;
                magnetoFieldMean = (magnetoFieldMax + magnetoFieldMin) / 2;
            }
            valD -= magnetoFieldMean;
            if (magnetoField == 1000000) {
                magnetoField = valD;
            } else {
                magnetoField *= 3;
                magnetoField += valD;
                magnetoField /= 4;
            }
        }
}

double get_speed_abs () {
    double ret = 0;
    ret += speed.x * speed.x;
    ret += speed.y * speed.y;
    ret += speed.z * speed.z;
    ret = sqrt(ret);
    return ret;
}

double get_accel_abs () {
    double ret = 0;
    ret += accelN.x * accelN.x;
    ret += accelN.y * accelN.y;
    ret += accelN.z * accelN.z;
    ret = sqrt(ret);
    return ret;
}

double get_magnetofield () {
    return magnetoField;
}

double get_magnetofieldMean () {
    return magnetoFieldMean;
}

double get_magnetofieldMax () {
    return magnetoFieldMax;
}

double get_magnetofieldMin () {
    return magnetoFieldMin;
}


void init_accel_speed () {
    init_accel_speed_counter = 0;
    accelP = accelN;
    accelI = accelN;
    speed.x = 0;
    speed.y = 0;
    speed.z = 0;
    speed.t = 0;
}

static void task_example(void *arg)
{
    while (1) {
        
            //vTaskDelay(1000 / portTICK_RATE_MS);
    }
}

wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();

void app_main(void)
{
    esp_err_t err;
    adc_config_t adc_cfg;
    adc_cfg.mode = ADC_READ_TOUT_MODE;
    adc_cfg.clk_div = 32;

    ESP_ERROR_CHECK(nvs_flash_init());

    tcpip_adapter_init();

    ESP_ERROR_CHECK(esp_event_loop_create_default());

    
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    wifi_init_sta();
    wifi_init_softap();

    err = adc_init(&adc_cfg);
    if (err != ESP_OK) {
            ESP_LOGI(TAG, "ADC init KO");
    }

    hw_timer_init(hw_timer_callback, NULL);
    hw_timer_alarm_us(HW_TIMER_IN_US, true);
    xTaskCreate(task_example, "task_example", 2048, NULL, 10, NULL);

    server = start_webserver();
}
