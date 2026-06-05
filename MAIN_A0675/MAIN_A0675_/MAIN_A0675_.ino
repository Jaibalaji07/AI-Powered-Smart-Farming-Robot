// WORKING CODE
#include <LiquidCrystal.h>
#include <Servo.h>
#include "DHT.h"
#include <SoftwareSerial.h>
SoftwareSerial mySerial(A3, A4);
#define dht_pin 2
#define dht_type DHT11
DHT dht(dht_pin, dht_type);

LiquidCrystal lcd(13, 12, 11, 10, 9, 8);

#define echopin 3
#define triggerpin 4

Servo myservo;

long duration;
int frontDistance;
int leftDistance;
int rightDistance;

#define positivefront 6
#define nagativefront 7
#define positiveback A0
#define nagativeback A1
#define RELAY A2
int readUltrasonic();

void straight();
void right();
void left();
void stopMotor();
void relayfun();
void setup() {

  Serial.begin(9600);
  mySerial.begin(9600);
  dht.begin();

  myservo.attach(5);
  myservo.write(90);

  pinMode(echopin, INPUT);
  pinMode(triggerpin, OUTPUT);

  pinMode(positivefront, OUTPUT);
  pinMode(nagativefront, OUTPUT);
  pinMode(positiveback, OUTPUT);
  pinMode(nagativeback, OUTPUT);

  lcd.begin(16, 2);
  lcd.print("AGRI MONITOR");
  delay(2000);
  lcd.clear();
}

void loop() {

  int temperature = (int)dht.readTemperature();
  if (temperature >= 35) {
    relayfun();
  }
  frontDistance = readUltrasonic();
  pinMode(RELAY, OUTPUT);
  lcd.setCursor(0, 0);
  lcd.print("T:");
  lcd.print(temperature);

  lcd.setCursor(8, 0);
  lcd.print("F:");
  lcd.print(frontDistance);
  lcd.print("cm   ");

  if (frontDistance < 10) {

    stopMotor();
    delay(300);

    lcd.setCursor(0, 1);
    lcd.print("Scanning...   ");

    for (int pos = 90; pos <= 180; pos += 2) {
      myservo.write(pos);
      delay(20);
    }
    delay(300);
    leftDistance = readUltrasonic();

    for (int pos = 180; pos >= 30; pos -= 2) {
      myservo.write(pos);
      delay(20);
    }
    delay(300);
    rightDistance = readUltrasonic();


    for (int pos = 30; pos <= 90; pos += 2) {
      myservo.write(pos);
      delay(20);
    }

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("L:");
    lcd.print(leftDistance);
    lcd.print(" R:");
    lcd.print(rightDistance);
    delay(800);


    if (leftDistance > rightDistance) {

      lcd.setCursor(0, 1);
      lcd.print("Turning LEFT  ");
      left();
      delay(4000);
    }
    else {

      lcd.setCursor(0, 1);
      lcd.print("Turning RIGHT ");
      right();
      delay(4000);
    }

    stopMotor();
    delay(300);
  }
  else {

    lcd.setCursor(0, 1);
    lcd.print("Moving Forward");
    straight();
  }
  if (mySerial.available()) {
    char c = mySerial.read();
    if(c=='A'){
      stopMotor();
      relayfun();
    }
  }
  delay(200);
}


int readUltrasonic() {

  digitalWrite(triggerpin, LOW);
  delayMicroseconds(2);

  digitalWrite(triggerpin, HIGH);
  delayMicroseconds(10);

  digitalWrite(triggerpin, LOW);

  duration = pulseIn(echopin, HIGH);

  return duration * 0.034 / 2;
}


void straight() {
  digitalWrite(positivefront, HIGH);
  digitalWrite(nagativefront, LOW);
  digitalWrite(positiveback, HIGH);
  digitalWrite(nagativeback, LOW);
}

void right() {

  digitalWrite(positivefront, LOW);
  digitalWrite(nagativefront, HIGH);
  digitalWrite(positiveback, HIGH);
  digitalWrite(nagativeback, LOW);
}

void left() {
  digitalWrite(positivefront, HIGH);
  digitalWrite(nagativefront, LOW);
  digitalWrite(positiveback, LOW);
  digitalWrite(nagativeback, HIGH);
}

void stopMotor() {
  digitalWrite(positivefront, LOW);
  digitalWrite(nagativefront, LOW);
  digitalWrite(positiveback, LOW);
  digitalWrite(nagativeback, LOW);
}

void relayfun() {
  digitalWrite(RELAY, HIGH);  // Relay ON
  delay(5000);
  digitalWrite(RELAY, LOW);   // Relay OFF

}
