#include <Arduino.h>
#include <Stepper.h>
#include <WiFi.h>

// Replace with your network credentials
const char* ssid = "REPLACE_WITH_YOUR_SSID";
const char* password = "REPLACE_WITH_YOUR_PASSWORD";

// Set web server port number to 80
WiFiServer server(80);

// ULN2003 Motor Driver Pins
#define IN1 19
#define IN2 18
#define IN3 5
#define IN4 17

#define IN5 13
#define IN6 12
#define IN7 4
#define IN8 11

// initialize the stepper library
Stepper myStepper(2048, IN1, IN3, IN2, IN4);
Stepper myStepper2(2048, IN5, IN6, IN7, IN8);

// Motor speeds for forward and backward movement
int motorSpeedForward = 100; // Adjust as needed
int motorSpeedBackward = -100; // Adjust as needed

void setup() {
  Serial.begin(115200);

  // Connect to Wi-Fi network with SSID and password
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  // Print local IP address and start web server
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  server.begin();

  // Set the initial speed of the motors
  myStepper.setSpeed(0);
  myStepper2.setSpeed(0);
}

void loop() {
  WiFiClient client = server.available();   // Listen for incoming clients

  if (client) {
    Serial.println("New Client.");
    String currentLine = "";

    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        Serial.write(c);

        if (c == '\n') {
          if (currentLine.length() == 0) {
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();

            if (currentLine.indexOf("Forward") != -1) {
              Serial.println("Forward");
              myStepper.setSpeed(motorSpeedForward);
              myStepper2.setSpeed(motorSpeedForward);
            } else if (currentLine.indexOf("Backward") != -1) {
              Serial.println("Backward");
              myStepper.setSpeed(motorSpeedBackward);
              myStepper2.setSpeed(motorSpeedBackward);
            } else if (currentLine.indexOf("Left") != -1) {
              Serial.println("Left");
              myStepper.setSpeed(motorSpeedBackward);
              myStepper2.setSpeed(motorSpeedForward);
            } else if (currentLine.indexOf("Right") != -1) {
              Serial.println("Right");
              myStepper.setSpeed(motorSpeedForward);
              myStepper2.setSpeed(motorSpeedBackward);
            } else if (currentLine.indexOf("Stop") != -1) {
              Serial.println("Stop");
              myStepper.setSpeed(0);
              myStepper2.setSpeed(0);
            }

            // HTML response
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
            client.println(".button { background-color: #4CAF50; border: none; color: white; padding: 16px 40px;");
            client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
            client.println(".button2 {background-color: #555555;}</style></head>");

            client.println("<body><h1>Remote Control Car</h1>");
            client.println("<p><a href=\"/Forward\"><button class=\"button\">Forward</button></a></p>");
            client.println("<p><a href=\"/Backward\"><button class=\"button\">Backward</button></a></p>");
            client.println("<p><a href=\"/Left\"><button class=\"button\">Left</button></a></p>");
            client.println("<p><a href=\"/Right\"><button class=\"button\">Right</button></a></p>");
            client.println("<p><a href=\"/Stop\"><button class=\"button button2\">Stop</button></a></p>");
            client.println("</body></html>");
            client.println();
            break;
          } else {
            currentLine = "";
          }
        } else if (c != '\r') {
          currentLine += c;
        }
      }
    }
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }
}
