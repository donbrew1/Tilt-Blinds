#include "0TA.h"
#include <Arduino.h>
#include <IRrecv.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>
#include <IRtext.h>
#include <IRtimer.h>
#include <IRutils.h>
#include <AccelStepper.h>
#include <WiFiClient.h>
#include <ESP8266mDNS.h>
#include <ESP8266WiFi.h>

// Define step constants
#define FULLSTEP 4
#define HALFSTEP 8

// Define Motor Pins (2 Motors used)

#define motorPin1  D5     // Blue   - 28BYJ48 pin 1
#define motorPin2  D6    // Pink   - 28BYJ48 pin 2
#define motorPin3  D7    // Yellow - 28BYJ48 pin 3
#define motorPin4  D8    // Orange - 28BYJ48 pin 4

#define motorPin5  D2     // Blue   - 28BYJ48 pin 1
#define motorPin6  D3     // Pink   - 28BYJ48 pin 2
#define motorPin7  D4     // Yellow - 28BYJ48 pin 3
#define motorPin8  3     // Orange - 28BYJ48 pin 4

// Define two motor objects
// The sequence 1-3-2-4 is required for proper sequencing of 28BYJ48
AccelStepper stepper1(HALFSTEP, motorPin1, motorPin3, motorPin2, motorPin4);
AccelStepper stepper2(HALFSTEP, motorPin5, motorPin7, motorPin6, motorPin8);

int RECV_PIN = D1;
IRrecv irrecv(RECV_PIN);
decode_results results;

unsigned long entry;
// Current time
unsigned long currentTime = millis();
// Previous time
unsigned long previousTime = 0;
// Define timeout time in milliseconds (example: 2000ms = 2s)
const long timeoutTime = 2000;

void setup() {
  setupOTA("Blinds Web");

  // your code
  irrecv.enableIRIn(); //Initialization infrared receiver

  stepper1.setMaxSpeed(1000.0);
  stepper1.setAcceleration(1000.0);
  stepper1.setSpeed(900);


  stepper2.setMaxSpeed(1000.0);
  stepper2.setAcceleration(1000.0);
  stepper2.setSpeed(900);

}

void loop() {
  WiFiClient client = server.available();   // Listen for incoming clients

  //IS THERE A WEB REQUEST
  if (client) {
    stepper1.enableOutputs();
    stepper2.enableOutputs();
    // If a new client connects,

    String currentLine = "";                // make a String to hold incoming data from the client
    currentTime = millis();
    previousTime = currentTime;
    while (client.connected() && currentTime - previousTime <= timeoutTime) { // loop while the client's connected
      currentTime = millis();
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then

        header += c;
        if (c == '\n') {                    // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:


            // turns the GPIOs on and off
            if (header.indexOf("GET /5/on") >= 0) {

              //    OPEN;
              stepper1.runToNewPosition(2049 * 10);
              stepper1.disableOutputs();
              stepper2.runToNewPosition(-2049 * 10);
              stepper2.disableOutputs();

            } else if (header.indexOf("GET /4/on") >= 0) {

              //CLOSE;
              stepper1.runToNewPosition(-2049 * 10);
              stepper1.disableOutputs();
              stepper2.runToNewPosition(2049 * 10);
              stepper2.disableOutputs();

            }


            // Display the HTML web page
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            // CSS to style the on/off buttons
            // Feel free to change the background-color and font-size attributes to fit your preferences
            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
            client.println(".button { background-color: #195B6A; border: none; color: white; padding: 16px 40px;");
            client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
            client.println(".button2 {background-color: #77878A;}</style></head>");

            // Web Page Heading
            client.println("<body><h1>BLINDS Web Server</h1>");


            client.println("<p><a href=\"/5/on\"><button class=\"button\">Open</button></a></p>");

            client.println("<p><a href=\"/4/on\"><button class=\"button\">Close</button></a></p>");


            client.println("</body></html>");

            // The HTTP response ends with another blank line
            client.println();
            // Break out of the while loop
            break;
          } else { // if you got a newline, then clear currentLine
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine

        }

      }

    }

    // Clear the header variable
    header = "";
    // Close the connection
    client.stop();

  }
  //IS THERE A IR SIGNAL
  if (irrecv.decode(&results)) {
    stepper1.enableOutputs();
    stepper2.enableOutputs();


    switch (results.value) {
      // CODES FOR MEMOREX DVD PLAYER FF REW (BOTH) STOP NEC protocol

      case 0xFF0BF4/*both OPEN*/:

        stepper1.runToNewPosition(2049 * 10);
        stepper1.disableOutputs();
        stepper2.runToNewPosition(-2049 * 10);
        stepper2.disableOutputs();

        break;

      case 0xFF8B74/*both CLOSE*/:

        stepper1.runToNewPosition(-2049 * 10);
        stepper1.disableOutputs();
        stepper2.runToNewPosition(2049 * 10);
        stepper2.disableOutputs();

        break;
      case 0xFF49B6 :  //SEARCH LEFT OPEN 1/2 turn
        stepper1.runToNewPosition(2049 * 2);
        stepper1.disableOutputs();
        stepper1.setCurrentPosition(0);

        break;
      case 0xFFCB34: //SETUP RIGHT OPEN 1/2 turn
        stepper2.runToNewPosition(-2049 * 2 );
        stepper2.disableOutputs();

        break;
      case 0xFF43BC:   //ANGLE LEFT 1/2 CLOSE turn
        stepper1.runToNewPosition(-2049 * 2);
        stepper1.disableOutputs();

        break;
      case 0XFF738C:  //ZOOM RIGHT CLOSE 1/2 turn
        stepper2.runToNewPosition(2049 * 2);
        stepper2.disableOutputs();

        break;


    }
    stepper1.setCurrentPosition(0);
    stepper2.setCurrentPosition(0);
    // GET THE NEXT IR VALUE
    irrecv.resume();
  }
  { ArduinoOTA.handle();


  }
}
