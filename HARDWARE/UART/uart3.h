
typedef void (*uart3_rx_callback)(unsigned char);

void UART3_Configuration(void);
void UART3_set_baudrate(int baudrate);
void uart3_sendbf(unsigned char *bf, int len);
int uart3_get_reclen(void);
void uart3_printf(char *bf);
void uart3_clr(void);


int uart3_fputc(int ch);
int uart3_fgetc(void);


void uart3_set_rx_callback(uart3_rx_callback cb);

