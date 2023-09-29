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

// Variables to store the current position of each motor
long currentPosition1 = 0;
long currentPosition2 = 0;

// Variable to store the target position for each motor
long targetPosition1 = 0;
long targetPosition2 = 0;

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

  // Set the speed at 5 rpm
  myStepper.setSpeed(5);
  myStepper2.setSpeed(5);
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

            if (currentPosition1 < targetPosition1) {
              Serial.println("Left on");
              myStepper.step(1);
              currentPosition1++;
            } else if (currentPosition1 > targetPosition1) {
              Serial.println("Left off");
              myStepper.step(-1);
              currentPosition1--;
            }

            if (currentPosition2 < targetPosition2) {
              Serial.println("Right on");
              myStepper2.step(1);
              currentPosition2++;
            } else if (currentPosition2 > targetPosition2) {
              Serial.println("Right off");
              myStepper2.step(-1);
              currentPosition2--;
            }

            // HTML response
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
            client.println(".button { background-color: #4CAF50; border: none; color: white; padding: 16px 40px;");
            client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
            client.println(".button2 {background-color: #555555;}</style></head>");

            client.println("<body><h1>ESP32 Web Server</h1>");
            client.println("<p>Left - Position " + String(currentPosition1) + "</p>");
            client.println("<p><a href=\"/LeftOn\"><button class=\"button\">ON</button></a></p>");
            client.println("<p><a href=\"/LeftOff\"><button class=\"button button2\">OFF</button></a></p>");

            client.println("<p>Right - Position " + String(currentPosition2) + "</p>");
            client.println("<p><a href=\"/RightOn\"><button class=\"button\">ON</button></a></p>");
            client.println("<p><a href=\"/RightOff\"><button class=\"button button2\">OFF</button></a></p>");

            // Button to start both motors simultaneously
            client.println("<p><a href=\"/BothOn\"><button class=\"button\">Both Motors ON</button></a></p>");
            client.println("<p><a href=\"/BothOff\"><button class=\"button button2\">Both Motors OFF</button></a></p>");

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
