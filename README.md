# a5100
# a5100

install esptool
erase flash
esptool.py --port com4 --baud 115200 write_flash -fm dio --flash_size=detect 0 esp8266.bin
