
//#include <Stepper.h>
#include <ArduinoJson.h>
#include <AccelStepper.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ArduinoOTA.h>
#include <WiFiUdp.h>

ESP8266WiFiMulti wifiMulti;     // Create an instance of the ESP8266WiFiMulti class, called 'wifiMulti'
WiFiUDP Udp;
unsigned int localUdpPort = 4210;  // local port to listen on
char incomingPacket[255];  // buffer for incoming packets
char  replyPacket[] = "Hi there! Got the message :-)";  // a reply string to send back


#define dirXPin 16
#define stepXPin 14
#define dirYPin 5
#define stepYPin 4
#define enableXPin 12
#define enableYPin 2
#define motorInterfaceType 1

#ifndef STASSID
#define STASSID "Jezus"
#define STAPSK  "Genowefa42"
#endif

const char* ssid = STASSID;
const char* password = STAPSK;

  
int fromSerial = 0;

AccelStepper stepperX = AccelStepper(motorInterfaceType, stepXPin, dirXPin);
AccelStepper stepperY = AccelStepper(motorInterfaceType, stepYPin, dirYPin);

void setup() {
  pinMode(enableXPin, OUTPUT);
  pinMode(enableYPin, OUTPUT);
  digitalWrite(enableXPin, HIGH);
  digitalWrite(enableYPin, HIGH);
  Serial.begin(115200);
    delay(10);
  Serial.println('\n');

  wifiMulti.addAP(ssid, password);   // add Wi-Fi networks you want to connect to
  
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
  stepperX.setMaxSpeed(360);
  stepperX.setAcceleration(200);
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
  runStepper();
}

void moveStepper(int a) {
  digitalWrite(enableYPin, LOW);
  stepperY.setCurrentPosition(0);
  stepperY.moveTo(a);
}

void listenInput() {
  while (Serial.available() > 0) {
    fromSerial = Serial.parseInt();

    if (fromSerial) {
      moveStepper(fromSerial);
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
    moveStepper(steps);
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
  if (stepperY.distanceToGo() != 0) {
    stepperY.run();
  } else {
    stepperY.stop();
    digitalWrite(enableYPin, HIGH);
  }
}
