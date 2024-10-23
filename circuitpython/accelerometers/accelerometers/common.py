
import board
import time


i2c = board.I2C()  # uses board.SCL and board.SDA

mins = {"x":99, "y":99, "z":99}
maxs = {"x":0, "y":0, "z":0}
deltas = {"x":0, "y":0, "z":0}

def set_delta(p="x", value=0):
    if not value:
        return
    value = abs(value)
    if value < mins[p]:
        mins[p] = value
    if value > maxs[p]:
        maxs[p] = value
    deltas[p] = 100*(maxs[p] - mins[p])/mins[p]


def run(sensor):
    while True:
        x, y, z=sensor.acceleration
        set_delta("x", x)
        set_delta("y", y)
        set_delta("z", z)
        print(f"x={x:.2f} / {deltas['x']:.0f}%", end="\t\t")
        print(f"y={y:.2f} / {deltas['y']:.0f}%", end="\t\t")
        print(f"z={z:.2f} / {deltas['z']:.0f}%", end="\t\t")
        print()
        time.sleep(0.2)
