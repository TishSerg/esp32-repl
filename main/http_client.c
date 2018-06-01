// =============================================================================
// File		:	http_client.c
// Version	:	0.1.0
// Date		:	2018-05-31
// Author	:	TishSerg (Ukraine)
// Email	:	TishSerg@gmail.com
// =============================================================================

#include <string.h>
#include "esp_http_client.h"
#include "esp_log.h"
#include "http_client.h"

static const char *TAG = "HTTP_CLIENT";


// =============================================================================
// ======================= Internal variables ==================================

static HTTP_ResponseCallback_t http_write_response_cb;
static esp_http_client_handle_t http_client;
// =============================================================================


// =============================================================================
// ======================= Internal functions ==================================

static int HttpResponseCallback_Default(const char *data, size_t length)
{
	return 0;
}

static esp_err_t _http_event_handler(esp_http_client_event_t *evt)
{
    switch(evt->event_id) {
        case HTTP_EVENT_ERROR:
            ESP_LOGD(TAG, "HTTP_EVENT_ERROR");
            break;
        case HTTP_EVENT_ON_CONNECTED:
            ESP_LOGD(TAG, "HTTP_EVENT_ON_CONNECTED");
            break;
        case HTTP_EVENT_HEADER_SENT:
            ESP_LOGD(TAG, "HTTP_EVENT_HEADER_SENT");
            break;
        case HTTP_EVENT_ON_HEADER:
            ESP_LOGD(TAG, "HTTP_EVENT_ON_HEADER, key=%s, value=%s", evt->header_key, evt->header_value);
            break;
        case HTTP_EVENT_ON_DATA:
            ESP_LOGD(TAG, "HTTP_EVENT_ON_DATA, len=%d", evt->data_len);
            if (!esp_http_client_is_chunked_response(evt->client)) {
            	http_write_response_cb(evt->data, evt->data_len);
            }
            break;
        case HTTP_EVENT_ON_FINISH:
            ESP_LOGD(TAG, "HTTP_EVENT_ON_FINISH");
            break;
        case HTTP_EVENT_DISCONNECTED:
            ESP_LOGD(TAG, "HTTP_EVENT_DISCONNECTED");
            break;
    }
    return ESP_OK;
}
// =============================================================================


// =============================================================================
// ======================= Exported functions ==================================

void http_init(HTTP_ResponseCallback_t callback)
{
	http_write_response_cb = callback;
	if (http_write_response_cb == NULL)
	{
		http_write_response_cb = HttpResponseCallback_Default;	// to prevent null-pointer call
	}

	esp_http_client_config_t config = {
			.event_handler = _http_event_handler,
	};

	http_client = esp_http_client_init(&config);
}

void http_deinit(void)
{
	esp_http_client_cleanup(http_client);
}

void http_perform_request(HTTP_Method_t method, const char *url, const char *body)
{
	esp_http_client_set_url(http_client, url);
	esp_http_client_set_method(http_client, method == HTTP_Method_POST ? HTTP_METHOD_POST : HTTP_METHOD_GET);
	esp_http_client_set_post_field(http_client, body, strlen(body));

	esp_err_t err = esp_http_client_perform(http_client);
	if (err == ESP_OK) {
		ESP_LOGI(TAG, "HTTP %s Status = %d, content_length = %d",
				method == HTTP_Method_POST ? "POST" : "GET",
				esp_http_client_get_status_code(http_client),
				esp_http_client_get_content_length(http_client));
	} else {
		ESP_LOGE(TAG, "HTTP %s request failed: %s", method == HTTP_Method_POST ? "POST" : "GET", esp_err_to_name(err));
	}
}
// =============================================================================
