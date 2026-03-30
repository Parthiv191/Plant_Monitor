#include "http_server.h"
#include <stdio.h>
#include "plant_data.h"
#include "esp_http_server.h"
#include "esp_log.h"
#include "mdns.h"

static const char *TAG = "http_server";
static httpd_handle_t s_server = NULL;

static esp_err_t api_data_get_handler(httpd_req_t *req)
{
    plant_readings_t r;
    plant_data_get(&r);
    char buf[384];
    int n = snprintf(buf, sizeof(buf),
                     "{\"temp_c\":%.2f,\"humidity\":%.2f,\"light_pct\":%.2f,\"soil_pct\":%.2f,"
                     "\"temp_status\":\"%s\",\"humidity_status\":\"%s\",\"light_status\":\"%s\",\"soil_status\":\"%s\"}",
                     r.temp_c, r.humidity, r.light_pct, r.soil_pct,
                     r.temp_status, r.humidity_status, r.light_status, r.soil_status);
    if (n < 0 || n >= (int)sizeof(buf)) {
        httpd_resp_send_500(req);
        return ESP_FAIL;
    }
    httpd_resp_set_type(req, "application/json");
    return httpd_resp_send(req, buf, (size_t)n);
}

static esp_err_t root_get_handler(httpd_req_t *req)
{
    static const char html[] =
        "<!DOCTYPE html><html><head><meta charset=\"utf-8\"/><meta name=\"viewport\" content=\"width=device-width,initial-scale=1\"/>"
        "<title>Plant Monitor</title>"
        "<style>"
        "body{font-family:system-ui,sans-serif;display:flex;justify-content:center;padding:1rem;background:#111;color:#eee;margin:0;}"
        ".card{background:#222;border-radius:12px;padding:1.5rem;max-width:480px;width:100%;box-shadow:0 4px 20px rgba(0,0,0,.4);}"
        "h1{margin:0 0 1rem;font-size:1.25rem;}"
        ".grid{display:grid;gap:.75rem;}"
        ".row{display:flex;justify-content:space-between;align-items:center;border-bottom:1px solid #333;padding:.35rem 0;flex-wrap:wrap;gap:.25rem;}"
        ".label{opacity:.6}.val{font-weight:600}.st{font-size:.85rem;opacity:.8;width:100%;text-align:right;}"
        "</style></head><body><div class=\"card\"><h1>Plant Monitor</h1><div id=\"g\" class=\"grid\">Loading...</div></div>"
        "<script>"
        "async function f(){try{const r=await fetch('/api/data');const j=await r.json();"
        "document.getElementById('g').innerHTML="
        "'<div class=\"row\"><span class=\"label\">Temp</span><span class=\"val\">'+j.temp_c.toFixed(1)+'C</span><span class=\"st\">'+j.temp_status+'</span></div>'+"
        "'<div class=\"row\"><span class=\"label\">Humidity</span><span class=\"val\">'+j.humidity.toFixed(1)+'%</span><span class=\"st\">'+j.humidity_status+'</span></div>'+"
        "'<div class=\"row\"><span class=\"label\">Light</span><span class=\"val\">'+j.light_pct.toFixed(1)+'%</span><span class=\"st\">'+j.light_status+'</span></div>'+"
        "'<div class=\"row\"><span class=\"label\">Soil</span><span class=\"val\">'+j.soil_pct.toFixed(1)+'%</span><span class=\"st\">'+j.soil_status+'</span></div>';"
        "}catch(e){document.getElementById('g').textContent='Error loading data';}}"
        "f();setInterval(f,5000);"
        "</script></body></html>";
    httpd_resp_set_type(req, "text/html");
    return httpd_resp_send(req, html, HTTPD_RESP_USE_STRLEN);
}

esp_err_t http_server_start(void)
{
    httpd_config_t cfg = HTTPD_DEFAULT_CONFIG();
    cfg.lru_purge_enable = true;
    esp_err_t err = httpd_start(&s_server, &cfg);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "httpd_start: %s", esp_err_to_name(err));
        return err;
    }

    httpd_uri_t root = {
        .uri = "/",
        .method = HTTP_GET,
        .handler = root_get_handler,
    };
    httpd_uri_t api = {
        .uri = "/api/data",
        .method = HTTP_GET,
        .handler = api_data_get_handler,
    };
    ESP_ERROR_CHECK(httpd_register_uri_handler(s_server, &root));
    ESP_ERROR_CHECK(httpd_register_uri_handler(s_server, &api));
    ESP_LOGI(TAG, "HTTP server on port %d", cfg.server_port);
    return ESP_OK;
}

void plant_mdns_start(void)
{
    esp_err_t err = mdns_init();
    if (err != ESP_OK) {
        ESP_LOGW(TAG, "mdns_init: %s", esp_err_to_name(err));
        return;
    }
    mdns_hostname_set("plantmonitor");
    mdns_instance_name_set("Plant Monitor");
    err = mdns_service_add(NULL, "_http", "_tcp", 80, NULL, 0);
    if (err != ESP_OK) {
        ESP_LOGW(TAG, "mdns_service_add: %s", esp_err_to_name(err));
    } else {
        ESP_LOGI(TAG, "mDNS: http://plantmonitor.local/");
    }
}
