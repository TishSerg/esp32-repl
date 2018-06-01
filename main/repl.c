// =============================================================================
// File		:	repl.c
// Version	:	0.1.0
// Date		:	2018-05-31
// Author	:	TishSerg (Ukraine)
// Email	:	TishSerg@gmail.com
// =============================================================================

#include <stdio.h>
#include <string.h>
#include "esp_event.h"	// for vTaskDelete()
#include "console_io.h"
#include "http_client.h"

#ifdef CONFIG_REPL_CMD_BUF_SIZE	// case: defined by 'make menuconfig'
#define REPL_CMD_BUF_SIZE	(CONFIG_REPL_CMD_BUF_SIZE)
#else	// case: fallback default
#define REPL_CMD_BUF_SIZE	(512)
#endif


// =============================================================================
// ======================= Internal variables ==================================


// =============================================================================


// =============================================================================
// ======================= Internal functions ==================================

static int handle_repl_command(char cmd_buffer[])
{
	const char tokenDelim[] = " ";		// tokens of command delimiters
	char *pSaveToken;
	char *pToken;
	pToken = strtok_r(cmd_buffer, tokenDelim, &pSaveToken);	// take command
	if (pToken != NULL) {
		if (strcasecmp("help", pToken) == 0) {
			sprintf(cmd_buffer, "Welcome to TishSerg's ESP32 REPL!\n"
					"Available commands:\n"
					"\thelp - display this help\n"
					"\thttp - perform a HTTP request\n"
					"\texit - exit the REPL\n");
			console_io_write(cmd_buffer, strlen(cmd_buffer));
			return 0;	// command has been handled
		}
		else if (strcasecmp("http", pToken) == 0) {
			pToken = strtok_r(NULL, tokenDelim, &pSaveToken);	// take HTTP method
			HTTP_Method_t http_method;
			char *url;
			char *body;

			if (pToken != NULL) {
				if (strcasecmp("get", pToken) == 0) {
					http_method = HTTP_Method_GET;
				}
				else if (strcasecmp("post", pToken) == 0) {
					http_method = HTTP_Method_POST;
				}
				else {
					sprintf(cmd_buffer, "Unrecognized HTTP method: %s\n"
							"Available methods: GET, POST\n", pToken);
					console_io_write(cmd_buffer, strlen(cmd_buffer));
					return 0;	// command has been handled
				}

				pToken = strtok_r(NULL, tokenDelim, &pSaveToken);	// take URL
				if (pToken != NULL) {
					url = pToken;
				}
				else {
					sprintf(cmd_buffer, "URL is required field! HTTP usage:\n"
							"http <METHOD> <URL>[ <BODY>]\n");
					console_io_write(cmd_buffer, strlen(cmd_buffer));
					return 0;	// command has been handled
				}

				if (http_method == HTTP_Method_POST) {
					pToken = strtok_r(NULL, tokenDelim, &pSaveToken);	// take POST body
					if (pToken != NULL) {
						body = pToken;
					}
					else {
						sprintf(cmd_buffer, "BODY is required for POST method! HTTP POST usage:\n"
								"http POST <URL> <BODY>\n");
						console_io_write(cmd_buffer, strlen(cmd_buffer));
						return 0;	// command has been handled
					}
				} else {
					body = NULL;
				}

				// At here, all the HTTP data is collected
				http_perform_request(http_method, url, body);
				return 0;	// command has been handled
			}
			else {
				sprintf(cmd_buffer, "HTTP usage:\n"
						"http <METHOD> <URL>[ <BODY>]\n");
				console_io_write(cmd_buffer, strlen(cmd_buffer));
				return 0;	// command has been handled
			}
		}
		else if (strcasecmp("exit", pToken) == 0) {
			sprintf(cmd_buffer, "Really exit? OK. Bye.\n");
			console_io_write(cmd_buffer, strlen(cmd_buffer));
			return !0;	// exit REPL
		}
		else {
			sprintf(cmd_buffer, "Unrecognized command: %s. Type 'help' if you need help.\n", pToken);
			console_io_write(cmd_buffer, strlen(cmd_buffer));
			return 0;	// command has been handled
		}
	}
	return 0;	// no command - do nothing
}

static int httpResponseCallback(const char *data, size_t length)
{
	return console_io_write(data, length);
}
// =============================================================================


// =============================================================================
// ======================= Exported functions ==================================

void repl_task(void *pvParameters)
{
	char cmd_buffer[REPL_CMD_BUF_SIZE];	// if this is too big - it's better to allocate it in heap (using malloc() or static)

	console_io_init(REPL_CMD_BUF_SIZE*2);
	http_init(httpResponseCallback);

	int breakLoop = !0;
	while (!breakLoop)	// main task loop
	{
		size_t cmd_len = console_io_readline(cmd_buffer, sizeof(cmd_buffer)/sizeof(cmd_buffer[0]));
		(void)cmd_len;	// prevent unused variable compiler warning
		breakLoop = handle_repl_command(cmd_buffer);
	}

	http_deinit();
	vTaskDelete(NULL);
}
// =============================================================================
