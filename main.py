import json
import urequests
import network
import machine
import time
from machine import Pin
from time import sleep_us



QX_ADDR='http://192.168.122.1:8080'
params = {
    "method": "",
    "params": [],
    "id": 1,  # move to setting
    "version": "1.0"
}
url = QX_ADDR + "/sony/camera"
revolution_to_len = 40
spacing = 64
json_dump = json.dumps(params)
json_dump_bytes = bytearray(json_dump, 'utf8')

def start():

    params["method"] = "startRecMode"
    json_dump = json.dumps(params)
    json_dump_bytes = bytearray(json_dump, 'utf8')
    r = urequests.request(method = "POST", url=url, data=json_dump_bytes)
    r.close()

def shot():

    params["method"] = "actTakePicture"
    json_dump = json.dumps(params)
    json_dump_bytes = bytearray(json_dump, 'utf8')
    r = urequests.request(method="POST", url=url, data=json_dump_bytes)
    r. close()



def take_photos():

    s.power_on()
    print("power on")
    s.revolution((-spacing/2)/revolution_to_len)
    print("left position")

    shot()
    print("shot 1 taken")
    time.sleep(.1)
    s.revolution((spacing) / revolution_to_len)
    print("right position")
    shot()
    print("shot 1 taken")
    time.sleep(.1)

    s.revolution((-spacing/2)/revolution_to_len)
    print("center position")
    s.power_off()
    print("power off")

class Stepper:
    """Class for stepper motor driven by Easy Driver."""

    def __init__(self, step_pin, dir_pin, sleep_pin):
        """Initialise stepper."""
        # self.stp = step_pin
        # self.dir = dir_pin
        # self.slp = sleep_pin
        #
        # self.stp.init(Pin.OUT)
        # self.dir.init(Pin.OUT)
        # self.slp.init(Pin.OUT)
        self.stp = Pin(step_pin, Pin.OUT)
        self.dir = Pin(dir_pin, Pin.OUT)
        self.slp = Pin(sleep_pin, Pin.OUT)
        self.step_time = 20  # us
        self.steps_per_rev = 3200
        self.current_position = 0

    def power_on(self):
        """Power on stepper."""
        self.slp.value(1)

    def power_off(self):
        """Power off stepper."""
        self.slp.value(0)
        self.current_position = 0

    def steps(self, step_count):
        """Rotate stepper for given steps."""
        self.dir.value(0 if step_count > 0 else 1)
        for i in range(abs(step_count)):
            self.stp.value(1)
            sleep_us(self.step_time)
            self.stp.value(0)
            sleep_us(self.step_time)
        self.current_position += step_count

    def rel_angle(self, angle):
        """Rotate stepper for given relative angle."""
        steps = int(angle / 360 * self.steps_per_rev)
        self.steps(steps)

    def abs_angle(self, angle):
        """Rotate stepper for given absolute angle since last power on."""
        steps = int(angle / 360 * self.steps_per_rev)
        steps -= self.current_position % self.steps_per_rev
        self.steps(steps)

    def revolution(self, rev_count):
        """Perform given number of full revolutions."""
        self.steps(rev_count * self.steps_per_rev)

    def set_step_time(self, us):
        """Set time in microseconds between each step."""
        if us < 20:  # 20 us is the shortest possible for esp8266
            self.step_time = 20
        else:
            self.step_time = us

def do_connect():

    wlan = network.WLAN(network.STA_IF)
    wlan.active(True)
    if not wlan.isconnected():
        print('connecting to network...')
        #wlan.connect('UPC0886495', 'GTJHQCTR')
        wlan.connect('DIRECT-OEE0:ILCE-5100', 'iHjAg5rD')
        while not wlan.isconnected():
            pass
    print('network config:', wlan.ifconfig())

s = Stepper(14,12,5)
pin = machine.Pin(13, machine.Pin.IN)
print("stepper 14,12,5")
print("button 13")

if pin.value():
    print("start loop")
    do_connect()
    start()
    while True:
        if not pin.value():
         take_photos()
else:
    print("configuration mode")