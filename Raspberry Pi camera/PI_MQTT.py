#written by mayank

import paho.mqtt.client as mqtt
MQTT_SERVER = "34.143.248.115"
MQTT_PATH = "PI"

from picamera import PiCamera
from time import sleep

def on_connect(client, userdata, flags, rc):
    if rc == 0:
        print("Successfully connected to broker.")
    else:
        print("Connection failed with code: %d." %rc)
    client.subscribe(MQTT_PATH + "/TakePicture")

def on_message(client, userdata, msg):
    print("Take Picture Command Received")
    camera = PiCamera()
    camera.start_preview()
    camera.capture('image_test.jpg')
    camera.stop_preview()
    camera.close()

    f=open("image_test.jpg", "rb")
    fileContent = f.read()
    f.close()
    byteArr = bytearray(fileContent)
    client.publish(MQTT_PATH + "/imagedata", byteArr)
    print("Picture successfully sent")    

def setup():
    client = mqtt.Client()
    client.on_connect = on_connect
    client.on_message = on_message
    # client.tls_insecure_set(True)
    client.connect(MQTT_SERVER, 1883, 60)
    client.loop_start()
    return client

def main():
    client = setup()
    while True:
        pass
#Client.loop_forever()

if __name__ == '__main__':
    main()
