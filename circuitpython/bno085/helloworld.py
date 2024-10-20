import time
import math
import board
import busio
from adafruit_bno08x.i2c import BNO08X_I2C
from adafruit_bno08x import BNO_REPORT_ACCELEROMETER, BNO_REPORT_GRAVITY, BNO_REPORT_ROTATION_VECTOR, BNO_REPORT_GYRO_INTEGRATED_ROTATION_VECTOR, BNO_REPORT_GAME_ROTATION_VECTOR

i2c = board.I2C()
bno = BNO08X_I2C(i2c)
bno.enable_feature(BNO_REPORT_GRAVITY)
bno.enable_feature(BNO_REPORT_ROTATION_VECTOR)

def euler_from_quaternion(q, precision=1):
        """
        Convert a quaternion into euler angles (roll, pitch, yaw)
        roll is rotation around x in radians (counterclockwise)
        pitch is rotation around y in radians (counterclockwise)
        yaw is rotation around z in radians (counterclockwise)
        """
        x, y, z, w = q
        t0 = +2.0 * (w * x + y * z)
        t1 = +1.0 - 2.0 * (x * x + y * y)
        roll_x = math.atan2(t0, t1)
     
        t2 = +2.0 * (w * y - z * x)
        t2 = +1.0 if t2 > +1.0 else t2
        t2 = -1.0 if t2 < -1.0 else t2
        pitch_y = math.asin(t2)
     
        t3 = +2.0 * (w * z + x * y)
        t4 = +1.0 - 2.0 * (y * y + z * z)
        yaw_z = math.atan2(t3, t4)
     
        return {"pitch": round(math.degrees(pitch_y),precision), "roll":round(math.degrees(roll_x),precision), "yaw":round(math.degrees(yaw_z),precision)}

while True:
    accel_x, accel_y, accel_z = bno.gravity  # pylint:disable=no-member
    print("X:%0.6f  Y:%0.6f Z:%0.6f  [m/s^2], euler:%s" % (accel_x, accel_y, accel_z, euler_from_quaternion(bno.quaternion)))
    time.sleep(0.1)
