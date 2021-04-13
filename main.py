def do_connect():
    import network
    wlan = network.WLAN(network.STA_IF)
    wlan.active(True)
    if not wlan.isconnected():
        print('connecting to network...')
        wlan.connect('UPC0886495', 'GTJHQCTR')
        # wlan.connect('DIRECT-OEE0:ILCE-5100', 'iHjAg5rD')
        while not wlan.isconnected():
            pass
    print('network config:', wlan.ifconfig())
do_connect()