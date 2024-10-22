# pip3 install adafruit-circuitpython-mpu6050


import json
import threading
import time

import board
import flask

from adafruit_mpu6050 import MPU6050, Range

i2c = board.I2C()  # uses board.SCL and board.SDA
mpu = MPU6050(i2c)
mpu.accelerometer_range = Range.RANGE_2_G
print(dir(mpu))

BNO_UPDATE_FREQUENCY_HZ = 10

app = flask.Flask(__name__)

bno_data = {"g0":[.0,.0,.0]}
bno_changed = threading.Condition()
bno_thread = None

def read_bno():
    while True:
        with bno_changed:
            bno_data["gravity"] = mpu.acceleration
            bno_data["calibration"] = None
            bno_changed.notify_all()
        time.sleep(1.0 / BNO_UPDATE_FREQUENCY_HZ)



def offset_g(g, g0, precision=1):
    x, y, z = g
    x0, y0, z0 = g0
    
    return {"x": round(x-x0, precision), "y": round(y-y0, precision), "z": round(z-z0, precision)}

def bno_sse():
    while True:
        with bno_changed:
            bno_changed.wait()
            x, y, z = bno_data["gravity"]
            gravity = offset_g(bno_data["gravity"], bno_data["g0"])
            data = {
                "gravity": gravity,
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
    bno_data["g0"] = bno_data["gravity"]
    print("save calibration")
    return "OK"

@app.route("/load_calibration", methods=["POST"])
def load_calibration():
    return "OK"

@app.route("/")
def root():
    return flask.render_template("index-mpu6050.html")

if __name__ == "__main__":
    app.run(host="0.0.0.0", port=80, debug=True, threaded=True)
