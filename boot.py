import network
import webrepl
    

def do_accespoint():
    try:
        import usocket as socket
    except:
        import socket

    import network

    import esp
    esp.osdebug(None)

    import gc
    gc.collect()

    ssid = 'Jezus'
    password = 'Genowefa'
    wlan = network.WLAN(network.STA_IF)
    wlan.active(True)
    ap = network.WLAN(network.AP_IF)
    ap.active(True)
    ap.config(essid=ssid, password=password, channel=5)
    while ap.active() == False:
        pass

    print('Connection successful')
    print(ap.ifconfig())


do_accespoint()
webrepl.start()
