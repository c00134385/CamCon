typedef void (*uart2_input_byte_callback)(unsigned char);
void UART2_Configuration(void);
int uart2_comm_get_length(void);
int uart2_comm_message_available(void);
int uart2_comm_read(unsigned char *buff, int length);
void uart2_comm_write(unsigned char *buff, int length);
void uart2_comm_clear(void);
void uart2_print_rx_data(void);
void uart2_set_input_byte_callback(uart2_input_byte_callback cb);

