// =============================================================================
// File		:	console_io.h
// Version	:	0.1.0
// Date		:	2018-05-31
// Author	:	TishSerg (Ukraine)
// Email	:	TishSerg@gmail.com
// =============================================================================

#pragma once
#ifndef REPL_CONSOLE_H
#define REPL_CONSOLE_H

#ifdef __cplusplus
extern "C" {
#endif


#include <stddef.h>

	void console_io_init(int buffer_size);
	int console_io_write(const char *data, size_t length);
	size_t console_io_readline(char data[], size_t maxLength);


#ifdef __cplusplus
}
#endif

#endif  /* REPL_CONSOLE_H */
