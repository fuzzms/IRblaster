/*
 * 
 */

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>
#include <WiFiClient.h>

const char* ssid = "";
const char* password = "";
#error Set SSID and WIFI password
MDNSResponder mdns;

ESP8266WebServer server(80);

IRsend irsend(4);  // Two IR LEDs are controlled by GPIO pin 4 (D2)

void handleRoot() {
  server.send(200, "text/html",
              "<html>" \
                "<head><title>ESP8266 IR Blaster (*WIP*)</title></head>" \
                "<body>" \
                  "<h1>Hello from an ESP8266 based Wifi IR blaster. <br>" \
	              "You can send various IR commands from here.</h1>"  \
                  "<p><h3><a href=\"ir?code=30\">Switch to MediaPC</a></h3></p>" \
	          "<p><h3><a href=\"ir?code=31\">Switch to ChromeCast</a></h3></p><br><br>" \
	          "<p><a href=\"ir?code=1\">Send MARANTZ Volume UP</a></p>" \
                  "<p><a href=\"ir?code=2\">Send MARANTZ Volume DOWN</a></p>" \
                  "<p><a href=\"ir?code=3\">Send MARANTZ Volume MUTE</a></p>" \
	          "<p><a href=\"ir?code=4\">Send MARANTZ Source TV</a></p>" \
	          "<p><a href=\"ir?code=5\">Send MARANTZ Source DVD</a></p>" \
	          "<p><a href=\"ir?code=6\">Send MARANTZ Power Off</a></p>" \
                  "<p><a href=\"ir?code=10\">Send SAMSUNG Source HDMI1</a></p>" \
                  "<p><a href=\"ir?code=11\">Send SAMSUNG Source HDMI2</a></p>" \
                  "<p><a href=\"ir?code=12\">Send SAMSUNG Source HDMI3</a></p>" \
                  "<p><a href=\"ir?code=13\">Send SAMSUNG Source HDMI4</a></p>" \
	          "<p><a href=\"ir?code=20\">Send SAMSUNG TV power toggle</a></p>" \
                  "<p><a href=\"ir?code=21\">Send SAMSUNG TV power off</a></p>" \
                  "<p><a href=\"ir?code=22\">Send SAMSUNG TV power on</a></p>" \
	          "<br><br>"\
	      
                "</body>" \
              "</html>");
}

void handleIr() {
  for (uint8_t i = 0; i < server.args(); i++) {
    if (server.argName(i) == "code") {
      uint32_t code = strtoul(server.arg(i).c_str(), NULL, 10);
      switch (code) {
      case 1:
	// Marantz Volume UP
	irsend.sendRC5(3088, 12, 1);
	break;
      case 2:
	// Marantz Volume DOWN
	irsend.sendRC5(1041, 12, 1);
	break;
      case 3:
	// Marantz MUTE
	irsend.sendRC5(1037, 12, 1);
	break;
      case 4:
	// Marantz Source TV
	irsend.sendRC5(0x83F, 12, 3);
	break;
      case 5:
	{  // Marantz Source DVD
	  uint16_t rawData[31] = {878, 888,  1806, 1726,  1812, 844,  878, 888,  878, 892,  874, 4430,  922, 848,  874, 892,  874, 892,  878, 888,  920, 850,  874, 892,  922, 844,  878, 1768,  1764, 1768,  1764};
	  irsend.sendRaw(rawData, 31, 36);
	  break;
	}
      case 6:
	{  // Marantz Power OFF
	  uint16_t rawData[33] = {874, 892,  1836, 820,  874, 892,  946, 820,  878, 888,  946, 824,  874, 4428,  878, 892,  874, 1768,  878, 892,  1760, 892,  922, 844,  878, 888,  922, 848,  874, 892,  916, 1730,  1832};
	  irsend.sendRaw(rawData, 33, 36);
	  break;
	}
      case 10:
	// Samsung Source HDMI1 
	irsend.sendSAMSUNG(0xE0E09768,64,1);
	break;
      case 11:
	// Samsung Source HDMI2 
	irsend.sendSAMSUNG(0xE0E07D82,64,1);
	break;
      case 12:
	// Samsung Source HDMI3
	irsend.sendSAMSUNG(0xE0E043BC,64,1);
	break;
      case 13:
	// Samsung Source HDMI4
	irsend.sendSAMSUNG(0xE0E0A35C,1);
	break;
      case 20:
	/// Send Samsung TV Power toggle
	irsend.sendSAMSUNG(3772793023,64,1);
	break;
      case 21:
	/// Send Samsung TV Power off
	irsend.sendSAMSUNG(0xE0E019E6,64,1);
	break;
      case 22:
	/// Send Samsung TV Power on
	irsend.sendSAMSUNG(0xE0E09966,64,1);
	break;
      case 30:
	// Change to MediaPC
	// Switch TV Source to HDMI 2
	irsend.sendSAMSUNG(0xE0E07D82,64,1);
	// Switch Receiver Source to TV
	irsend.sendRC5(0x83F, 12, 3);
	break;
      case 31:
	// Change to ChromeCast
	// Switch Receiver Source to DVD
	{
	  uint16_t rawData[31] = {878, 888,  1806, 1726,  1812, 844,  878, 888,  878, 892,  874, 4430,  922, 848,  874, 892,  874, 892,  878, 888,  920, 850,  874, 892,  922, 844,  878, 1768,  1764, 1768,  1764};
	  irsend.sendRaw(rawData, 31, 36);
	  delay(400);
	  // Switch TV Source to HDMI 3
	  irsend.sendSAMSUNG(0xE0E043BC,64,1);
	  break;
	}
      }
    }
  }
  handleRoot();
}

void handleNotFound() {
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET)?"GET":"POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++)
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  server.send(404, "text/plain", message);
}

void setup(void) {
  irsend.begin();
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if (mdns.begin("esp8266", WiFi.localIP())) { 
    Serial.println("MDNS responder started");
  }

  server.on("/", handleRoot);
  server.on("/ir", handleIr);

  server.on("/inline", [](){
    server.send(200, "text/plain", "this works as well");
  });

  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");
}

void loop(void) {
  server.handleClient();
}
