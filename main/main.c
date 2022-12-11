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
#include "driver/i2c.h"
#include "driver/hw_timer.h"

#include <esp_http_server.h>
#include "common.h"
#include "math.h"

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
int magnetoFieldInit = 0;

int accelero_started = 0;
int accel_speed_has_to_be_switch = 0;

XYZT accelWG = {0, 0, 0, 0};
XYZT gyro = {0, 0, 0, 0};
XYZT accelI = {0, 0, 0, 0};
XYZT accelN = {0, 0, 0, 0};
XYZT accelP = {0, 0, 0, 0};
XYZT speed = {0, 0, 0, 0};

double accelMaxWG = 0;;

double Temp;
uint8_t who_am_i;

double time_in_s = 0;


/**
 * TEST CODE BRIEF
 *
 * This example will show you how to use I2C module by running two tasks on i2c bus:
 *
 * - read external i2c sensor, here we use a MPU6050 sensor for instance.
 * - Use one I2C port(master mode) to read or write the other I2C port(slave mode) on one ESP8266 chip.
 *
 * Pin assignment:
 *
 * - master:
 *    GPIO14 is assigned as the data signal of i2c master port
 *    GPIO2 is assigned as the clock signal of i2c master port
 *
 * Connection:
 *
 * - connect sda/scl of sensor with GPIO14/GPIO2
 * - no need to add external pull-up resistors, driver will enable internal pull-up resistors.
 *
 * Test items:
 *
 * - read the sensor data, if connected.
 */


#define I2C_EXAMPLE_MASTER_SCL_IO           GPIO_NUM_14               /*!< gpio number for I2C master clock */
#define I2C_EXAMPLE_MASTER_SDA_IO           GPIO_NUM_12               /*!< gpio number for I2C master data  */
#define I2C_EXAMPLE_MASTER_NUM              I2C_NUM_0        /*!< I2C port number for master dev */
#define I2C_EXAMPLE_MASTER_TX_BUF_DISABLE   0                /*!< I2C master do not need buffer */
#define I2C_EXAMPLE_MASTER_RX_BUF_DISABLE   0                /*!< I2C master do not need buffer */

#define MPU6050_SENSOR_ADDR                 0x68             /*!< slave address for MPU6050 sensor */
#define MPU6050_CMD_START                   0x41             /*!< Command to set measure mode */
#define MPU6050_WHO_AM_I                    0x75             /*!< Command to read WHO_AM_I reg */
#define WRITE_BIT                           I2C_MASTER_WRITE /*!< I2C master write */
#define READ_BIT                            I2C_MASTER_READ  /*!< I2C master read */
#define ACK_CHECK_EN                        0x1              /*!< I2C master will check ack from slave*/
#define ACK_CHECK_DIS                       0x0              /*!< I2C master will not check ack from slave */
#define ACK_VAL                             0x0              /*!< I2C ack value */
#define NACK_VAL                            0x1              /*!< I2C nack value */
#define LAST_NACK_VAL                       0x2              /*!< I2C last_nack value */

/**
 * Define the mpu6050 register address:
 */
#define SMPLRT_DIV      0x19
#define CONFIG          0x1A
#define GYRO_CONFIG     0x1B
#define ACCEL_CONFIG    0x1C
#define ACCEL_XOUT_H    0x3B
#define ACCEL_XOUT_L    0x3C
#define ACCEL_YOUT_H    0x3D
#define ACCEL_YOUT_L    0x3E
#define ACCEL_ZOUT_H    0x3F
#define ACCEL_ZOUT_L    0x40
#define TEMP_OUT_H      0x41
#define TEMP_OUT_L      0x42
#define GYRO_XOUT_H     0x43
#define GYRO_XOUT_L     0x44
#define GYRO_YOUT_H     0x45
#define GYRO_YOUT_L     0x46
#define GYRO_ZOUT_H     0x47
#define GYRO_ZOUT_L     0x48
#define PWR_MGMT_1      0x6B
#define WHO_AM_I        0x75  /*!< Command to read WHO_AM_I reg */

/**
 * @brief i2c master initialization
 */
static esp_err_t i2c_example_master_init()
{
    i2c_port_t i2c_master_port = I2C_EXAMPLE_MASTER_NUM;
    i2c_config_t conf;
    conf.mode = I2C_MODE_MASTER;
    conf.sda_io_num = I2C_EXAMPLE_MASTER_SDA_IO;
    conf.sda_pullup_en = GPIO_PULLUP_ENABLE;
    conf.scl_io_num = I2C_EXAMPLE_MASTER_SCL_IO;
    conf.scl_pullup_en = GPIO_PULLUP_ENABLE;
    //conf.clk_stretch_tick = 300; // 300 ticks, Clock stretch is about 210us, you can make changes according to the actual situation.
    ESP_ERROR_CHECK(i2c_driver_install(i2c_master_port, conf.mode));
    ESP_ERROR_CHECK(i2c_param_config(i2c_master_port, &conf));
    return ESP_OK;
}

/**
 * @brief test code to write mpu6050
 *
 * 1. send data
 * ___________________________________________________________________________________________________
 * | start | slave_addr + wr_bit + ack | write reg_address + ack | write data_len byte + ack  | stop |
 * --------|---------------------------|-------------------------|----------------------------|------|
 *
 * @param i2c_num I2C port number
 * @param reg_address slave reg address
 * @param data data to send
 * @param data_len data length
 *
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG Parameter error
 *     - ESP_FAIL Sending command error, slave doesn't ACK the transfer.
 *     - ESP_ERR_INVALID_STATE I2C driver not installed or not in master mode.
 *     - ESP_ERR_TIMEOUT Operation timeout because the bus is busy.
 */
static esp_err_t i2c_example_master_mpu6050_write(i2c_port_t i2c_num, uint8_t reg_address, uint8_t *data, size_t data_len)
{
    int ret;
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, MPU6050_SENSOR_ADDR << 1 | WRITE_BIT, ACK_CHECK_EN);
    i2c_master_write_byte(cmd, reg_address, ACK_CHECK_EN);
    i2c_master_write(cmd, data, data_len, ACK_CHECK_EN);
    i2c_master_stop(cmd);
    ret = i2c_master_cmd_begin(i2c_num, cmd, 1000 / portTICK_RATE_MS);
    i2c_cmd_link_delete(cmd);

    return ret;
}

/**
 * @brief test code to read mpu6050
 *
 * 1. send reg address
 * ______________________________________________________________________
 * | start | slave_addr + wr_bit + ack | write reg_address + ack | stop |
 * --------|---------------------------|-------------------------|------|
 *
 * 2. read data
 * ___________________________________________________________________________________
 * | start | slave_addr + wr_bit + ack | read data_len byte + ack(last nack)  | stop |
 * --------|---------------------------|--------------------------------------|------|
 *
 * @param i2c_num I2C port number
 * @param reg_address slave reg address
 * @param data data to read
 * @param data_len data length
 *
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG Parameter error
 *     - ESP_FAIL Sending command error, slave doesn't ACK the transfer.
 *     - ESP_ERR_INVALID_STATE I2C driver not installed or not in master mode.
 *     - ESP_ERR_TIMEOUT Operation timeout because the bus is busy.
 */
static esp_err_t i2c_example_master_mpu6050_read(i2c_port_t i2c_num, uint8_t reg_address, uint8_t *data, size_t data_len)
{
    int ret;
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, MPU6050_SENSOR_ADDR << 1 | WRITE_BIT, ACK_CHECK_EN);
    i2c_master_write_byte(cmd, reg_address, ACK_CHECK_EN);
    i2c_master_stop(cmd);
    ret = i2c_master_cmd_begin(i2c_num, cmd, 1000 / portTICK_RATE_MS);
    i2c_cmd_link_delete(cmd);

    if (ret != ESP_OK) {
        return ret;
    }

    cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, MPU6050_SENSOR_ADDR << 1 | READ_BIT, ACK_CHECK_EN);
    i2c_master_read(cmd, data, data_len, LAST_NACK_VAL);
    i2c_master_stop(cmd);
    ret = i2c_master_cmd_begin(i2c_num, cmd, 1000 / portTICK_RATE_MS);
    i2c_cmd_link_delete(cmd);

    return ret;
}

static esp_err_t i2c_example_master_mpu6050_init(i2c_port_t i2c_num)
{
    uint8_t cmd_data[4];
    unsigned int state = 0;
    ESP_ERROR_CHECK(gpio_set_level(GPIO_NUM_15, 1));
    vTaskDelay(100 / portTICK_RATE_MS);
    i2c_example_master_init();
    vTaskDelay(500 / portTICK_RATE_MS);

    cmd_data[0] = 0x80;    // reset mpu6050
    ESP_ERROR_CHECK(i2c_example_master_mpu6050_write(i2c_num, 0x6B, cmd_data, 1));
    vTaskDelay(1000 / portTICK_RATE_MS);
    cmd_data[0] = 0x10;    // reset mpu6050
    ESP_ERROR_CHECK(i2c_example_master_mpu6050_write(i2c_num, 0x6B, cmd_data, 1));
    //cmd_data[0] = 0x06;    // Set the Low Pass Filter
    //ESP_ERROR_CHECK(i2c_example_master_mpu6050_write(i2c_num, CONFIG, cmd_data, 1));
    //cmd_data[0] = 0x10;
    //ESP_ERROR_CHECK(i2c_example_master_mpu6050_write(i2c_num, 0x6B, cmd_data, 1));
    //vTaskDelay(100 / portTICK_RATE_MS);
    cmd_data[0] = 0x00;    // Set the GYRO range
    ESP_ERROR_CHECK(i2c_example_master_mpu6050_write(i2c_num, 0x1B, cmd_data, 1));
    cmd_data[0] = 0x10;    // Set the ACCEL range
    ESP_ERROR_CHECK(i2c_example_master_mpu6050_write(i2c_num, 0x1C, cmd_data, 1));
    cmd_data[0] = 0x00;
    ESP_ERROR_CHECK(i2c_example_master_mpu6050_write(i2c_num, 0x38, cmd_data, 1));
    //cmd_data = 0x01;
    who_am_i = 0;
    i2c_example_master_mpu6050_read(I2C_EXAMPLE_MASTER_NUM, WHO_AM_I, &who_am_i, 1);
    ESP_LOGI(TAG, "WHO_AM_I: 0x%02x\n", who_am_i);
    vTaskDelay(500 / portTICK_RATE_MS);
    accelero_started = 1;
            ESP_LOGI(TAG, "Accelero started\n");
    return ESP_OK;
}

static int ret_hw_timer_callback = -6;
static unsigned int ret_hw_timer_callback_error = 0; 
static unsigned int ret_hw_timer_callback_OK = 0; 
static unsigned int ret_hw_timer_callback_count = 0; 
static unsigned int ret_hw_timer_callback_end = 0; 

static void hw_timer_callback(void* t)
{
    time_in_s += (double)HW_TIMER_IN_US / 1000000;
    static int counter = 0;
    counter++;
    uint16_t val;

        int ret = adc_read (&val);
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
    if (accelero_started == 0) return;
    if ((counter % 5) == 0) {
    uint8_t sensor_data[14];
    //uint8_t who_am_i, i;
    //static uint32_t error_count = 0;

    
    /*
    while (1) {
        who_am_i = 0;
        i2c_example_master_mpu6050_read(I2C_EXAMPLE_MASTER_NUM, WHO_AM_I, &who_am_i, 1);

        if (0x68 != who_am_i) {
            error_count++;
            ESP_LOGI(TAG, "Who am i KO\n");
        }
        */
        memset(sensor_data, 0, 14);
        ret_hw_timer_callback = i2c_example_master_mpu6050_read(I2C_EXAMPLE_MASTER_NUM, 0x3B, sensor_data, 14);
        //ret = i2c_example_master_mpu6050_read(I2C_EXAMPLE_MASTER_NUM, 0x3C, sensor_data, 1);
            ret_hw_timer_callback_count++;

        if (ret_hw_timer_callback == ESP_OK) {
            ret_hw_timer_callback_OK++;
            /*
            ESP_LOGI(TAG, "*******************\n");
            ESP_LOGI(TAG, "WHO_AM_I: 0x%02x\n", who_am_i);
            */
            Temp = 36.53 + ((double)(int16_t)((sensor_data[6] << 8) | sensor_data[7]) / 340);
            
            int16_t tmp;
            tmp = (sensor_data[0] << 8) | sensor_data[1];
            if (tmp & 0x8000) tmp = -( ~(tmp - 1) );
            accelN.x = (double)tmp;
            tmp = (sensor_data[2] << 8) | sensor_data[3];
            if (tmp & 0x8000) tmp = -( ~(tmp - 1) );
            accelN.y = (double)tmp;
            tmp = (sensor_data[4] << 8) | sensor_data[5];
            if (tmp & 0x8000) tmp = -( ~(tmp - 1) );
            accelN.z = (double)tmp;

            accelN.x /= 4096.0;
            accelN.y /= 4096.0;
            accelN.z /= 4096.0;
            accelN.t = time_in_s;
            accelWG.x = accelN.x - accelI.x;
            accelWG.y = accelN.y - accelI.y;
            accelWG.z = accelN.z - accelI.z;
            accelWG.t = time_in_s;
            
            double tmpD;
            tmpD = get_XYZT_abs(&accelWG);
            if (tmpD > accelMaxWG) accelMaxWG = tmpD;

            tmp = (sensor_data[8] << 8) | sensor_data[9];
            if (tmp & 0x8000) tmp = -( ~(tmp - 1) );
            gyro.x = (double)tmp;
            tmp = (sensor_data[10] << 8) | sensor_data[11];
            if (tmp & 0x8000) tmp = -( ~(tmp - 1) );
            gyro.y = (double)tmp;
            tmp = (sensor_data[12] << 8) | sensor_data[13];
            if (tmp & 0x8000) tmp = -( ~(tmp - 1) );
            gyro.z = (double)tmp;

            gyro.x /= 131.0;
            gyro.y /= 131.0;
            gyro.z /= 131.0;
            gyro.t = time_in_s;

            if (init_accel_speed_counter == -1) {
                tmpD = accelWG.x + accelP.x;
                tmpD /= 2;
                tmpD *= 9.80655;
                tmpD *= (accelWG.t - accelP.t);
                speed.x += tmpD;
            
                tmpD = accelWG.y + accelP.y;
                tmpD /= 2;
                tmpD *= 9.80655;
                tmpD *= (accelWG.t - accelP.t);
                speed.y += tmpD;
            
                tmpD = accelWG.z + accelP.z;
                tmpD /= 2;
                tmpD *= 9.80655;
                tmpD *= (accelWG.t - accelP.t);
                speed.z += tmpD;
            
                accelP = accelWG;
            } else if (init_accel_speed_counter == 100) {
                speed.x = 0;
                speed.y = 0;
                speed.z = 0;
                accelP = accelN;
                init_accel_speed_counter = -1;
            } else if (init_accel_speed_counter == 0) {
                accelI = accelN;
                init_accel_speed_counter++;
            } else {
                accelI.x *= init_accel_speed_counter;
                accelI.x += accelN.x;
                accelI.y *= init_accel_speed_counter;
                accelI.y += accelN.y;
                accelI.z *= init_accel_speed_counter;
                accelI.z += accelN.z;
                init_accel_speed_counter++;
                accelI.x /= init_accel_speed_counter;
                accelI.y /= init_accel_speed_counter;
                accelI.z /= init_accel_speed_counter;
            }
        } else {
            ret_hw_timer_callback_error++; 
        }
            ret_hw_timer_callback_end++;
    }

        //vTaskDelay(100 / portTICK_RATE_MS);
    //}

    //i2c_driver_delete(I2C_EXAMPLE_MASTER_NUM);
}

double get_XYZT_abs(XYZT *a) {
    double ret = 0;
    ret += a->x * a->x;
    ret += a->y * a->y;
    ret += a->z * a->z;
    ret = sqrt(ret);
    return ret;
}

void init_accel_speed () {
    init_accel_speed_counter = 0;
    accelP = accelN;
    accelI = accelN;
    speed.x = 0;
    speed.y = 0;
    speed.z = 0;
    speed.t = 0;
    accelWG.x = 0;
    accelWG.y = 0;
    accelWG.z = 0;
    accelWG.t = 0;
    accelMaxWG = 0;
}

static void i2c_task_example(void *arg)
{
    while (1) {
#if 0
            ESP_LOGI(TAG, "WHO_AM_I: 0x%02x\n", who_am_i);
            ESP_LOGI(TAG, "TEMP: %d.%d\n", (uint16_t)Temp, (uint16_t)(Temp * 100) % 100);
            ESP_LOGI(TAG, "accel: {x: %c%d.%03d, y: %c%d.%03d, z: %c%d.%03d}\n",
                    (accelN.x<0)?'-':'+', (unsigned int)(((accelN.x<0)?-1:1)*accelN.x), (unsigned int)(accelN.x * 1000) % 1000,
                    (accelN.y<0)?'-':'+', (unsigned int)(((accelN.y<0)?-1:1)*accelN.y), (unsigned int)(accelN.y * 1000) % 1000,
                    (accelN.z<0)?'-':'+', (unsigned int)(((accelN.z<0)?-1:1)*accelN.z), (unsigned int)(accelN.z * 1000) % 1000
                    );
            ESP_LOGI(TAG, "gyro: {x: %c%d.%03d, y: %c%d.%03d, z: %c%d.%03d}\n",
                    (gyro.x<0)?'-':'+', (unsigned int)(((gyro.x<0)?-1:1)*gyro.x), (unsigned int)(gyro.x * 1000) % 1000,
                    (gyro.y<0)?'-':'+', (unsigned int)(((gyro.y<0)?-1:1)*gyro.y), (unsigned int)(gyro.y * 1000) % 1000,
                    (gyro.z<0)?'-':'+', (unsigned int)(((gyro.z<0)?-1:1)*gyro.z), (unsigned int)(gyro.z * 1000) % 1000
                    );
#endif
            if (accel_speed_has_to_be_switch == 1) {
                    ESP_LOGI(TAG, "accel will be switch\n");
                accel_speed_has_to_be_switch = 0;
                i2c_example_master_mpu6050_init(I2C_EXAMPLE_MASTER_NUM);
            }
                    ESP_LOGW(TAG, "Pb with accel: %d\n", ret_hw_timer_callback);
            vTaskDelay(500 / portTICK_RATE_MS);
    }
    i2c_driver_delete(I2C_EXAMPLE_MASTER_NUM);
}

int GPIO_levels[17] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0} ;
int GPIO_previous_levels[17] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0} ;
double GPIO_levels_time[17] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0} ;
double GPIO_previous_levels_time[17] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0} ;
unsigned int counter = 0;
unsigned int counter_last = 0;
double counter_last_time_in_s = 0;
double counter_last_freq = 0;
static void gpio_isr_handler(void *arg)
{
    uint32_t gpio_num = (uint32_t) arg;
    int last_level = GPIO_levels[gpio_num];
    double last_level_time = GPIO_levels_time[gpio_num];
    GPIO_levels[gpio_num] = gpio_get_level(gpio_num);
    GPIO_levels_time[gpio_num] = time_in_s;
    if (GPIO_levels[gpio_num] != last_level) counter++;
    if (counter % 2 == 0) {
        double new_time_in_s = time_in_s;
        counter_last_freq = 1 / (2 * (new_time_in_s - counter_last_time_in_s) / (counter - counter_last));
        counter_last_time_in_s = new_time_in_s;
        counter_last = counter;
    }
}


wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();

void app_main(void)
{
    esp_err_t err;

    ESP_ERROR_CHECK(nvs_flash_init());
    
    gpio_config_t io_conf;
    io_conf.pin_bit_mask = BIT(GPIO_NUM_15) | BIT(GPIO_NUM_13);
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pull_down_en = 0;
    io_conf.pull_up_en = 0;
    gpio_config(&io_conf);
    ESP_ERROR_CHECK(gpio_set_level(GPIO_NUM_15, 0));
    ESP_ERROR_CHECK(gpio_set_level(GPIO_NUM_13, 0));
    vTaskDelay(3000 / portTICK_RATE_MS);
    tcpip_adapter_init();

    ESP_ERROR_CHECK(esp_event_loop_create_default());

    //            i2c_example_master_mpu6050_init(I2C_EXAMPLE_MASTER_NUM);
    
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    wifi_init_sta();
    wifi_init_softap();

    
    adc_config_t adc_cfg;
    adc_cfg.mode = ADC_READ_TOUT_MODE;
    adc_cfg.clk_div = 32;
    err = adc_init(&adc_cfg);
    if (err != ESP_OK) {
            ESP_LOGI(TAG, "ADC init KO");
    }
    
    //interrupt of rising edge
    io_conf.intr_type = GPIO_INTR_ANYEDGE;
    //bit mask of the pins, use GPIO4/5 here
    io_conf.pin_bit_mask = BIT(GPIO_NUM_4) | BIT(GPIO_NUM_5);
    //set as input mode
    io_conf.mode = GPIO_MODE_INPUT;
    //io_conf.pull_up_en = 1;
    //enable pull-up mode
    gpio_config(&io_conf);
    //change gpio intrrupt type for one pin
    //gpio_set_intr_type(GPIO_Pin_15, GPIO_INTR_ANYEDGE);
    gpio_install_isr_service(0);
    gpio_isr_handler_add(GPIO_NUM_4, gpio_isr_handler, (void *) GPIO_NUM_4);
    gpio_isr_handler_add(GPIO_NUM_5, gpio_isr_handler, (void *) GPIO_NUM_5);

    hw_timer_init(hw_timer_callback, NULL);
    hw_timer_alarm_us(HW_TIMER_IN_US, true);
    //xTaskCreate(i2c_task_example, "i2c_task_example", 2048, NULL, 10, NULL);

    server = start_webserver();

    while (1) {
            if (accel_speed_has_to_be_switch == 1) {
                    ESP_LOGI(TAG, "accel will be switch\n");
                accel_speed_has_to_be_switch = 0;
                i2c_example_master_mpu6050_init(I2C_EXAMPLE_MASTER_NUM);
            }
            //ESP_LOGW(TAG, "Accel errors: %d", ret_hw_timer_callback_error);
            //ESP_LOGW(TAG, "Accel OK: %d", ret_hw_timer_callback_OK);
            vTaskDelay(500 / portTICK_RATE_MS);
    }

}
