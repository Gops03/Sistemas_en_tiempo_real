#include <stdio.h>
#include "driver/ledc.h"
#include "esp_err.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include <string.h>
#include "driver/adc.h"
#include <math.h>
#include "freertos/event_groups.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "nvs_flash.h"
#include "esp_netif.h"
#include "driver/gpio.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include "lwip/api.h"
#include "lwip/netdb.h"
#include <esp_http_server.h>

#define LEDC_TIMER              LEDC_TIMER_0
#define LEDC_MODE               LEDC_LOW_SPEED_MODE

#define LEDC_OUTPUT_IO          (2)  // Define el GPIO de salida para el LED
#define BUTTON_PIN              (19) // Cambia esto al pin GPIO donde está conectado tu botón

#define LEDC_CHANNEL            LEDC_CHANNEL_0
#define LEDC_DUTY_RES           LEDC_TIMER_14_BIT
#define LEDC_FREQUENCY          (3000)
#define MAX_PRESSES              3

static const char *TAG = "espressif";
int led_state = 0;
int duty_cycle = 100; // Duty cycle inicial al 100%

#define EXAMPLE_ESP_WIFI_SSID "ej"
#define EXAMPLE_ESP_WIFI_PASS "delunoalocho"
#define EXAMPLE_ESP_MAXIMUM_RETRY 5

static EventGroupHandle_t s_wifi_event_group;

#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT BIT1

static int s_retry_num = 0;

void event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START)
    {
        esp_wifi_connect();
    }
    else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED)
    {
        if (s_retry_num < EXAMPLE_ESP_MAXIMUM_RETRY)
        {
            esp_wifi_connect();
            s_retry_num++;
            ESP_LOGI(TAG, "Reintentando conectar a la red Wi-Fi...");
        }
        else
        {
            xEventGroupSetBits(s_wifi_event_group, WIFI_FAIL_BIT);
        }
        ESP_LOGI(TAG, "Fallo en la conexión a la red Wi-Fi");
    }
    else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP)
    {
        ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
        ESP_LOGI(TAG, "Obtenida dirección IP: " IPSTR, IP2STR(&event->ip_info.ip));
        s_retry_num = 0;
        xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
    }
}

void connect_wifi(void)
{
    s_wifi_event_group = xEventGroupCreate();

    ESP_ERROR_CHECK(esp_netif_init());

    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    esp_event_handler_instance_t instance_any_id;
    esp_event_handler_instance_t instance_got_ip;
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                        ESP_EVENT_ANY_ID,
                                                        &event_handler,
                                                        NULL,
                                                        &instance_any_id));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT,
                                                        IP_EVENT_STA_GOT_IP,
                                                        &event_handler,
                                                        NULL,
                                                        &instance_got_ip));

    wifi_config_t wifi_config = {
        .sta = {
            .ssid = EXAMPLE_ESP_WIFI_SSID,
            .password = EXAMPLE_ESP_WIFI_PASS,
            .threshold.authmode = WIFI_AUTH_WPA2_PSK,
        },
    };
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(TAG, "Inicializando conexión a la red Wi-Fi...");

    EventBits_t bits = xEventGroupWaitBits(s_wifi_event_group,
                                           WIFI_CONNECTED_BIT | WIFI_FAIL_BIT,
                                           pdFALSE,
                                           pdFALSE,
                                           portMAX_DELAY);

    if (bits & WIFI_CONNECTED_BIT)
    {
        ESP_LOGI(TAG, "Conexión exitosa a la red Wi-Fi");
    }
    else if (bits & WIFI_FAIL_BIT)
    {
        ESP_LOGI(TAG, "Fallo en la conexión a la red Wi-Fi");
    }
    else
    {
        ESP_LOGE(TAG, "Evento inesperado");
    }

    vEventGroupDelete(s_wifi_event_group);
}

char html_page[] = "<!DOCTYPE html><html><head><style type=\"text/css\">html {  font-family: Arial;  display: inline-block;  margin: 0px auto;  text-align: center;}h1{  color: #070812;  padding: 2vh;}.button {  display: inline-block;  background-color: #b30000;  border: none;  border-radius: 4px;  color: white;  padding: 16px 40px;  text-decoration: none;  font-size: 30px;  margin: 2px;  cursor: pointer;}.button2 {  background-color: #364cf4;}.content {   padding: 50px;}.card-grid {  max-width: 800px;  margin: 0 auto;  display: grid;  grid-gap: 2rem;  grid-template-columns: repeat(auto-fit, minmax(200px, 1fr));}.card {  background-color: white;  box-shadow: 2px 2px 12px 1px rgba(140,140,140,.5);}.card-title {  font-size: 1.2rem;  font-weight: bold;  color: #034078}</style>  <title>ESP32 LED Control</title>  <meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">  <link rel=\"icon\" href=\"data:,\">  <link rel=\"stylesheet\" href=\"https://use.fontawesome.com/releases/v5.7.2/css/all.css\"    integrity=\"sha384-fnmOCqbTlWIlj8LyTjo7mOUStjsKC4pOpQbqyi7RrhN7udi9RwhKkMHpvLbHG9Sr\" crossorigin=\"anonymous\">  <link rel=\"stylesheet\" type=\"text/css\"></head><body>  <h2>ESP32 LED Control</h2>  <div class=\"content\">    <div class=\"card-grid\">      <div class=\"card\">        <p><i class=\"fas fa-lightbulb fa-2x\" style=\"color:#c81919;\"></i>     <strong>LED Control</strong></p>        <p>LED state: <strong> ON</strong></
<p>LED Duty Cycle: <span id=\"dutyCycle\">100%</span></p>
        <input type=\"range\" id=\"dutyCycleRange\" min=\"0\" max=\"100\" step=\"1\" value=\"100\">
        <br><button class=\"button button2\" onclick=\"updateLED()\">Actualizar LED</button>
      </div>
    </div>
  </div>
  <script>
    var dutyCycleRange = document.getElementById('dutyCycleRange');
    var dutyCycle = document.getElementById('dutyCycle');

    // Mostrar el valor inicial del rango de duty cycle
    dutyCycle.innerHTML = dutyCycleRange.value + "%";

    // Actualizar el valor del duty cycle cuando se ajusta el rango
    dutyCycleRange.oninput = function() {
      dutyCycle.innerHTML = this.value + "%";
    };

    function updateLED() {
      var newDutyCycle = dutyCycleRange.value;
      var xhr = new XMLHttpRequest();
      xhr.open(\"GET\", \"/led?duty=\" + newDutyCycle, true);
      xhr.send();
    }
  </script>
</body>
</html>"

esp_err_t led_control_get_handler(httpd_req_t *req)
{
    char *buf;
    size_t buf_len;
    uint8_t new_duty;
    char param[32];

    // Recuperar el valor del parámetro 'duty' de la solicitud
    if (httpd_query_key_value(req->uri, "duty", param, sizeof(param)) == ESP_OK)
    {
        new_duty = atoi(param);
        if (new_duty >= 0 && new_duty <= 100)
        {
            duty_cycle = new_duty;
            ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, (duty_cycle * ((1 << LEDC_DUTY_RES) - 1)) / 100);
            ledc_update_duty(LEDC_MODE, LEDC_CHANNEL);
            httpd_resp_sendstr(req, "LED actualizado.");
        }
        else
        {
            httpd_resp_sendstr(req, "Valor de duty no válido. Debe estar entre 0 y 100.");
        }
    }
    else
    {
        httpd_resp_sendstr(req, "Parámetro 'duty' no especificado.");
    }
    return ESP_OK;
}

httpd_uri_t led_control = {
    .uri = "/led",
    .method = HTTP_GET,
    .handler = led_control_get_handler,
    .user_ctx = NULL};

// Registra el controlador de la URI para el LED
httpd_register_uri_handler(server, &led_control);
