# E-INK diplay with esp32


- https://esphome.io/
- https://wiki.seeedstudio.com/XIAO_ESP32S3_esphome/
- http://staff.ltam.lu/feljc/electronics/homeassistant/ESPHome_programming_2.pdf


boards:
- xiao esp32c6
- firebeetle 2 esp32-c6 v1.0
- xiao esp32s3 (camera)

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

