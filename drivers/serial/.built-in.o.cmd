cmd_drivers/serial/built-in.o :=  arm-linux-ld -EL    -r -o drivers/serial/built-in.o drivers/serial/serial_core.o drivers/serial/mxc_uart.o drivers/serial/mxc_uart_early.o 
