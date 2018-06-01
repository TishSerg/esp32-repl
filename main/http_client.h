// =============================================================================
// File		:	http_client.h
// Version	:	0.1.0
// Date		:	2018-05-31
// Author	:	TishSerg (Ukraine)
// Email	:	TishSerg@gmail.com
// =============================================================================

#pragma once
#ifndef HTTP_CLIENT_H
#define HTTP_CLIENT_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>

	typedef enum {
		HTTP_Method_GET,
		HTTP_Method_POST,
	} HTTP_Method_t;

	typedef int (*HTTP_ResponseCallback_t)(const char *data, size_t length);

	void http_init(HTTP_ResponseCallback_t callback);
	void http_deinit(void);
	void http_perform_request(HTTP_Method_t method, const char *url, const char *body);


#ifdef __cplusplus
}
#endif

#endif  /* HTTP_CLIENT_H */
