// TCP Server with WiFi connection communication via Socket by Modbus TCP

#include <stdio.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/timers.h"
#include "freertos/event_groups.h"
#include "freertos/queue.h"

#include "esp_wifi.h"
#include "esp_log.h"
#include "esp_check.h"
#include "esp_mac.h"
#include "esp_eth.h"
#include "esp_netif.h"
#include "esp_http_server.h"
#include "esp_http_client.h"
#include "esp_event.h"
#include "esp_system.h"
#include "esp_err.h"

#include "lwip/inet.h"
#include "lwip/netdb.h"
#include "lwip/sockets.h"
#include "lwip/ip_addr.h"

#include "nvs_flash.h"
#include "ping/ping_sock.h"
#include "driver/gpio.h"

#include "my_data.h"
#define PORT 502

static const char *TAG = "TCP_SOCKET";

static void tcp_server_task(void *pvParameters)
{
    // Variables definitions
    char addr_str[128];
    char rx_buffer[128];
    int tx_buffer_length = 12;
    char tx_buffer[tx_buffer_length];
    int keepAlive = 1;
    int keepIdle = 5;
    int keepInterval = 5;
    int keepCount = 3;
    struct sockaddr_storage dest_addr;

    int counter = 0;

    // server address definition
    struct sockaddr_in *dest_addr_ip4 = (struct sockaddr_in *)&dest_addr;
    dest_addr_ip4->sin_addr.s_addr = htonl(INADDR_ANY);
    dest_addr_ip4->sin_family = AF_INET;
    dest_addr_ip4->sin_port = htons(PORT);

    // Open socket
    int listen_sock = socket(AF_INET, SOCK_STREAM, 0); // 0 for TCP Protocol
    int opt = 1;
    setsockopt(listen_sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    ESP_LOGI(TAG, "Socket created");

    bind(listen_sock, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
    ESP_LOGI(TAG, "Socket bound, port %d", PORT);

    // Listen to the socket
    listen(listen_sock, 1);

    while (1)
    {
        ESP_LOGI(TAG, "Socket listening");

        struct sockaddr_storage source_addr; // Large enough for both IPv4 or IPv6
        socklen_t addr_len = sizeof(source_addr);

        // Accept socket
        int sock = accept(listen_sock, (struct sockaddr *)&source_addr, &addr_len);
        if (sock < 0)
        {
            ESP_LOGE(TAG, "Unable to accept connection: errno %d", errno);
            break;
        }

        // Set tcp keepalive option
        setsockopt(sock, SOL_SOCKET, SO_KEEPALIVE, &keepAlive, sizeof(int));
        setsockopt(sock, IPPROTO_TCP, TCP_KEEPIDLE, &keepIdle, sizeof(int));
        setsockopt(sock, IPPROTO_TCP, TCP_KEEPINTVL, &keepInterval, sizeof(int));
        setsockopt(sock, IPPROTO_TCP, TCP_KEEPCNT, &keepCount, sizeof(int));

        // Convert ip address to string
        if (source_addr.ss_family == PF_INET)
        {
            inet_ntoa_r(((struct sockaddr_in *)&source_addr)->sin_addr, addr_str, sizeof(addr_str) - 1);
        }

        ESP_LOGI(TAG, "Socket accepted ip address: %s", addr_str);

        // Modbus TCP data reception
        ssize_t bytes_received = recv(sock, rx_buffer, sizeof(rx_buffer), 0);
        printf("ESP32 RECEIVED: %d\n", bytes_received);

        printf("\nMBAP Header - Modbus Application Protocol:\n");           // Start Modbus Application Protocol (MBAP)
        printf("Transaction ID:                     %02x\n", rx_buffer[0]); 
        printf("Transaction ID:                     %02x\n", rx_buffer[1]);
        printf("Protocol ID:                        %02x\n", rx_buffer[2]); // 0x0000 for Modbus TCP
        printf("Protocol ID:                        %02x\n", rx_buffer[3]); // 0x0000 for Modbus TCP
        printf("Length of the remaining message:    %02x\n", rx_buffer[4]);
        printf("Length of the remaining message:    %02x\n", rx_buffer[5]);
        printf("Unit Identifier:                    %02x\n", rx_buffer[6]);

        printf("\nPDU - Protocol Data Unit:\n");                            // Start Protocol Data Unit (PDU)
        printf("Function Code:                      %02x\n", rx_buffer[7]); // 0x10: Write Multiple Registers
        printf("Starting Address:                   %02x\n", rx_buffer[8]);
        printf("Starting Address:                   %02x\n", rx_buffer[9]);
        printf("Quantity of Registers               %02x\n", rx_buffer[10]); // Quantity of Coils to Read
        printf("Quantity of Registers               %02x\n", rx_buffer[11]); // Quantity of Coils to Read
        printf("Values to be written:               %02x\n", rx_buffer[12]); // Values to be written
        printf("Data:                               %02x\n", rx_buffer[13]);  
        printf("Data:                               %02x\n", rx_buffer[14]);
       
        // Server response message for Modbus TCP - tx_buffer build
        tx_buffer[0] = rx_buffer[0];
        tx_buffer[1] = rx_buffer[1];
        tx_buffer[2] = rx_buffer[2];
        tx_buffer[3] = rx_buffer[3];
        tx_buffer[4] = rx_buffer[4];
        tx_buffer[5] = 0x0006;
        tx_buffer[6] = rx_buffer[6];
        tx_buffer[7] = rx_buffer[7];
        tx_buffer[8] = rx_buffer[8];
        tx_buffer[9] = rx_buffer[9];
        tx_buffer[10] = rx_buffer[10];
        tx_buffer[11] = rx_buffer[11];

        printf("\nESP32 SEND: %d\n", tx_buffer_length);
        printf("\nMBAP Header - Modbus Application Protocol:\n");           // Start Modbus Application Protocol (MBAP)
        printf("Transaction ID:                     %02x\n", tx_buffer[0]); 
        printf("Transaction ID:                     %02x\n", tx_buffer[1]);
        printf("Protocol ID:                        %02x\n", tx_buffer[2]); // 0x0000 for Modbus TCP
        printf("Protocol ID:                        %02x\n", tx_buffer[3]); // 0x0000 for Modbus TCP
        printf("Length of the remaining message:    %02x\n", tx_buffer[4]);
        printf("Length of the remaining message:    %02x\n", tx_buffer[5]);
        printf("Unit Identifier:                    %02x\n", tx_buffer[6]);

        printf("\nPDU - Protocol Data Unit:\n");                             // Start Protocol Data Unit (PDU)
        printf("Function Code:                      %02x\n", tx_buffer[7]);
        printf("Starting Address:                   %02x\n", tx_buffer[8]);
        printf("Starting Address:                   %02x\n", tx_buffer[9]);
        printf("Quantity of Registers               %02x\n", tx_buffer[10]); // Quantity of Coils to Read
        printf("Quantity of Registers               %02x\n", tx_buffer[11]); // Quantity of Coils to Read

        // Send data via socket
        send(sock, tx_buffer, tx_buffer_length, 0);

        counter++;
        printf("Send_reply function number %d was activated\n", counter);
        vTaskDelay(2000 / portTICK_PERIOD_MS);

        shutdown(sock, 0);
        close(sock);
    }
    close(listen_sock);
    vTaskDelete(NULL);
}

static void wifi_event_handler(void *event_handler_arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    switch (event_id)
    {
    case WIFI_EVENT_STA_START:
        printf("WiFi connecting WIFI_EVENT_STA_START ... \n");
        break;
    case WIFI_EVENT_STA_CONNECTED:
        printf("WiFi connected WIFI_EVENT_STA_CONNECTED ... \n");
        break;
    case WIFI_EVENT_STA_DISCONNECTED:
        printf("WiFi lost connection WIFI_EVENT_STA_DISCONNECTED ... \n");
        break;
    case IP_EVENT_STA_GOT_IP:
        printf("WiFi got IP ... \n\n");
        break;
    default:
        break;
    }
}

void wifi_connection()
{
    nvs_flash_init();
    esp_netif_init();
    esp_event_loop_create_default();
    esp_netif_create_default_wifi_sta();
    wifi_init_config_t wifi_initiation = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&wifi_initiation);
    esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, wifi_event_handler, NULL);
    esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, wifi_event_handler, NULL);
    wifi_config_t wifi_configuration = {
        .sta = {
            .ssid = SSID,
            .password = PASS}};
    esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_configuration);
    esp_wifi_set_mode(WIFI_MODE_STA);
    esp_wifi_start();
    esp_wifi_connect();
}

void app_main(void)
{
    wifi_connection();
    vTaskDelay(5000 / portTICK_PERIOD_MS);
    xTaskCreate(tcp_server_task, "tcp_server", 4096, (void *)AF_INET, 5, NULL);
}
