import json
import urequests

QX_ADDR='http://192.168.122.1:8080'
params = {
    "method": "",
    "params": [],
    "id": 1,  # move to setting
    "version": "1.0"
}
url = QX_ADDR + "/sony/camera"


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
