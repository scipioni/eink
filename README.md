# E-INK diplay with esp32


- https://esphome.io/
- pinouts: https://randomnerdtutorials.com/esp8266-pinout-reference-gpios/
- http://staff.ltam.lu/feljc/electronics/homeassistant/ESPHome_programming_2.pdf


boards:
- xiao esp32c6
- firebeetle 2 esp32-c6 v1.0
- xiao esp32s3 (camera)

eink 
```bash
CONFIG=eink task flash
```

camera xiao esp32s3
```bash
CONFIG=camera task flash
```

