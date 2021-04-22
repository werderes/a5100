import network

    

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

    ap = network.WLAN(network.AP_IF)
    ap.active(True)
    ap.config(essid=ssid, password=password)

    while ap.active() == False:
        pass

    print('Connection successful')
    print(ap.ifconfig())


do_accespoint()
