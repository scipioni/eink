# E-INK diplay with esp32


- https://esphome.io/
- https://wiki.seeedstudio.com/XIAO_ESP32S3_esphome/
- http://staff.ltam.lu/feljc/electronics/homeassistant/ESPHome_programming_2.pdf


boards:
- xiao esp32c6
- firebeetle 2 esp32-c6 v1.0
- xiao esp32s3 (camera)
- orange pi zero lts:
    - ethernet, wifi, 512M, sd card, usb A, 39euro with plastic box (POE optional)
- waypondev nanopi neo plus2:
    - ethernet, wifi, 512M, EMMC 8G, sd card, 2 x usb A, 46euro
- nanopi neo:
    - ethernet, no wifi, 512M, sd card, usb A, 45euro with metal box

hw:
- AZDelivery 3 x Anello LED RGB compatibile con WS2812B da 5 V
- BNO055 accelerometer, gyroscope, magnetometer
  - https://mischianti.org/bno055-for-esp32-esp8266-and-arduino-wiring-and-advanced-bosch-library-2/ 

eink 
```bash
CONFIG=eink task flash
```

camera xiao esp32s3
```bash
CONFIG=camera task flash
```

