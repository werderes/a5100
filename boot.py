import network
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
    

def do_accespoint():
    ap = network.WLAN(network.AP_IF)
    ap.active(True)
    ap.config(essid = 'Jezus')


do_accespoint()
do_connect()