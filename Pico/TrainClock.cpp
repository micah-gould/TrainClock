#include <string>
#include <iostream>
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "lwip/apps/http_client.h"
#include "json.hpp"
#include "config.h"

#define GPIO_END 16

using json = nlohmann::json;

// ---------- GPIO helpers ----------
void gpio_on(int pin) {
    gpio_set_dir(pin, GPIO_OUT);
    gpio_put(pin, 0);
}

void gpio_off(int pin) {
    gpio_set_dir(pin, GPIO_IN);
}

void gpio_set(int pin, bool state) {
    if (state) {
        gpio_on(pin);
    } else {
        gpio_off(pin);
    }
}

// ---------- Number helpers ----------

void set_digit(int num, int display) {
    int PinMap[10][8] = {
        {0,0,1,1,1,1,1,1},
        {0,0,1,0,0,0,1,0},
        {0,1,0,1,1,1,1,0},
        {0,1,1,1,0,1,1,0},
        {0,1,1,0,0,0,1,1},
        {0,1,1,1,0,1,0,1},
        {0,1,1,1,1,1,0,1},
        {0,0,1,0,0,1,1,0},
        {0,1,1,1,1,1,1,1},
        {0,1,1,1,0,1,1,1}
    };

    for (int i = 0; i < 8; i++) {
        if (PinMap[num][i]) {
            gpio_set(i + 8 * display, 1);
        }
    }
}

void set_number(int num) {
    set_digit(num % 10, 0);
    set_digit(num / 10, 1);
}

void clear() {
    for (int i = 0; i < GPIO_END; i++) {
        gpio_set(i, 0);
    }
}

void overwrite_set_number(int num) {
    clear();
    set_number(num);
}

void set_error(int num) {
    clear();
    int E[8] = {0,1,0,1,1,1,0,1};

    for (int i = 0; i < 8; i++) {
        if (E[i]) {
            gpio_set(i + 8, 1);
        }
    }

    set_digit(num % 10, 0);
    gpio_set(0, 1);
}

// ---------- HTTP GET using Pico W (async) ----------
struct HttpResponse {
    std::string body;
    bool done = false;
};

// TCP receive callback
static err_t http_body_cb(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, signed char err) {
    HttpResponse* r = static_cast<HttpResponse*>(arg);
    if (p != nullptr) {
        int copy_len = p->tot_len;
        r->body.append((char*)p->payload, copy_len);
        pbuf_free(p);
    }
    return ERR_OK;
}

// HTTP result callback
static void http_result_cb(void* arg, httpc_result_t result, u32_t content_len, u32_t response_time, signed char err_code) {
    HttpResponse* r = static_cast<HttpResponse*>(arg);
    r->done = true;
}

// Synchronous wrapper for HTTP GET
bool http_get_sync(const std::string &url, HttpResponse &resp) {
    httpc_connection_t settings = {};
    settings.use_proxy = 0;
    settings.result_fn = http_result_cb;

    httpc_state_t* conn = nullptr;
    err_t err = httpc_get_file_dns(url.c_str(), HTTP_DEFAULT_PORT, "/", &settings, http_body_cb, &resp, &conn);
    if (err != ERR_OK) {
        std::cout << "HTTP request failed: " << err << std::endl;
        return false;
    }

    // Wait until done
    while(!resp.done){
        cyw43_arch_poll();
        sleep_ms(10);
    }

    return true;
}

// ---------- Get departures ----------
int getNextTime() {
    std::cout << "Getting next departure time from local server" << std::endl;

    HttpResponse resp;
    resp.body.clear(); 
    resp.done = false;

    const char* host = "192.168.86.52";
    const char* path = "/nextTrain";
    uint16_t port = 8159;

    // Configure HTTP connection
    httpc_connection_t settings = {};
    settings.use_proxy = 0;
    settings.result_fn = http_result_cb;

    httpc_state_t* conn = nullptr;
    err_t err = httpc_get_file_dns(host, port, path, &settings, http_body_cb, &resp, &conn);

    if (err != ERR_OK) {
        std::cout << "HTTP request failed: " << err << std::endl;
        return -1;
    }

    // Wait for response to complete
    while (!resp.done) {
        cyw43_arch_poll();
        sleep_ms(10);
    }

    // Debug: raw response
    std::cout << "Response raw: " << resp.body << std::endl;

    // Exception-free JSON parse
    auto depJson = nlohmann::json::parse(resp.body, nullptr, false);
    if (depJson.is_discarded()) {
        std::cout << "JSON parsing error" << std::endl;
        return -2;
    }

    if (!depJson.contains("nextTrainInMinutes") || depJson["nextTrainInMinutes"].is_null()) {
        std::cout << "JSON missing nextTrainInMinutes" << std::endl;
        return -3;
    }

    int nextTime = depJson["nextTrainInMinutes"];
    std::cout << "Next train in " << nextTime << " minutes" << std::endl;
    return nextTime;
}

// ---------- Main ----------
int main() {
    stdio_init_all();
        for (int i = 0; i < GPIO_END; i++) {
        gpio_init(i);
    }

    // Initialise the Wi-Fi chip
    if (cyw43_arch_init_with_country(CYW43_COUNTRY_GERMANY)) {
        std::cout << "Wi-Fi init failed" << std::endl;
        set_error(0);
        for (int i = 0; i < 100; i++) {
            cyw43_arch_gpio_put(0, 1);
            sleep_ms(50);
            cyw43_arch_gpio_put(0, 0);
            sleep_ms(50);
        }
        return 1;
    }

    for (int i = 0; i < 2; i++) {
        cyw43_arch_gpio_put(0, 1);
        sleep_ms(250);
        cyw43_arch_gpio_put(0, 0);
        sleep_ms(250);
    }

    std::cout << "Initialized" << std::endl;
    
    cyw43_arch_enable_sta_mode();

    if (cyw43_arch_wifi_connect_timeout_ms(SSID, PASS, CYW43_AUTH_WPA2_AES_PSK, 10000)) {
        std::cout << "Failed to connect" << std::endl;
        set_error(1);
        for (int i = 0; i < 50; i++) {
            cyw43_arch_gpio_put(0, 1);
            sleep_ms(100);
            cyw43_arch_gpio_put(0, 0);
            sleep_ms(100);
        }
        return 1;
    }

    cyw43_arch_gpio_put(0, 1);
    sleep_ms(500);
    cyw43_arch_gpio_put(0, 0);
    sleep_ms(500);

    std::cout << "Connected" << std::endl;

    int time = 0;
    int newTime = 0;
    int counter = 0;
    int errorCode;
    while (true) {
        std::cout << (time, newTime, counter, errorCode) << std::endl;
        counter++;
        gpio_set(8, 1);
        newTime = getNextTime() % 100;
        gpio_set(8, 0);
        if (0 < newTime) {
            time = newTime;
            counter = 0;
            overwrite_set_number(time);
            for (int i = 0; i < 15; i++) {
                gpio_set(0, 1);
                sleep_ms(500);
                gpio_set(0, 0);
                sleep_ms(500);
            }
            continue;
        }
        errorCode = 1 - newTime;
        sleep_ms(5000);
        if (counter % 12 == 0 && time > 0) {
            time--;
        }
        if (time == 0) {
            set_error(errorCode);
        }
    }

    std::cout << "Error" << std::endl;
}
