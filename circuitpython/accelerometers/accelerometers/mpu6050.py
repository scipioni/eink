# pip3 install adafruit-circuitpython-mpu6050

from accelerometers.common import *

from adafruit_mpu6050 import MPU6050, Range

sensor = MPU6050(i2c)
sensor.accelerometer_range = Range.RANGE_2_G

run(sensor)
