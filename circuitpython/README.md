# CircuitPython

## on PC whith blinka


rootless
```sh
sudo groupadd gpiod
sudo usermod -a -G dialout -G gpio user

```

/etc/udev/rules.d/90-gpio.rules
```
SUBSYSTEM=="gpio", KERNEL=="gpiochip*", GROUP="gpio", MODE="0660"

```


```sh
sudo apt install -y python3-libgpiod gcc
python -mvenv --.venv
pip install adafruit-blinka
pip install adafruit-circuitpython-bno055
pip3 install adafruit-circuitpython-bno08x

```

hello world python 
```python
import board

i2c = board.I2C()

print([hex(address) for address in i2c.scan()])

```

enable mdns
```
sudo apt-get install avahi-daemon libnss-mdns libnss-mymachines
```


## mpu-6050


x=0.46 / 28%            y=0.20 / 63%            z=9.64 / 2%
