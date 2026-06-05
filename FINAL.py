import time
import threading
import math
import cv2
import cvzone
from ultralytics import YOLO
import firebase_admin
from firebase_admin import credentials, db

cred = credentials.Certificate("fir-d280e-firebase-adminsdk-fbsvc-966bcf49e6.json")  
firebase_admin.initialize_app(cred, {
    'databaseURL': 'https://fir-d280e-default-rtdb.firebaseio.com/'
})


def update_firebase(class_name):
    ref = db.reference('/Monitoring')
    data = {
        'id': class_name,
    }
    ref.set(data)


# def update_value(value):
#     try:
#         print(f"Sending value {value} to {AIO_FEED}")
#         client.publish(AIO_FEED, value)
#     except Exception as e:
#         print("Error publishing data:", e)

class VideoStream:
    def __init__(self, src=0):
        self.stream = cv2.VideoCapture(src)
        self.ret, self.frame = self.stream.read()
        self.stopped = False

    def start(self):
        threading.Thread(target=self.update, args=()).start()
        return self

    def update(self):
        while not self.stopped:
            if not self.ret:
                self.stop()
            else:
                self.ret, self.frame = self.stream.read()

    def read(self):
        return self.frame

    def stop(self):
        self.stopped = True
        self.stream.release()

cap = VideoStream('http://10.185.168.48:8080/video').start()

model = YOLO('leaf.pt')
classnames = [
    "Apple Scab Leaf", "Apple leaf", "Apple rust leaf", "Bell_pepper leaf",
    "Bell_pepper leaf spot", "Blueberry leaf", "Cherry leaf", "Corn Gray leaf spot",
    "Corn leaf blight", "Corn rust leaf", "Peach leaf", "Potato leaf", "Potato leaf early blight",
    "Potato leaf late blight", "Raspberry leaf", "Soyabean leaf", "Soybean leaf",
    "Squash Powdery mildew leaf", "Strawberry leaf", "Tomato Early blight leaf",
    "Tomato Septoria leaf spot", "Tomato leaf", "Tomato leaf bacterial spot",
    "Tomato leaf late blight", "Tomato leaf mosaic virus", "Tomato leaf yellow virus",
    "Tomato mold leaf", "Tomato two spotted spider mites leaf", "grape leaf", "grape leaf black rot"
]

frame_skip = 5
frame_count = 0
display_width = 480
display_height = 360

while True:
    frame = cap.read()
    if frame is None:
        print("Failed to grab frame")
        break
    
    frame_count += 1
    if frame_count % frame_skip == 0:
        frame = cv2.resize(frame, (640, 480))
        result = model(frame, stream=True)
        for info in result:
            boxes = info.boxes
            for box in boxes:
                confidence = math.ceil(box.conf[0] * 100)
                Class = int(box.cls[0])

                if confidence > 30:
                    x1, y1, x2, y2 = map(int, box.xyxy[0])
                    cv2.rectangle(frame, (x1, y1), (x2, y2), (0, 0, 255), 5)
                    label = f'{classnames[Class]} {confidence}%'
                    cvzone.putTextRect(frame, label, [x1 + 8, y1 + 100], scale=1.5, thickness=2)
                    print(f'Object: {classnames[Class]}, Confidence: {confidence}%')

                    if classnames[Class] == "Tomato Early blight leaf":
                        update_firebase('A')
                    elif classnames[Class] == "Tomato Septoria leaf spot":
                        update_firebase('A')
                    elif classnames[Class] == "Tomato leaf":
                        update_firebase('A')
                    elif classnames[Class] == "Tomato leaf bacterial spot":
                        update_firebase('A')
                    elif classnames[Class] == "Tomato leaf late blight":
                        update_firebase('A')
                    elif classnames[Class] == "Tomato leaf mosaic virus":
                        update_firebase('A')
                    elif classnames[Class] == "Tomato leaf yellow virus":
                        update_firebase('A')
                    elif classnames[Class] == "Tomato mold leaf":
                        update_firebase('A')
                    elif classnames[Class] == "Tomato two spotted spider mites leaf":
                        update_firebase('A')
                    elif classnames[Class] == "grape leaf":
                        update_firebase('A')
                    elif classnames[Class] == "grape leaf black rot":
                        update_firebase('A')

        display_frame = cv2.resize(frame, (display_width, display_height))
        cv2.imshow('frame', display_frame)

    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

cap.stop()
cv2.destroyAllWindows()