from machine import Pin
import time
led_pin = 25  # helteck wifi lora 32 (v1)

def blink():
    led = Pin(led_pin, Pin.OUT)
    while True:
      led.on()
      time.sleep_ms(500)
      led.off()
      time.sleep_ms(500)

if __name__ == "__main__":
    blink()
