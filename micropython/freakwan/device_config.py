from machine import Pin, ADC

class DeviceConfig:
    config = {}
    
    def get_battery_microvolts(): return 0
    def power_up(freakwan): pass # By default, nothing to do at power-up.

    if False:
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

    config['sx1262'] = {
        'busy': 13,
        'miso': 11,
        'mosi': 10,
        'clock': 9, # sck
        'chipselect': 8, # nss
        'reset': 12,
        'dio': 14,
    }


