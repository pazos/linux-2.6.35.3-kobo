cmd_drivers/leds/built-in.o :=  arm-linux-ld -EL    -r -o drivers/leds/built-in.o drivers/leds/led-core.o drivers/leds/led-class.o drivers/leds/leds-mc13892.o 
