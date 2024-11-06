"""
	Example for 4.2 inch black & white Waveshare E-ink screen
	Run on ESP32


    https://github.com/HelTecAutomation/Heltec_ESP32/blob/a1dfa1d87d07e3dcddb42081e7cf0ef33fbee726/examples/Factory_Test/Wireless_paper_FactoryTest/Wireless_paper_FactoryTest.ino
    https://github.com/HelTecAutomation/Heltec_ESP32/blob/095755261d93051ada4456119a7b413f1aef2bf0/src/HT_QYEG0213RWS800_BWR.h
"""

from lib.epaper2in13 import EPD

from machine import Pin, SPI

# SPIV on ESP32
# factory_display(6, 5, 4,7, 3,2,-1, 6000000);//rst=6,dc=5,cs=4,busy=7,sck=3,mosi=2,miso=no,frequency=6000000
sck = Pin(3)
#miso = Pin(-1)
mosi = Pin(2)
dc = Pin(5)
cs = Pin(4)
rst = Pin(6) 
busy = Pin(7) # eink-busy
spi = SPI(2, baudrate=6000000, polarity=0, phase=0, sck=sck, miso=None, mosi=mosi)

e = EPD(spi, cs, dc, rst, busy)
e.init()

w = 250
h = 122
x = 0
y = 0

# --------------------

# use a frame buffer
# 400 * 300 / 8 = 15000 - thats a lot of pixels
import framebuf
buf = bytearray(w * h // 8)
fb = framebuf.FrameBuffer(buf, w, h, framebuf.MONO_HLSB)
black = 0
white = 1
fb.fill(white)

# --------------------

# write hello world with black bg and white text
fb.text('Hello World',30,0,black)
fb.pixel(30, 10, black)
fb.hline(30, 30, 10, black)
fb.vline(30, 50, 10, black)
fb.line(30, 70, 40, 80, black)
fb.rect(30, 90, 10, 10, black)
fb.fill_rect(30, 110, 10, 10, black)
for row in range(0,36):
	fb.text(str(row),0,row*8,black)
