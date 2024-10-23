# pip3 install adafruit-circuitpython-icm20x

from accelerometers.common import *

import adafruit_icm20x

sensor = adafruit_icm20x.ICM20948(i2c)
sensor.accelerometer_range = adafruit_icm20x.AccelRange.RANGE_2G

run(sensor)
