# written by jun heng
# integrated by chuan an

import paho.mqtt.client as mqtt
import numpy as np
import json
import cv2
import joblib
import tensorflow as tf
import pymongo
from tensorflow.keras.models import load_model
from tensorflow.python.keras import backend as K
from PIL import Image
import os
import sys
from sklearn.model_selection import train_test_split
from sklearn.neighbors import KNeighborsClassifier
import sklearn.neighbors._base
sys.modules['sklearn.neighbors.base'] = sklearn.neighbors._base

# MQTT_SERVER = "192.168.123.246"
MQTT_SERVER = "localhost"
MQTT_PATH = "Image"
MQTT_PATH_PI = "PI"
MQTT_PATH_WEMOS = "WEMOS"
MQTT_PATH_TELEBOT = "Telebot"

client = pymongo.MongoClient('')# removed for security reasons
db = client.get_database('wemosvending_db')

session = tf.compat.v1.Session()
graph = tf.compat.v1.get_default_graph()

model_cnn = None
model_knn = None
model_rf = None

classes = ["10c", "20c", "5c"]
#classes = {"10c":0, "20c":1, "5c":2}

MODEL_NAME_CNN = "cnn-combined_updated.hd5"
MODEL_NAME_RF = "rf.pkl"
MODEL_NAME_KNN = "knn_hough.pkl"

# The callback for when the client receives a CONNACK response from the server.


def on_connect(client, userdata, flags, rc):
    print("Connected with result code "+str(rc))
    # receieve PI image
    client.subscribe(MQTT_PATH_PI + "/imagedata")

    # receive WEMOS command
    client.subscribe(MQTT_PATH_WEMOS + "/detectcoin")

    # The callback for when a PUBLISH message is received from the server.
    client.subscribe(MQTT_PATH_TELEBOT + "/dispense")

def on_message(client, userdata, msg):
    if msg.topic == "WEMOS/detectcoin":
        print(msg.payload.decode("utf-8"))
        if (msg.payload.decode("utf-8") == 6969):
            print("MESSGE SENT")
            client.publish(MQTT_PATH_TELEBOT + "/machinestolen")
        elif (msg.payload.decode("utf-8") == '6969'):
            print("ESSAGEGE SENT")
            client.publish(MQTT_PATH_TELEBOT + "/machinestolen")
        else:
            print(msg.topic)
            takepicture_pi(client)
    if msg.topic == "PI/imagedata":
        print(msg.topic)
        f = open('output.jpg', 'wb')
        f.write(msg.payload)
        print("Image Received")
        f.close()
        cnn = load_image_cnn("output.jpg")
        knn = load_image_knn("output.jpg")
        rf = load_image_rf("output.jpg")
        result = classify_coin(cnn, knn, rf)
        send_result_wemos(client, result)
    print(msg.topic)
    if msg.topic == "WEMOS/machinestolen":
        print("VENDING MACHINE REPED")
    
    if msg.topic == "Telebot/dispense":
        message = msg.payload.decode("utf-8")
        if (message == "Snack 1"):
            print("SNACK 1 SENT")
            client.publish(MQTT_PATH_WEMOS + "/dispense", 1)
        if (message == "Snack 2"):
            print("SNACK 2 SENT")
            client.publish(MQTT_PATH_WEMOS + "/dispense", 2)
        if (message =="Jackpot"):
            print("JACKPOT SENT")
            client.publish(MQTT_PATH_WEMOS + "/dispense", 3)



def load_image_cnn(image_frame):
    img = Image.open(image_frame)
    img = img.rotate(4)
    area = (400, 250, 950, 800)
    img = img.crop(area)
    pil_image = img.convert('RGB')
    open_cv_image = np.array(pil_image)
    resized_image = open_cv_image[:, :, ::-1].copy()
    resized_image = cv2.resize(resized_image, (300, 300))
    resized_image = np.expand_dims(resized_image, axis=0)
    return resized_image


def load_image_knn(image_frame):
    img = Image.open(image_frame)
    img = img.rotate(4)
    area = (400, 250, 950, 800)
    img = img.crop(area)

    pil_image = img.convert('RGB')
    open_cv_image = np.array(pil_image)
    # Convert RGB to BGR 
    resized_image = open_cv_image[:, :, ::-1].copy() 

    # Convert to grayscale.
    gray = cv2.cvtColor(resized_image, cv2.COLOR_BGR2GRAY)

    # Blur using 3 * 3 kernel.
    gray_blurred = cv2.blur(gray, (3, 3))

    # Apply Hough transform on the blurred image.
    detected_circles = cv2.HoughCircles(gray_blurred, 
                   cv2.HOUGH_GRADIENT, 1, 50, param1 = 100,
               param2 = 100, minRadius = 90, maxRadius = 300)

    a = None
    b = None
    r = None
    # Draw circles that are detected.
    print(detected_circles)
    if detected_circles is not None:
        print("CIRCLE DETECTED")
    # Convert the circle parameters a, b and r to integers.
        detected_circles = np.uint16(np.around(detected_circles))

        for pt in detected_circles[0,:]:
            a, b, r = pt[0], pt[1], pt[2]
            break
    print(r)
    try:
        x_test = r.reshape(-1,1)
    except Exception as e:
        print(e)
        x_test = np.uint16(152).reshape(-1,1);
        print("CODE FKED UO")
    return x_test


def load_image_rf(image_frame):
    img = Image.open(image_frame)
    img = img.rotate(4)
    area = (400, 250, 950, 800)
    img = img.crop(area)
    pil_image = img.convert('RGB')
    open_cv_image = np.array(pil_image)
    resized_image = open_cv_image[:, :, ::-1].copy()
    resized_image = cv2.resize(resized_image, (300, 300))
    resized_image = np.expand_dims(resized_image, axis=0)
    nsamples, nx, ny, nrgb = resized_image.shape
    x_test = resized_image.reshape((nsamples,nx*ny*nrgb))
    return x_test

def send_result_wemos(client, result):
    res = 5
    if result == 1: res=20
    if result == 0: res=10
    client.publish(MQTT_PATH_WEMOS + "/coindone", res)
    print(str(res) + " sent back to WEMOS")

def takepicture_pi(client):
    client.publish(MQTT_PATH_PI + "/TakePicture", "Take Picture")
    print("Take Picture command sent")


def setup():
    global db
    global client
    print("connecting to client")
    testcollection = db["snacks_info"]
    #mytest = [
    #  {"_id":2, "name":"troll"},
    #]
    #db.testcollection.insert_many(mytest)
    client = mqtt.Client()
    client.on_connect = on_connect
    client.on_message = on_message
    client.connect(MQTT_SERVER, 1883, 60)
    client.loop_start()
    client.connect(MQTT_SERVER)
    return client


def classify_coin(cnn, knn, rf):
    global session
    global model_cnn, model_knn, model_rf
    global graph
    print("start predicting")
    with session.as_default():
        with graph.as_default():
            K.set_session(session)
            res_cnn = model_cnn.predict(cnn)
            res_rf = model_rf.predict(rf)
            res_knn = model_knn.predict(knn)
            y_prob_dec_cnn = [format(float(prob), '.8f') for prob in res_cnn[0]]
            print('Probability CNN:',y_prob_dec_cnn)
            y_classes_cnn = res_cnn.argmax(axis=-1)
            print('Predicted class CNN:', y_classes_cnn, '\n')
            class_cnn = y_classes_cnn[0]

            y_prob_dec_rf = [format(float(prob), '.8f') for prob in res_rf[0]]
            print('Probability RF:',y_prob_dec_rf)
            y_classes_rf = res_rf.argmax(axis=-1)
            print('Predicted class RF:', y_classes_rf, '\n')
            class_rf = y_classes_rf[0]

            y_prob_dec_knn = [format(float(prob), '.8f') for prob in res_knn[0]]
            print('Probability KNN:',y_prob_dec_knn)
            y_classes_knn = res_knn.argmax(axis=-1)
            print('Predicted class KNN:', y_classes_knn, '\n')
            class_knn = y_classes_knn[0]
            #themax = np.argmax(result)
            #win = themax
    print("Done")
    #print(classes[win])
    #return classes[win]
    result = weighted_voting(class_cnn, class_knn, class_rf)
    return result


def weighted_voting(cnn_pred,knn_pred,rf_pred):
    cnn_weight = 0.25
    knn_weight = 0.5
    rf_weight = 0.25
    model = {}
    try:
        model[cnn_pred] += cnn_weight
    except:
        model[cnn_pred] = cnn_weight
    try:
        model[knn_pred] += knn_weight
    except:
        model[knn_pred] = knn_weight
    try:
        model[rf_pred] += rf_weight
    except:
        model[rf_pred] = rf_weight

    max_val = max(model, key=model.get)
    min_val = min(model, key=model.get)
    print(model)
    if max_val == min_val and len(model) != 1: #if there is a tiebreak, use knn results since rf can be very wrong
        return knn_pred
    else:
        return max_val

def main():
    global session
    global model_knn, model_cnn, model_rf
    global graph
    with session.as_default():
        with graph.as_default():
            Client = setup()
            K.set_session(session)
            model_cnn = load_model(MODEL_NAME_CNN)
            model_knn = joblib.load(MODEL_NAME_KNN)
            model_rf = joblib.load(MODEL_NAME_RF)
            # send_command(Client)
            while True:
                user_input = input("Type your shit")
                if (user_input == "k"):
                    takepicture_pi(Client)
                pass


if __name__ == '__main__':
    main()
