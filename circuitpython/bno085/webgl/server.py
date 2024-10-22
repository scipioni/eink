import json
import threading
import time
import math
import numpy as np
import quaternion
from scipy.spatial.transform import Rotation as R

import board
import busio
import flask

from adafruit_bno08x.i2c import BNO08X_I2C
from adafruit_bno08x import BNO_REPORT_ACCELEROMETER, BNO_REPORT_GRAVITY, BNO_REPORT_ROTATION_VECTOR, BNO_REPORT_GYRO_INTEGRATED_ROTATION_VECTOR, BNO_REPORT_GAME_ROTATION_VECTOR

i2c = board.I2C() 
bno = BNO08X_I2C(i2c)
bno.enable_feature(BNO_REPORT_GRAVITY)
bno.enable_feature(BNO_REPORT_ROTATION_VECTOR)

BNO_UPDATE_FREQUENCY_HZ = 10
CALIBRATION_FILE = "calibration.json"

app = flask.Flask(__name__)

bno_data = {"q0":None, "g0":[.0,.0,.0]}
bno_changed = threading.Condition()
bno_thread = None

def euler_from_quaternion2(q, precision=1):
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

def euler_from_quaternion(q, precision=1):
        i, j, k, w = q
        r = R.from_quat([i, j, k, w])
        euler = [round(x, precision) for x in r.as_euler('xyz', degrees=True)]
        return {"pitch": euler[1], "roll": euler[0], "yaw": euler[2]}

def read_bno():
    while True:
        with bno_changed:
            bno_data["gravity"] = bno.gravity
            bno_data["quaternion"] = bno.quaternion
            bno_data["calibration"] = bno.calibration_status
            bno_changed.notify_all()
        time.sleep(1.0 / BNO_UPDATE_FREQUENCY_HZ)


def offset(q, q0):
    i, j, k, w = q
    q_ = quaternion.quaternion(w,i,j,k)
    i, j, k, w = q0
    q0_ = quaternion.quaternion(w,i,j,k)
    r = (q_ + q0_.conjugate()).normalized()
    return [r.x,r.y,r.z,r.w]

def offset_g(g, g0, precision=2):
    x, y, z = g
    x0, y0, z0 = g0
    
    return {"x": round(x-x0, precision), "y": round(y-y0, precision), "z": round(z-z0, precision)}

def bno_sse():
    while True:
        with bno_changed:
            bno_changed.wait()
            x, y, z = bno_data["gravity"]
            gravity = offset_g(bno_data["gravity"], bno_data["g0"])
            quat = bno_data["quaternion"]
            q0 = bno_data["q0"]
            if q0:
                quat = offset(quat, q0)
            data = {
                "gravity": gravity,
                "quaternion": [round(x,2) for x in quat],
                "euler": euler_from_quaternion(quat),
            }
            yield "data: {0}\n\n".format(json.dumps(data))


@app.before_first_request
def start_bno_thread():
    global bno_thread  # pylint: disable=global-statement
    bno_thread = threading.Thread(target=read_bno)
    bno_thread.daemon = True  # Don't let the BNO reading thread block exiting.
    bno_thread.start()


@app.route("/bno")
def bno_path():
    return flask.Response(bno_sse(), mimetype="text/event-stream")
 

@app.route("/save_calibration", methods=["POST"])
def save_calibration():
    bno_data["q0"] = bno.quaternion
    bno_data["g0"] = bno.gravity
    print(f"save q0={bno.quaternion}")
    return "OK"

@app.route("/load_calibration", methods=["POST"])
def load_calibration():
    return "OK"

@app.route("/")
def root():
    return flask.render_template("index.html")

if __name__ == "__main__":
    app.run(host="0.0.0.0", port=80, debug=True, threaded=True)
