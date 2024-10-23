# pip3 install adafruit-circuitpython-bno08x
import math

from accelerometers.common import *

from adafruit_bno08x.i2c import BNO08X_I2C
from adafruit_bno08x import BNO_REPORT_ACCELEROMETER, BNO_REPORT_GRAVITY, BNO_REPORT_ROTATION_VECTOR, BNO_REPORT_GYRO_INTEGRATED_ROTATION_VECTOR, BNO_REPORT_GAME_ROTATION_VECTOR

sensor = BNO08X_I2C(i2c)
sensor.enable_feature(BNO_REPORT_GRAVITY)
sensor.enable_feature(BNO_REPORT_ROTATION_VECTOR)


def get_angle(x, y, z):
    xy = math.sqrt(x**2 + y**2)
    angle_z = math.atan2(xy, z)

    return math.degrees(angle_z)

i=0
while True:
    i += 1
    x, y, z=sensor.gravity
    if i > 5:
        set_delta("x", x)
        set_delta("y", y)
        set_delta("z", z)
    
    angle = get_angle(x,y,z)
    print(f"x={x:.2f} / {deltas['x']:.0f}%", end="\t\t")
    print(f"y={y:.2f} / {deltas['y']:.0f}%", end="\t\t")
    print(f"z={z:.2f} / {deltas['z']:.0f}% angle={angle:.1f}")



    time.sleep(0.2)
