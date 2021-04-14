import json
from urllib.request import urlopen
from time import sleep




SSDP_ADDR = "239.255.255.250"  # The remote host
SSDP_PORT = 1900    # The same port as used by the server
SSDP_ST = "urn:schemas-sony-com:service:ScalarWebAPI:1"
PACKET_BUFFER_SIZE = 1024
SSDP_MSG_TEMPLATE = '\r\n'.join((
    'M-SEARCH * HTTP/1.1',
    'HOST: 239.255.255.250:1900',
    'MAN: "ssdp:discover"',
    'MX: {mx_timeout}',
    'ST: {}'.format(SSDP_ST),
    'USER-AGENT: pysony',
    '',
    ''
))


QX_ADDR='http://10.0.0.1:10000'
params = {
    "method": "",
    "params": [],
    "id": 1,  # move to setting
    "version": "1.0"
}


params["method"] = "startRecMode"
url = QX_ADDR + "/sony/camera"
json_dump = json.dumps(params)
json_dump_bytes = bytearray(json_dump, 'utf8')
read = urlopen(url, json_dump_bytes).read()
result = eval(read)
print(result)