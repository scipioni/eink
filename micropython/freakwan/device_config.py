
class DeviceConfig:
    config = {}
    
    def get_battery_microvolts(): return 0
    def power_up(freakwan): pass # By default, nothing to do at power-up.


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
    
    if False:
        config['ssd1306']= {
            'sda': 48,
            'scl': 47,
            'xres': 128,
            'yres': 64,
        }

