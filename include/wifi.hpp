#pragma once

#include <freertos/FreeRTOS.h>
#include <freertos/event_groups.h>
#include <esp_log.h>
#include <esp_event.h>

#include "logger.hpp"

class WiFi {
    static constexpr auto CONNECTED_BIT = BIT0;
    static Logger LOGGER;

public:
    static std::error_code initialize();
    static std::error_code connect(const std::string &ssid, const std::string &password, TickType_t timeout);

    static void on_got_ip();
    static std::error_code on_disconnect();

private:
    static EventGroupHandle_t _event_group;

    static esp_err_t _event_handler(void *ctx, system_event_t *event);
};
