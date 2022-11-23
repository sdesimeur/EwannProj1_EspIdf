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

#include <esp_http_server.h>

#include "common.h"

static const char *TAG="HTTP";
extern httpd_handle_t server;

/* An HTTP GET handler */
char resp_str[256];

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

esp_err_t rpc_getspeed_get_handler(httpd_req_t *req)
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

    if (get_speed_abs() < 0) {
        sprintf(resp_str, "-%d.%09d", (unsigned int)(-get_speed_abs()), ((unsigned int)(-get_speed_abs() * 1000000000)) % 1000000000);
    } else {
        sprintf(resp_str, "%d.%09d", (unsigned int)(get_speed_abs()), ((unsigned int)(get_speed_abs() * 1000000000)) % 1000000000);
    }
    httpd_resp_send(req, resp_str, strlen(resp_str));

    /* After sending the HTTP response the old HTTP request
     * headers are lost. Check if HTTP request headers can be read now. */
    if (httpd_req_get_hdr_value_len(req, "Host") == 0) {
        //ESP_LOGI(TAG, "Request headers lost");
    }
    return ESP_OK;
}

httpd_uri_t rpc_getspeed = {
    .uri       = "/rpc/getSpeed",
    .method    = HTTP_GET,
    .handler   = rpc_getspeed_get_handler,
    /* Let's pass response string in user
     * context to demonstrate it's usage */
    .user_ctx = NULL
};

esp_err_t rpc_getmagnetfield_get_handler(httpd_req_t *req)
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
    double val = get_magnetofield();
    /* Read URL query string length and allocate memory for length + 1,
     * extra byte for null termination */
    buf_len = httpd_req_get_url_query_len(req) + 1;
    if (buf_len > 1) {
        buf = malloc(buf_len);
        if (httpd_req_get_url_query_str(req, buf, buf_len) == ESP_OK) {
            char param[32];
            // Get value of expected key from query string
            if (httpd_query_key_value(buf, "w", param, sizeof(param)) == ESP_OK) {
                switch (param[0]){
                    case 'm':
                        val = get_magnetofieldMin();
                        break;
                    case 'M':
                        val = get_magnetofieldMax();
                        break;
                    case '2':
                        val = get_magnetofieldMean();
                        break;
                    case 'n':
                    default:
                        val = get_magnetofield();
                        break;
                }
                //ESP_LOGI(TAG, "Found URL query => %s = %s", buf, param);
            }
        }
        free(buf);
    }
    
    
    if (val < 0) {
        sprintf(resp_str, "-%d.%09d", (unsigned int)(-val), ((unsigned int)(-val * 1000000000)) % 1000000000);
    } else {
        sprintf(resp_str, "%d.%09d", (unsigned int)(val), ((unsigned int)(val * 1000000000)) % 1000000000);
    }
    httpd_resp_send(req, resp_str, strlen(resp_str));

    /* After sending the HTTP response the old HTTP request
     * headers are lost. Check if HTTP request headers can be read now. */
    if (httpd_req_get_hdr_value_len(req, "Host") == 0) {
        //ESP_LOGI(TAG, "Request headers lost");
    }
    return ESP_OK;
}

httpd_uri_t rpc_getmagnetfield = {
    .uri       = "/rpc/getMagnetField",
    .method    = HTTP_GET,
    .handler   = rpc_getmagnetfield_get_handler,
    /* Let's pass response string in user
     * context to demonstrate it's usage */
    .user_ctx = NULL
};

esp_err_t rpc_getaccel_get_handler(httpd_req_t *req)
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
    if (get_accel_abs() < 0) {
        sprintf(resp_str, "-%d.%09d", (unsigned int)(-get_accel_abs()), ((unsigned int)(-get_accel_abs() * 1000000000)) % 1000000000);
    } else {
        sprintf(resp_str, "%d.%09d", (unsigned int)(get_accel_abs()), ((unsigned int)(get_accel_abs() * 1000000000)) % 1000000000);
    }
    httpd_resp_send(req, resp_str, strlen(resp_str));

    /* After sending the HTTP response the old HTTP request
     * headers are lost. Check if HTTP request headers can be read now. */
    if (httpd_req_get_hdr_value_len(req, "Host") == 0) {
        //ESP_LOGI(TAG, "Request headers lost");
    }
    return ESP_OK;
}

httpd_uri_t rpc_getaccel = {
    .uri       = "/rpc/getAccel",
    .method    = HTTP_GET,
    .handler   = rpc_getaccel_get_handler,
    /* Let's pass response string in user
     * context to demonstrate it's usage */
    .user_ctx = NULL
};

esp_err_t rpc_getrawaccel_get_handler(httpd_req_t *req)
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
    XYZT *a = &accelN;

    /* Read URL query string length and allocate memory for length + 1,
     * extra byte for null termination */
    buf_len = httpd_req_get_url_query_len(req) + 1;
    if (buf_len > 1) {
        buf = malloc(buf_len);
        if (httpd_req_get_url_query_str(req, buf, buf_len) == ESP_OK) {
            char param[32];
            // Get value of expected key from query string
            if (httpd_query_key_value(buf, "w", param, sizeof(param)) == ESP_OK) {
                switch (param[0]){
                    case 'i':
                        a = &accelI;
                        break;
                    case 'n':
                    default:
                        a = &accelN;
                        break;
                }
                //ESP_LOGI(TAG, "Found URL query => %s = %s", buf, param);
            }
        }
        free(buf);
    }

    sprintf(resp_str, "{ %c%d.%09d, %c%d.%09d, %c%d.%09d }",
            (a->x<0)?'-':'+',
            (unsigned int)(((a->x<0)?-1:1) * a->x), ((unsigned int)(((a->x<0)?-1:1) * a->x * 1000000000)) % 1000000000,
            (a->y<0)?'-':'+',
            (unsigned int)(((a->y<0)?-1:1) * a->y), ((unsigned int)(((a->y<0)?-1:1) * a->y * 1000000000)) % 1000000000,
            (a->z<0)?'-':'+',
            (unsigned int)(((a->z<0)?-1:1) * a->z), ((unsigned int)(((a->z<0)?-1:1) * a->z * 1000000000)) % 1000000000);
    httpd_resp_send(req, resp_str, strlen(resp_str));

    /* After sending the HTTP response the old HTTP request
     * headers are lost. Check if HTTP request headers can be read now. */
    if (httpd_req_get_hdr_value_len(req, "Host") == 0) {
        //ESP_LOGI(TAG, "Request headers lost");
    }
    return ESP_OK;
}

httpd_uri_t rpc_getrawaccel = {
    .uri       = "/rpc/getRawAccel",
    .method    = HTTP_GET,
    .handler   = rpc_getrawaccel_get_handler,
    /* Let's pass response string in user
     * context to demonstrate it's usage */
    .user_ctx = NULL
};


esp_err_t rpc_getcounter_handler(httpd_req_t *req)
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

    sprintf(resp_str, "{ %d, %d.%09d }", counter
            (unsigned int)time_in_s, ((unsigned int)(time_in_s * 1000000000)) % 1000000000);
    httpd_resp_send(req, resp_str, strlen(resp_str));

    /* After sending the HTTP response the old HTTP request
     * headers are lost. Check if HTTP request headers can be read now. */
    if (httpd_req_get_hdr_value_len(req, "Host") == 0) {
        //ESP_LOGI(TAG, "Request headers lost");
    }
    return ESP_OK;
}


httpd_uri_t rpc_getcounter = {
    .uri       = "/rpc/getCounter",
    .method    = HTTP_GET,
    .handler   = rpc_getcounter_handler,
    /* Let's pass response string in user
     * context to demonstrate it's usage */
    .user_ctx = NULL
};


esp_err_t rpc_initaccelspeed_get_handler(httpd_req_t *req)
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
    init_accel_speed();
    const char * resp_str1 = "1";
    httpd_resp_send(req, resp_str1, strlen(resp_str1));

    /* After sending the HTTP response the old HTTP request
     * headers are lost. Check if HTTP request headers can be read now. */
    if (httpd_req_get_hdr_value_len(req, "Host") == 0) {
        //ESP_LOGI(TAG, "Request headers lost");
    }
    return ESP_OK;
}

httpd_uri_t rpc_initaccelspeed = {
    .uri       = "/rpc/initAccelSpeed",
    .method    = HTTP_GET,
    .handler   = rpc_initaccelspeed_get_handler,
    /* Let's pass response string in user
     * context to demonstrate it's usage */
    .user_ctx = NULL
};

esp_err_t rpc_configmagneto_get_handler(httpd_req_t *req)
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
            char param[32];
            // Get value of expected key from query string
            if (httpd_query_key_value(buf, "config", param, sizeof(param)) == ESP_OK) {
                magnetoFieldInit = (param[0] == '0')?0:1;
                //ESP_LOGI(TAG, "Found URL query => %s = %s", buf, param);
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

httpd_uri_t rpc_configmagneto = {
    .uri       = "/rpc/configMagneto",
    .method    = HTTP_GET,
    .handler   = rpc_configmagneto_get_handler,
    /* Let's pass response string in user
     * context to demonstrate it's usage */
    .user_ctx = NULL
};
/* An HTTP POST handler */
esp_err_t echo_post_handler(httpd_req_t *req)
{
    char buf[100];
    int ret, remaining = req->content_len;

    while (remaining > 0) {
        /* Read the data for the request */
        if ((ret = httpd_req_recv(req, buf,
                        MIN(remaining, sizeof(buf)))) <= 0) {
            if (ret == HTTPD_SOCK_ERR_TIMEOUT) {
                /* Retry receiving if timeout occurred */
                continue;
            }
            return ESP_FAIL;
        }

        /* Send back the same data */
        httpd_resp_send_chunk(req, buf, ret);
        remaining -= ret;

        /* Log data received */
        //ESP_LOGI(TAG, "=========== RECEIVED DATA ==========");
        //ESP_LOGI(TAG, "%.*s", ret, buf);
        //ESP_LOGI(TAG, "====================================");
    }

    // End response
    httpd_resp_send_chunk(req, NULL, 0);
    return ESP_OK;
}

httpd_uri_t echo = {
    .uri       = "/echo",
    .method    = HTTP_POST,
    .handler   = echo_post_handler,
    .user_ctx  = NULL
};

/* An HTTP PUT handler. This demonstrates realtime
 * registration and deregistration of URI handlers
 */
esp_err_t ctrl_put_handler(httpd_req_t *req)
{
    char buf;
    int ret;

    if ((ret = httpd_req_recv(req, &buf, 1)) <= 0) {
        if (ret == HTTPD_SOCK_ERR_TIMEOUT) {
            httpd_resp_send_408(req);
        }
        return ESP_FAIL;
    }

    if (buf == '0') {
        /* Handler can be unregistered using the uri string */
        //ESP_LOGI(TAG, "Unregistering /hello and /echo URIs");
        httpd_unregister_uri(req->handle, "/hello");
        httpd_unregister_uri(req->handle, "/echo");
    }
    else {
        //ESP_LOGI(TAG, "Registering /hello and /echo URIs");
        httpd_register_uri_handler(req->handle, &root_index);
        httpd_register_uri_handler(req->handle, &echo);
    }

    /* Respond with empty body */
    httpd_resp_send(req, NULL, 0);
    return ESP_OK;
}

httpd_uri_t ctrl = {
    .uri       = "/ctrl",
    .method    = HTTP_PUT,
    .handler   = ctrl_put_handler,
    .user_ctx  = NULL
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
        httpd_register_uri_handler(server, &rpc_getspeed);
        httpd_register_uri_handler(server, &rpc_getcounter);
        httpd_register_uri_handler(server, &rpc_getmagnetfield);
        httpd_register_uri_handler(server, &rpc_getaccel);
        httpd_register_uri_handler(server, &rpc_getrawaccel);
        httpd_register_uri_handler(server, &rpc_initaccelspeed);
        httpd_register_uri_handler(server, &rpc_configmagneto);
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
