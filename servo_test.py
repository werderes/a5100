import machine

p4 = machine.Pin(4)
servo = machine.PWM(p4,freq=50)

def set_angle(angle):
    duty =int(angle/2+38)
    servo.duty(duty)