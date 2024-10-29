### This file holds your device hardware configuration.
#
# Here we show a couple of different examples, however if you want
# to create a configuration for your device that is yet not supported
# by FreakWAN, it is probably better to start from the configuration of
# a device similar and modify it.

from machine import Pin

class DeviceConfig:
    config = {}

    config['ssd1306']= {
        'sda': 4,
        'scl': 15,
        'xres': 128,
        'yres': 64,
    }
    
    def power_up():
        # Init battery voltage pin
        DeviceConfig.battery_adc = ADC(Pin(1))

        # Voltage is divided by 2 befor reaching PID 32. Since normally
        # a 3.7V battery is used, to sample it we need the full 3.3
        # volts range.
        DeviceConfig.battery_adc.atten(ADC.ATTN_11DB)

    def get_battery_microvolts():
        return DeviceConfig.battery_adc.read_uv()*2

    config['tx_led'] = {
        'pin': 35,
        'inverted': False,      # Set to True if pin on = led off
    }

    config['sx1276'] = {
        'miso': 19,
        'mosi': 27,
        'clock': 5,
        'chipselect': 18,
        'reset': 14,
        'dio0': 26
    }


