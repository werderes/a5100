
//#include <Stepper.h>
#include <ArduinoJson.h>
#include <AccelStepper.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ArduinoOTA.h>
#include <WiFiUdp.h>
#include <ArduinoHttpClient.h>

ESP8266WiFiMulti wifiMulti;     // Create an instance of the ESP8266WiFiMulti class, called 'wifiMulti'
WiFiUDP Udp;
unsigned int localUdpPort = 4210;  // local port to listen on
char incomingPacket[255];  // buffer for incoming packets
char  replyPacket[] = "Hi there! Got the message :-)";  // a reply string to send back


#define dirYPin 16
#define stepYPin 14
#define dirXPin 5
#define stepXPin 4
#define enableYPin 12
#define enableXPin 2
#define motorInterfaceType 1

const byte interruptPin = 13;

#ifndef STASSID
#define STASSID "Jezus"
#define STAPSK  "Genowefa42"
#endif

const char* ssid = STASSID;
const char* password = STAPSK;

const char* ssid_sony = "Sony";
const char* password_sony = "sonysony";
const char* adress_sony = "http://192.168.122.1:8080/sony/camera";

int fromSerial = 0;
int buttonState = LOW;
int prevButtonState = LOW;
unsigned long previousMillis = 0;
unsigned long currentMillis = 0;

int shotSequence[][2] = {{1, 1}, {2, 1}, {2, 2}, {1, 2}, {0, 2}, {0, 1}, {0, 0}, {1, 0}, {2, 0}, {3, 0}, {3, 1}, {3, 2}, {3, 3}, {3, 2}, {3, 1}, {3, 0}};
int SPRX = 1600;
int SPRY = 1000;
float angleX = 16;
float angleY = 8;
int numberOfSteps = sizeof(shotSequence) / 8                ;
int currentStep = 0;
bool shooting = 0;
bool movingX = 0;
bool movingY = 0;
int pauseOne = 500;
int pauseTwo = 500;
int x = 0;
int y = 0;
AccelStepper stepperX = AccelStepper(motorInterfaceType, stepXPin, dirXPin);
AccelStepper stepperY = AccelStepper(motorInterfaceType, stepYPin, dirYPin);

DynamicJsonDocument doc(2048);

void ICACHE_RAM_ATTR handleInterrupt();

void setup() {
  pinMode(enableXPin, OUTPUT);
  pinMode(enableYPin, OUTPUT);
  digitalWrite(enableXPin, HIGH);
  digitalWrite(enableYPin, HIGH);
  pinMode(interruptPin, INPUT_PULLUP);
  currentMillis = millis();
  previousMillis = currentMillis;

  attachInterrupt(digitalPinToInterrupt(interruptPin), handleInterrupt, RISING);
  StaticJsonDocument<3> arr;
  doc["method"] = "";
  doc["params"] = arr;
  doc["id"] = 1;
  doc["version"] = "1.0";

  Serial.begin(115200);
  delay(10);
  Serial.println('\n');

  wifiMulti.addAP(ssid, password);   // add Wi-Fi networks you want to connect to
  wifiMulti.addAP(ssid_sony, password_sony);   // add Wi-Fi networks you want to connect to

  Serial.println("Connecting ...");
  int i = 0;
  while (wifiMulti.run() != WL_CONNECTED) { // Wait for the Wi-Fi to connect
    delay(250);
    Serial.print('.');
  }
  Serial.println('\n');
  Serial.print("Connected to ");
  Serial.println(WiFi.SSID());              // Tell us what network we're connected to
  Serial.print("IP address:\t");
  Serial.println(WiFi.localIP());           // Send the IP address of the ESP8266 to the computer

  ArduinoOTA.setHostname("Maryja");
  ArduinoOTA.setPassword("Genowefa42");

  ArduinoOTA.onStart([]() {
    Serial.println("Start");
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed");
  });
  ArduinoOTA.begin();
  Serial.println("OTA ready");
  delay(1000);
  stepperX.stop();
  stepperX.setCurrentPosition(0);
  stepperX.setMaxSpeed(3200);
  stepperX.setAcceleration(600);
  stepperY.stop();
  stepperY.setCurrentPosition(0);
  stepperY.setMaxSpeed(3200);
  stepperY.setAcceleration(600);

  WiFi.mode(WIFI_AP_STA);
  //WiFi.disconnect();
  delay(100);

  IPAddress localIp(192, 168, 0, 1);
  IPAddress gateway(192, 168, 0, 1);
  IPAddress subnet(255, 255, 255, 0);

  WiFi.softAPConfig(localIp, gateway, subnet);
  WiFi.softAP("Lenin", "Genowefa42");
  delay(100);
  Serial.println();

  Udp.begin(localUdpPort);
  delay(500);
  Serial.printf("Now listening at IP %s, UDP port %d\n", WiFi.localIP().toString().c_str(), localUdpPort);


}

void loop() {
  ArduinoOTA.handle();
  listenInput();
  listenUdpInput();
  if (shooting) {
    shootSequence();
  }
}

void moveStepperY(int a) {
  digitalWrite(enableYPin, LOW);
  stepperY.setCurrentPosition(0);
  stepperY.moveTo(a);
}
void moveStepperX(int a) {
  digitalWrite(enableXPin, LOW);
  stepperX.setCurrentPosition(0);
  stepperX.moveTo(a);
}
void listenInput() {
  while (Serial.available() > 0) {
    fromSerial = Serial.parseInt();

    if (fromSerial) {
      moveStepperY(fromSerial);
      Serial.println("ok");
      fromSerial = 0;
    }
  }
}

void listenUdpInput() {
  int packetSize = Udp.parsePacket();
  if (packetSize)
  {
    // receive incoming UDP packets
    Serial.printf("Received %d bytes from %s, port %d\n", packetSize, Udp.remoteIP().toString().c_str(), Udp.remotePort());
    int len = Udp.read(incomingPacket, 255);
    if (len > 0)
    {
      incomingPacket[len] = 0;
    }
    Serial.printf("UDP packet contents: %s\n", incomingPacket);

    // send back a reply, to the IP address and port we got the packet from
    Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
    Udp.write(replyPacket);
    Udp.endPacket();
    int steps = String(incomingPacket).toInt();
    Serial.print(steps);
    //moveStepperY(steps);
    shootSequence();
  }
  /* while (Serial.available() > 0) {
     fromSerial = Serial.parseInt();
     Serial.print(fromSerial);
     if (fromSerial) {
       moveStepper(fromSerial);
       fromSerial = 0;
     }
    }*/
}

void runStepper() {
  if (stepperX.distanceToGo() != 0) {
    stepperX.run();
  } else {
    stepperX.stop();
    movingX = 0;
    //digitalWrite(enableXPin, HIGH);
  }
  if (stepperY.distanceToGo() != 0) {
    stepperY.run();
  } else {
    stepperY.stop();
    movingY = 0;
    //digitalWrite(enableXPin, HIGH);
  }
}
/*
  void handleInterrupt() {
  Serial.println("interrupted!");
  moveStepper(200);
  }*/

void start_camera() {


  doc["method"] = "startRecMode";
  String json;
  serializeJson(doc, json);
  
    HTTPClient http;

    // Send request
    http.begin(adress_sony);
    http.POST(json);

    // Read response
    Serial.print(http.getString());

    // Disconnect
    http.end();
  
}

void handleInterrupt() {
  currentMillis = millis();
  if ((currentMillis - previousMillis > 100) && (digitalRead(interruptPin) == HIGH)) {
    previousMillis = currentMillis;
    if (!shooting) {
      
      shooting = 1;
    } else {
      shooting = 0;
    }
  }
}

void shootSequence() {
  for (int n = 0; n < numberOfSteps; n++) {
    if (shooting) {
      Serial.print("step number:");
      Serial.println(n);
      shoot();
      delay(pauseOne);
      x = (shotSequence[(n + 1) % (numberOfSteps)][0] - shotSequence[n % (numberOfSteps)][0]) * floor(angleX * SPRX / 360);
      moveStepperX(x);
      Serial.print("x steps:");
      Serial.println(x);
      y = (shotSequence[(n + 1) % (numberOfSteps)][1] - shotSequence[n % (numberOfSteps)][1]) * floor(angleY * SPRY / 360);
      moveStepperY(y);
      Serial.print("y steps:");
      Serial.println(y);
      Serial.println(" ");
      movingX = 1;
      movingY = 1;
      while (movingX || movingY) {
        runStepper();
      }
      delay(pauseTwo);
    } else {
      Serial.println("break");
      x = (shotSequence[0][0] - shotSequence[n % (numberOfSteps)][0]) * floor(angleX * SPRX / 360);
      moveStepperX(x);
      Serial.print("x steps:");
      Serial.println(x);
      y = (shotSequence[0][1] - shotSequence[n % (numberOfSteps)][1]) * floor(angleY * SPRY / 360);
      moveStepperY(y);
      Serial.print("y steps:");
      Serial.println(y);
      Serial.println(" ");
      movingX = 1;
      movingY = 1;
      while (movingX || movingY) {
        runStepper();
      }
      delay(pauseTwo);
      shooting = 0;
      
      break;
    }
    
  }
  digitalWrite(enableXPin, HIGH);
  digitalWrite(enableYPin, HIGH);
  Serial.println("end of shooting!");
  shooting = 0;
}
void shoot() {
  Serial.println("shot!!");
}
