import give_camera
import serial
import time

arduino = serial.Serial('/dev/ttyACM0', 115200)
camera = give_camera.give_camera()

def slide(steps):
    steps = bytes(steps, "UTF-8")
    arduino.write(steps)

def shot(steps,steps1):
    global camera
    camera.startRecMode()
    time.sleep(1)
    slide(steps)
    time.sleep(2)
    camera.actTakePicture()
    #time.sleep(.5)
    slide(steps1)
    time.sleep(3)
    camera.actTakePicture()
    #time.sleep(.5)
    slide(steps)
def shot1(steps,steps1):
    global camera
    camera.startRecMode()
    time.sleep(1)
    slide(steps)
    time.sleep(2)
    camera.actTakePicture()
    slide(steps1)
    time.sleep(2)
    camera.actTakePicture()
    slide(steps1)
    time.sleep(2)
    camera.actTakePicture()
    slide(steps1)
    time.sleep(2)
    camera.actTakePicture()
    time.sleep(2)
    slide(steps)
