import os
interface = 'wlp2s0'
name = 'UPC0886495'
password = 'password'
os.system('iwconfig ' + interface + ' essid ' + name + ' key ' + password)