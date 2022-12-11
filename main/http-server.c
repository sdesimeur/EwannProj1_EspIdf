/* Simple HTTP Server Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <sys/param.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "esp_event.h"
//#include "protocol_examples_common.h"
#include "nvs.h"
#include "index.h"
#include "nvs_flash.h"

#include "driver/gpio.h"
#include <esp_http_server.h>

#include "common.h"

static const char *TAG="HTTP";
extern httpd_handle_t server;

/* An HTTP GET handler */
char resp_str[1024];

esp_err_t index_get_handler(httpd_req_t *req)
{
    char*  buf;
    size_t buf_len;

    /* Get header value string length and allocate memory for length + 1,
     * extra byte for null termination */
    buf_len = httpd_req_get_hdr_value_len(req, "Host") + 1;
    if (buf_len > 1) {
        buf = malloc(buf_len);
        /* Copy null terminated value string into buffer */
        if (httpd_req_get_hdr_value_str(req, "Host", buf, buf_len) == ESP_OK) {
            //ESP_LOGI(TAG, "Found header => Host: %s", buf);
        }
        free(buf);
    }

    /*
    buf_len = httpd_req_get_hdr_value_len(req, "Test-Header-2") + 1;
    if (buf_len > 1) {
        buf = malloc(buf_len);
        if (httpd_req_get_hdr_value_str(req, "Test-Header-2", buf, buf_len) == ESP_OK) {
            //ESP_LOGI(TAG, "Found header => Test-Header-2: %s", buf);
        }
        free(buf);
    }

    buf_len = httpd_req_get_hdr_value_len(req, "Test-Header-1") + 1;
    if (buf_len > 1) {
        buf = malloc(buf_len);
        if (httpd_req_get_hdr_value_str(req, "Test-Header-1", buf, buf_len) == ESP_OK) {
            //ESP_LOGI(TAG, "Found header => Test-Header-1: %s", buf);
        }
        free(buf);
    }
    */
    /* Read URL query string length and allocate memory for length + 1,
     * extra byte for null termination */
    /*
    buf_len = httpd_req_get_url_query_len(req) + 1;
    if (buf_len > 1) {
        buf = malloc(buf_len);
        if (httpd_req_get_url_query_str(req, buf, buf_len) == ESP_OK) {
            //ESP_LOGI(TAG, "Found URL query => %s", buf);
            char param[32];
            // Get value of expected key from query string
            if (httpd_query_key_value(buf, "query1", param, sizeof(param)) == ESP_OK) {
                //ESP_LOGI(TAG, "Found URL query parameter => query1=%s", param);
            }
            if (httpd_query_key_value(buf, "query3", param, sizeof(param)) == ESP_OK) {
                //ESP_LOGI(TAG, "Found URL query parameter => query3=%s", param);
            }
            if (httpd_query_key_value(buf, "query2", param, sizeof(param)) == ESP_OK) {
                //ESP_LOGI(TAG, "Found URL query parameter => query2=%s", param);
            }
        }
        free(buf);
    }
    */

    /* Set some custom headers */
    //httpd_resp_set_hdr(req, "Custom-Header-1", "Custom-Value-1");
    //httpd_resp_set_hdr(req, "Custom-Header-2", "Custom-Value-2");

    /* Send response with custom headers and body set as the
     * string passed in user context*/
    const char* resp_str = (const char*) req->user_ctx;
    httpd_resp_send(req, resp_str, strlen(resp_str));

    /* After sending the HTTP response the old HTTP request
     * headers are lost. Check if HTTP request headers can be read now. */
    if (httpd_req_get_hdr_value_len(req, "Host") == 0) {
        //ESP_LOGI(TAG, "Request headers lost");
    }
    return ESP_OK;
}

httpd_uri_t root_index = {
    .uri       = "/",
    .method    = HTTP_GET,
    .handler   = index_get_handler,
    /* Let's pass response string in user
     * context to demonstrate it's usage */
    .user_ctx = INDEX_HTML
};

esp_err_t get_all_values_handler(httpd_req_t *req)
{
    char*  buf;
    size_t buf_len;

    /* Get header value string length and allocate memory for length + 1,
     * extra byte for null termination */
    buf_len = httpd_req_get_hdr_value_len(req, "Host") + 1;
    if (buf_len > 1) {
        buf = malloc(buf_len);
        /* Copy null terminated value string into buffer */
        if (httpd_req_get_hdr_value_str(req, "Host", buf, buf_len) == ESP_OK) {
            //ESP_LOGI(TAG, "Found header => Host: %s", buf);
        }
        free(buf);
    }
    XYZT *a;
    a = &accelN;
    XYZT *b;
    b = &accelI;
    double c = get_XYZT_abs(&accelN);
    double d = get_XYZT_abs(&speed);
    double e = accelMaxWG;
    double f = get_XYZT_abs(&accelWG);
    
    double g = magnetoField;
    double h = magnetoFieldMean;
    double i = magnetoFieldMax;
    double j = magnetoFieldMin;
    
    sprintf(resp_str, "{ "
                " \"counter\" : { \"val\" : %d , \"duration\" : %d.%09d , \"freq\" : %d.%09d } ,"
                " \"accel\" : { "
                    "\"MaxWG\" : %d.%09d , "
                    "\"WG\" : %d.%09d , "
                    "\"speed\" : %d.%09d , "
                    "\"val\" : %d.%09d , "
                    "\"N\" : [ %c%d.%09d, %c%d.%09d, %c%d.%09d ], "
                    "\"I\" : [ %c%d.%09d, %c%d.%09d, %c%d.%09d ] "
                " } ,"
                " \"magnet\" : { "
                    "\"val\" : %c%d.%09d , "
                    "\"Mean\" : %c%d.%09d , "
                    "\"Max\" : %c%d.%09d , "
                    "\"Min\" : %c%d.%09d"
                " } "
            " }",
     // counter
            counter,
            (unsigned int)time_in_s, ((unsigned int)(time_in_s * 1000000000)) % 1000000000,
            (unsigned int)counter_last_freq, ((unsigned int)(counter_last_freq * 1000000000)) % 1000000000,
     // accel       
            (unsigned int)e, ((unsigned int)(e * 1000000000)) % 1000000000,
            (unsigned int)f, ((unsigned int)(f * 1000000000)) % 1000000000,
            (unsigned int)d, ((unsigned int)(d * 1000000000)) % 1000000000,
            (unsigned int)c, ((unsigned int)(c * 1000000000)) % 1000000000,
            
            (a->x<0)?'-':' ',
            (unsigned int)(((a->x<0)?-1:1) * a->x), ((unsigned int)(((a->x<0)?-1:1) * a->x * 1000000000)) % 1000000000,
            (a->y<0)?'-':' ',
            (unsigned int)(((a->y<0)?-1:1) * a->y), ((unsigned int)(((a->y<0)?-1:1) * a->y * 1000000000)) % 1000000000,
            (a->z<0)?'-':' ',
            (unsigned int)(((a->z<0)?-1:1) * a->z), ((unsigned int)(((a->z<0)?-1:1) * a->z * 1000000000)) % 1000000000,
            
            (b->x<0)?'-':' ',
            (unsigned int)(((b->x<0)?-1:1) * b->x), ((unsigned int)(((b->x<0)?-1:1) * b->x * 1000000000)) % 1000000000,
            (b->y<0)?'-':' ',
            (unsigned int)(((b->y<0)?-1:1) * b->y), ((unsigned int)(((b->y<0)?-1:1) * b->y * 1000000000)) % 1000000000,
            (b->z<0)?'-':' ',
            (unsigned int)(((b->z<0)?-1:1) * b->z), ((unsigned int)(((b->z<0)?-1:1) * b->z * 1000000000)) % 1000000000,
    // magnet
            (g<0)?'-':' ',
            (unsigned int)(((g<0)?-1:1) * g), ((unsigned int)(((g<0)?-1:1) * g * 1000000000)) % 1000000000,
            (h<0)?'-':' ',
            (unsigned int)(((h<0)?-1:1) * h), ((unsigned int)(((h<0)?-1:1) * h * 1000000000)) % 1000000000,
            (i<0)?'-':' ',
            (unsigned int)(((i<0)?-1:1) * i), ((unsigned int)(((i<0)?-1:1) * i * 1000000000)) % 1000000000,
            (j<0)?'-':' ',
            (unsigned int)(((j<0)?-1:1) * j), ((unsigned int)(((j<0)?-1:1) * j * 1000000000)) % 1000000000

            );
    httpd_resp_send(req, resp_str, strlen(resp_str));

    /* After sending the HTTP response the old HTTP request
     * headers are lost. Check if HTTP request headers can be read now. */
    if (httpd_req_get_hdr_value_len(req, "Host") == 0) {
        //ESP_LOGI(TAG, "Request headers lost");
    }
    return ESP_OK;
}

httpd_uri_t rpc_getter = {
    .uri       = "/getAllValues",
    .method    = HTTP_GET,
    .handler   = get_all_values_handler,
    /* Let's pass response string in user
     * context to demonstrate it's usage */
    .user_ctx = NULL
};

int gpio_on[17] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
esp_err_t rpc_command_handler(httpd_req_t *req)
{
    char*  buf;
    size_t buf_len;

    /* Get header value string length and allocate memory for length + 1,
     * extra byte for null termination */
    buf_len = httpd_req_get_hdr_value_len(req, "Host") + 1;
    if (buf_len > 1) {
        buf = malloc(buf_len);
        /* Copy null terminated value string into buffer */
        if (httpd_req_get_hdr_value_str(req, "Host", buf, buf_len) == ESP_OK) {
            //ESP_LOGI(TAG, "Found header => Host: %s", buf);
        }
        free(buf);
    }
    /* Read URL query string length and allocate memory for length + 1,
     * extra byte for null termination */
    buf_len = httpd_req_get_url_query_len(req) + 1;
    if (buf_len > 1) {
        buf = malloc(buf_len);
        if (httpd_req_get_url_query_str(req, buf, buf_len) == ESP_OK) {
            char param[16];
            // Get value of expected key from query string
            if (httpd_query_key_value(buf, "initMagnetoField", param, sizeof(param)) == ESP_OK) {
                magnetoFieldInit = (param[0] == '0')?0:1;
                //ESP_LOGI(TAG, "Found URL query => %s = %s", buf, param);
            } else if (httpd_query_key_value(buf, "initAccelSpeed", param, sizeof(param)) == ESP_OK) {
                init_accel_speed();
            } else if (httpd_query_key_value(buf, "startAccelSpeed", param, sizeof(param)) == ESP_OK) {
                accel_speed_has_to_be_switch = 1;
                ESP_LOGW(TAG, "start accel speed");
            } else if (httpd_query_key_value(buf, "gpio", param, sizeof(param)) == ESP_OK) {
                int gpio_num = 0;
                sscanf(param, "%d", &gpio_num);
                char param1[3];
                if (httpd_query_key_value(buf, "level", param1, sizeof(param1)) == ESP_OK) {
                    int new = (param1[0] == '0')?0:1;
                    if (gpio_on[gpio_num] != new) {
                        gpio_set_level(gpio_num, new);
                        gpio_on[gpio_num] = new;
                    }
                }
            }
        }
        free(buf);
    }
    
    const char * resp_str1 = "1";
    httpd_resp_send(req, resp_str1, strlen(resp_str1));

    /* After sending the HTTP response the old HTTP request
     * headers are lost. Check if HTTP request headers can be read now. */
    if (httpd_req_get_hdr_value_len(req, "Host") == 0) {
        //ESP_LOGI(TAG, "Request headers lost");
    }
    return ESP_OK;
}

httpd_uri_t rpc_command = {
    .uri       = "/command",
    .method    = HTTP_GET,
    .handler   = rpc_command_handler,
    /* Let's pass response string in user
     * context to demonstrate it's usage */
    .user_ctx = NULL
};

httpd_handle_t start_webserver(void)
{
    httpd_handle_t server = NULL;
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();

    // Start the httpd server
    //ESP_LOGI(TAG, "Starting server on port: '%d'", config.server_port);
    if (httpd_start(&server, &config) == ESP_OK) {
        // Set URI handlers
        //ESP_LOGI(TAG, "Registering URI handlers");
        httpd_register_uri_handler(server, &root_index);
        httpd_register_uri_handler(server, &rpc_getter);
        httpd_register_uri_handler(server, &rpc_command);
        //httpd_register_uri_handler(server, &echo);
        //httpd_register_uri_handler(server, &ctrl);
        return server;
    }

    //ESP_LOGI(TAG, "Error starting server!");
    return NULL;
}

void stop_webserver(httpd_handle_t server)
{
    // Stop the httpd server
    httpd_stop(server);
}


static void disconnect_handler(void* arg, esp_event_base_t event_base, 
                               int32_t event_id, void* event_data)
{
    httpd_handle_t* server = (httpd_handle_t*) arg;
    if (*server) {
        //ESP_LOGI(TAG, "Stopping webserver");
        stop_webserver(*server);
        *server = NULL;
    }
}

static void connect_handler(void* arg, esp_event_base_t event_base, 
                            int32_t event_id, void* event_data)
{
    httpd_handle_t* server = (httpd_handle_t*) arg;
    if (*server == NULL) {
        //ESP_LOGI(TAG, "Starting webserver");
        *server = start_webserver();
    }
}
/*
void app_main()
{
    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());


    ESP_ERROR_CHECK(example_connect());

    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &connect_handler, &server));
    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, WIFI_EVENT_STA_DISCONNECTED, &disconnect_handler, &server));

    server = start_webserver();
}
*/
