ESP8266 SD card connections:
https://learn.sparkfun.com/tutorials/esp8266-thing-hookup-guide/using-the-arduino-addon
Pin Number       SPI Function
12               MISO
13               MOSI
14               SCLK
15               CS

Note these Pin Numbers corresponds to the GPIO numbers (not the processor numbers...). The chipselect pin number necessaary to provide to the arduinio IDE SD libraries also relates to the GPIO numbers (probably this is how the pin translation was done at the lower levels).