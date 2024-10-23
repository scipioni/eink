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

data = {"q0":None, "g0":[.0,.0,.0], "a0":.0}
try:
    with open(CALIBRATION_FILE, 'r') as f:
        data = json.load(f)
except:
    print("failed to load {CALIBRATION_FILE}, initialize with default {data}")

bno_changed = threading.Condition()
bno_thread = None

def get_angle(x, y, z):
    """
    from vector and gravity

    """
    xy = math.sqrt(x**2 + y**2)
    angle_z = math.atan2(xy, z)

    return math.degrees(angle_z)

def euler_from_quaternion(q, precision=1):
        i, j, k, w = q
        r = R.from_quat([i, j, k, w])
        euler = [round(x, precision) for x in r.as_euler('xyz', degrees=True)]
        return {"pitch": euler[1], "roll": euler[0], "yaw": euler[2]}

def read_bno():
    while True:
        with bno_changed:
            data["gravity"] = bno.gravity
            data["quaternion"] = bno.quaternion
            data["calibration"] = bno.calibration_status
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

def offset_angle(angle, a0, precision=1):
    return abs(angle-a0)

def bno_sse():
    while True:
        with bno_changed:
            bno_changed.wait()
            x, y, z = data["gravity"]
            gravity = offset_g(data["gravity"], data["g0"])
            angle = offset_angle(get_angle(x,y,z), data["a0"])
            quat = data["quaternion"]
            q0 = data["q0"]
            if q0:
                quat = offset(quat, q0)
            payload = {
                "gravity": gravity,
                "quaternion": [round(x,2) for x in quat],
                "euler": euler_from_quaternion(quat),
                "angle": f"{angle:.1f}",
            }
            yield "data: {0}\n\n".format(json.dumps(payload))


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
    data["q0"] = bno.quaternion
    data["g0"] = bno.gravity
    data["a0"] = get_angle(*bno.gravity)
    with open(CALIBRATION_FILE, 'w') as f:
        json.dump(data, f)
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
