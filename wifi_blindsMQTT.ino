//mostly styolen from https://diygeorge.wordpress.com/2018/04/14/diy-automatic-window-blinds-software/
//with web page and MQTT and NodeRed (using BigTimer node)
#include <EEPROM.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266mDNS.h>
#include <ESP8266WebServer.h>
#include <stepper_motor_28byj48.h> //https://diygeorge.files.wordpress.com/2018/04/28byj48-5v2.zip
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <PubSubClient.h>
#include <IRrecv.h>
#include <IRremoteESP8266.h>
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
ESP8266WebServer http(80);
// blinds setup
StepperMotor28byj48 motor(D8, D7, D6, D5); // D8, D7, D6, D5 output pins for Right motor
StepperMotor28byj48 motor1(D4, D3, D2, 3); //output pins for Left motor
int turns = 0;                          // no of turns to open or close
int blindStatus = 0;                    // 0 = setup, 1 = close, 2 = open
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//MQTT setup
//EDIT THESE LINES TO MATCH YOUR SETUP
#define MQTT_SERVER "192.168.1.212"
const char* ssid = "Dhome";
const char* password = "TiffanyZ7";
char* Topic = "blinds6";
void callback(char* topic, byte* payload, unsigned int length);//callback header
WiFiClient wifiClient;
PubSubClient client(MQTT_SERVER, 1883, callback, wifiClient); 
/////////////////////////////////////////////////////////////////////////////////////////////////////////
void writeEEPROM(int val) {
  EEPROM.write(0, val);
  EEPROM.commit();
}
void openBlinds() {
  if (blindStatus != 2) {
    Serial.print("Opening blinds... ");
    blindStatus = 2;
    writeEEPROM(2);
    openRight();
    delay(50);
    openLeft();
    delay(50);
    Serial.println("done.");
  }
}
void closeBlinds() {
  if (blindStatus != 1) {
    Serial.print("Closing blinds... ");
    blindStatus = 1;
    writeEEPROM(1);
    closeRight();
    delay(50);
    closeLeft();
    delay(50);
    Serial.println("done.");
  }
}
void closeLeft() {

  for (int i = 1; i <= turns; i++) {
    motor1.cwTurn();
  }
}
void openRight() {
  for (int i = 1; i <= turns; i++) {
    motor.ccwTurn();
  }
}
void closeRight() {
  for (int i = 1; i <= turns; i++) {
    motor.cwTurn();
  }
}
void openLeft() {
  for (int i = 1; i <= turns; i++) {
    motor1.ccwTurn();
  }
}
void callback(char* topic, byte* payload, unsigned int length) {
  //convert topic to string to make it easier to work with
  String topicStr = topic;
  char p0 = (char)payload[0];
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  //Print out some debugging info
  Serial.println("Callback update.");
  Serial.print("Topic: ");
  Serial.println(topicStr);
  Serial.print("payload : " );
  Serial.println(payload[0]);
  //  open blinds if the payload is '0' and publish to the MQTT server a confirmation message
  if (p0 == '0') {
    client.publish(Topic, "OPENING");
    turns = 4;
    openBlinds();
  }
  //close blinds if the payload is '1' and publish to the MQTT server a confirmation message
  else if (p0 == '1') {
    client.publish(Topic, "CLOSING");
    turns = 4;
    closeBlinds();
  }
  else if (p0 == '2') {
    turns = 1;
    openRight();
  }
  else if (p0 == '3') {
    turns = 1;
    closeRight();
  }
  else if (p0 == '4') {
    turns = 1;
    openLeft();
  }
else if (p0 == '5') {
  turns = 1;
  closeLeft();
}
else if (p0 == '6') {
  turns = 4;
  openLeft();
}
else if (p0 == '7') {
  turns = 4;
  closeLeft();
}
else if (p0 == '8') {
  turns = 4;
  openRight();
}
else if (p0 == '9') {
  turns = 4;
  closeRight();
}
}
void reconnect() {
  //attempt to connect to the wifi if connection is lost
  if (WiFi.status() != WL_CONNECTED) {
    //debug printing
    Serial.print("Connecting to ");
    Serial.println(ssid);
    //loop while we wait for connection
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
    //print out some more debug once connected
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
  }
  //make sure we are connected to WIFI before attemping to reconnect to MQTT
  if (WiFi.status() == WL_CONNECTED) {
    // Loop until we're reconnected to the MQTT server
    while (!client.connected()) {
      Serial.print("Attempting MQTT connection...");
      // Generate client name based on MAC address and last 8 bits of microsecond counter
      String clientName;
      clientName += "esp8266-";
      uint8_t mac[6];
      WiFi.macAddress(mac);
      clientName += macToStr(mac);
      //if connected, subscribe to the topic(s) we want to be notified about
      if (client.connect((char*) clientName.c_str())) {
        Serial.println("\tMQTT Connected");
        client.subscribe(Topic);
        //  client.subscribe("blinds");
      }
      //otherwise print failed for debugging
      else {
        Serial.println("\tFailed.");
        abort();
      }
    }
  }
}
//generate unique name from MAC addr
String macToStr(const uint8_t* mac) {
  String result;
  for (int i = 0; i < 6; ++i) {
    result += String(mac[i], 16);
    if (i < 5) {
      result += ':';
    }
  }
  return result;
}
void httpOpenRequest() {
  if (blindStatus != 2) {
    http.send(200, "text/html", "SUCCESS, blinds are opening now, please wait...");
    openBlinds();
  } else {
    http.send(200, "text/html", "FAIL, blinds already opened!");
  }
}
void httpCloseRequest() {
  if (blindStatus != 1) {
    http.send(200, "text/html", "SUCCESS, blinds are closing now, please wait...");
    closeBlinds();
  } else {
    http.send(200, "text/html", "FAIL, blinds already closed!");
  }
}
void httpStatusRequest() {
  String sts;
  switch (blindStatus) {
    case 1 : sts = "Blinds are closed";
      client.publish(Topic, "CLOSED"); break;
    case 2 : sts = "Blinds are open";
      client.publish(Topic, "OPEN"); break;
    default: sts = "Blinds are in SETUP mode."; break;
  }
  http.send(200, "text/html", sts);
}
void webpage() {
  char webpage[] =
    "<!DOCTYPE html><html><head>"
    "<title>My Blinds</title>"
    "<meta name='viewport' content='width=device-width, initial-scale=1'>"
    "<script>function jx(vars, div_id) {"
    "var elm = document.getElementById(div_id);"
    "var xhttp = new XMLHttpRequest();"
    "xhttp.onreadystatechange = function() {"
    "if (this.readyState == 4 && this.status == 200) {"
    "elm.innerHTML = this.responseText;}};"
    "xhttp.open('POST', vars, true);"
    "xhttp.setRequestHeader('Content-type', 'application/x-www-form-urlencoded');"
    "xhttp.send(vars); }"
    "function closeBlinds(){ jx('/api/close', 'rasp'); }"
    "function openBlinds(){ jx('/api/open', 'rasp'); }"
    "function checkBlinds(){ jx('/api/status', 'rasp'); }"
    "window.onload = function() { checkBlinds();"
    "setInterval('javascript:checkBlinds();', 5000);};"
    "</script></head><body><center><h1>My Blinds</h1>"
    "<p><button onclick='closeBlinds();'>Close blinds</button>"
    "&nbsp;<button onclick='openBlinds();'>Open blinds</button></p>"
    "<div id='rasp'></div></center></body></html>";
  http.send(200, "text/html", webpage);
}
void start_webserver() {
  http.on("/api/open", HTTP_POST, httpOpenRequest); // http post request to open
  http.on("/api/close", HTTP_POST, httpCloseRequest); // http post request to close
  http.on("/api/status", HTTP_POST, httpStatusRequest); // http post request for status
  http.onNotFound(webpage); // no matter what page we load except the 3 post requests just return one page
  http.begin(); // start server
}
void setup() {
  // Serial.begin(115200);
  EEPROM.begin(512);
  {
    for (int i = 0; i < 255; i++)
      EEPROM.write(i, i);
  }
  motor.begin();
  motor1.begin();
  start_webserver();
  blindStatus = EEPROM.read(0);
  WiFi.begin(ssid, password);
  //attempt to connect to the WIFI network and then connect to the MQTT server
  reconnect();
  //wait a bit before starting the main loop
  delay(2000);
  ArduinoOTA.setHostname("BigTimerMQTTBlinds");
  ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH) {
      type = "sketch";
    } else { // U_FS
      type = "filesystem";
    }
    // NOTE: if updating FS this would be the place to unmount FS using FS.end()
    Serial.println("Start updating " + type);
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) {
      Serial.println("Auth Failed");
    } else if (error == OTA_BEGIN_ERROR) {
      Serial.println("Begin Failed");
    } else if (error == OTA_CONNECT_ERROR) {
      Serial.println("Connect Failed");
    } else if (error == OTA_RECEIVE_ERROR) {
      Serial.println("Receive Failed");
    } else if (error == OTA_END_ERROR) {
      Serial.println("End Failed");
    }
  });
  ArduinoOTA.begin();
  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  //  irrecv.enableIRIn(); //Initialization infrared receiver
}
void loop() {
  http.handleClient();  // handle http requests
  //  is_wifi_connected();  // check if wifi still connected
  //  wait4user();          // handle open/close switches
  //reconnect if connection is lost
  if (!client.connected() && WiFi.status() == 3) {
    reconnect();
  }
  //MUST delay to allow ESP8266 WIFI functions to run
  delay(10);
  ArduinoOTA.handle();
  //maintain MQTT connection
  client.loop();
}
