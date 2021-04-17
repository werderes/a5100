import json
import urequests
import network
import machine
import time
from stepper import Stepper

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

def start():

    params["method"] = "startRecMode"
    json_dump = json.dumps(params)
    json_dump_bytes = bytearray(json_dump, 'utf8')
    urequests.request(method = "POST", url=url, data=json_dump_bytes)


def shot():

    params["method"] = "actTakePicture"
    json_dump = json.dumps(params)
    json_dump_bytes = bytearray(json_dump, 'utf8')
    urequests.request(method="POST", url=url, data=json_dump_bytes)


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


do_connect()
s = Stepper(14,12,5)
pin = machine.Pin(13, machine.Pin.IN)
start()
while True:
    if not pin.value():
        take_photos()