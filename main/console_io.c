// =============================================================================
// File		:	console_io.c
// Version	:	0.1.0
// Date		:	2018-05-31
// Author	:	TishSerg (Ukraine)
// Email	:	TishSerg@gmail.com
// =============================================================================

#include "freertos/FreeRTOS.h"
#include "driver/uart.h"


// =============================================================================
// ======================= Config ==============================================

#ifdef CONFIG_CONSOLE_IO_UART_BAUDRATE	// case: defined by 'make menuconfig'
#define CONSOLE_IO_UART_BAUDRATE	(CONFIG_CONSOLE_IO_UART_BAUDRATE)
#else	// case: fallback default
#define CONSOLE_IO_UART_BAUDRATE	(115200)
#endif

#ifdef CONFIG_CONSOLE_IO_UART_NUM	// TODO check this
#defined CONSOLE_IO_UART_NUM (UART_NUM_2)
#endif

#if defined(CONFIG_CONSOLE_IO_UART_NUM_0)	// case: defined by 'make menuconfig'
#define CONSOLE_IO_UART_NUM	(UART_NUM_0)
#elif defined(CONFIG_CONSOLE_IO_UART_NUM_1)
#define CONSOLE_IO_UART_NUM	(UART_NUM_1)
#elif defined(CONFIG_CONSOLE_IO_UART_NUM_2)
#define CONSOLE_IO_UART_NUM	(UART_NUM_2)
#else	// case: fallback default
#define CONSOLE_IO_UART_NUM	(UART_NUM_1)
#endif

#ifdef CONFIG_CONSOLE_IO_UART_SET_PINS	// case: defined by 'make menuconfig'
#define CONSOLE_IO_UART_TX_IO	(CONFIG_CONSOLE_IO_UART_TX_IO)
#define CONSOLE_IO_UART_RX_IO	(CONFIG_CONSOLE_IO_UART_RX_IO)
#else	// case: fallback default
#define CONSOLE_IO_UART_TX_IO	(UART_PIN_NO_CHANGE)
#define CONSOLE_IO_UART_RX_IO	(UART_PIN_NO_CHANGE)
#endif

#define CONSOLE_LINE_DELIMITER	('\n')
// =============================================================================


// =============================================================================
// ======================= Exported functions ==================================

void console_io_init(int buffer_size)
{
	const uart_config_t uart_config = {
			.baud_rate = CONSOLE_IO_UART_BAUDRATE,
			.data_bits = UART_DATA_8_BITS,
			.parity = UART_PARITY_DISABLE,
			.stop_bits = UART_STOP_BITS_1,
			.flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
	};
	uart_param_config(CONSOLE_IO_UART_NUM, &uart_config);
	uart_set_pin(CONSOLE_IO_UART_NUM, CONSOLE_IO_UART_TX_IO, CONSOLE_IO_UART_RX_IO, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
	uart_driver_install(CONSOLE_IO_UART_NUM, buffer_size, 0, 0, NULL, 0);
}

int console_io_write(const char *data, size_t length)
{
	return uart_write_bytes(CONSOLE_IO_UART_NUM, data, length);
}

size_t console_io_readline(char data[], size_t maxLength)
{
	const int readTimeout = 100;
	size_t dataLength = 0;

	while (dataLength < maxLength)
	{
		// We will read byte-by-byte analyzing each one to avoid complicated code
		int bytesRead = uart_read_bytes(CONSOLE_IO_UART_NUM, (uint8_t*)&data[dataLength], 1, readTimeout * portTICK_PERIOD_MS);

		if (bytesRead > 0) {
			if (data[dataLength] == CONSOLE_LINE_DELIMITER)
			{
				data[dataLength] = '\0';	// we're dropping trailing delimiter
			}

			if (data[dataLength] == '\0')
			{	// We'll get here in case delimiter read or null-byte read
				dataLength += bytesRead;
				break;
			}
			else
			{
				dataLength += bytesRead;
			}
		}
	}

	return dataLength;
}
// =============================================================================
