#include <WiFi.h>
#include <WebServer.h>

#include "Relay.h"

const uint16_t kIrLed = 4;

// Network Settings
const char* ssid = "IoT-Relay";
const char* password = "12345678";

// WebServer
WebServer server(80);

// Relay
#define totalRelay 4
Relay relays[totalRelay] = {
  Relay(16, "Relay 1", [](byte state){
    Serial.print("Relay 1 > ");
    Serial.println(state==HIGH ? "Hidup" : "Mati");
  }),
  Relay(17, "Relay 2", [](byte state){
    Serial.print("Relay 2 > ");
    Serial.println(state==HIGH ? "Hidup" : "Mati");
  }),
  Relay(5, "Relay 3", [](byte state){
    Serial.print("Relay 3 > ");
    Serial.println(state==HIGH ? "Hidup" : "Mati");
  }),
  Relay(18, "Relay 4", [](byte state){
    Serial.print("Relay 4 > ");
    Serial.println(state==HIGH ? "Hidup" : "Mati");
  })
};

// Current time
unsigned long currentTime = millis();
// Previous time
unsigned long previousTime = 0;
// Define timeout time in milliseconds (example: 2000ms = 2s)
const long timeoutTime = 2000;

String getHTML() {
  String html = "<!DOCTYPE html><html>";
  String initData = "";
  for (int i=0; i<totalRelay; i++) {
    Relay relay = relays[i];
    int pin = relay.getPin();
    String pinStr = String(pin);
    int state = relay.getState();
    initData += "'";
    initData += pinStr;
    initData += "'";
    initData += ":{";
    initData += "'name':";
    initData += "'";
    initData += relay.display_name;
    initData += "','state':";
    initData += (state==LOW) ? "true" : "false";
    initData += "},";
    html += "<!--";
    html += pinStr;
    html += " = ";
    html += String(state);
    html += "-->\r\n";
  }
  html += "<head><title>Smart Relay IoT</title>";
  html += "<style>html { font-family: Verdana, Geneva, Tahoma, sans-serif; color: white; margin: 0; }";
  html += "body { background-color: #151722; text-align: center; padding: 2%; }";
  html += ".active { background-color: #47abb8 !important; }";
  html += ".container { text-align: center; align-content: center; align-items: center; }";
  html += ".container>.button { border-bottom: solid #2c3047 2px; display: flex; height: 100px; align-items: center; justify-content: center; text-decoration: none; cursor: pointer; background-color: #151722; transition-duration: 200ms; }";
  html += ".container>.button:hover { background-color: #26293b; }";
  html += "</style></head>";
  html += "<body><h1>ESP32 Web Server</h1><hr color=\"#1f2230\" />";
  html += "<div class=\"container\"></div>";
  html += "<script type=\"text/javascript\" charset=\"utf-8\">";
  html += "var data = {";
  html += initData;
  html += "};";
  html += "var buttons = document.querySelector('.container').children;";
  html += "function createButton(id, name, value) { var button = document.createElement('div'); button.className = 'button'; button.id = id; if (value) button.classList.add('active'); var title = document.createElement('h1'); title.textContent = name; button.appendChild(title); return button; }";
  html += "for (const [k,v] of Object.entries(data)) { var button = createButton(k, v.name, v.state); button.addEventListener('click', function() { data[k].state = !data[k].state; if (data[k].state) { this.classList.add('active'); } else { this.classList.remove('active'); }";
  html += "const formData = new FormData(); formData.append('pin', k); formData.append('status', (data[k].state ? 'on' : 'off')); fetch('/io', { method: 'POST', body: formData }); }); document.querySelector('.container').appendChild(button); }";
  html += "</script></body></html>";

  return html;
}

void setup() {
  Serial.begin(115200);
  Serial.print("Creating Access Point: ");
  Serial.println(ssid);
  WiFi.softAP(ssid, password);
  Serial.print("IP: ");
  Serial.println(WiFi.softAPIP());
  
  server.on("/io", HTTP_POST, []() {
    String pinStr = server.arg(String("pin"));
    String stateStr = server.arg(String("status"));
    byte state = stateStr==String("on") ? LOW : HIGH;
    
    Serial.println(String("Pin "+pinStr+" = "+stateStr));
    for (int i = 0; i < sizeof(relays); i++) {
      int pin = relays[i].getPin();
      if (pin == pinStr.toInt()) {
        relays[i].setState(state);
        state = relays[i].getState();
      }
    }
    server.send(201, "text/json", "{ \"pin\": "+pinStr+", \"status\": "+String(state)+" }");
  });

  server.on("/io", HTTP_GET, []() {
    String pinStr = server.arg(String("pin"));
    int state;
    
    for (Relay relay : relays) {
      int pin = relay.getPin();
      if (pin == pinStr.toInt()) {
        state = relay.getState();
      }
    }
    server.send(200, "text/json", "{ \"pin\": "+pinStr+", \"status\": "+String(state)+" }");
  });

  server.on("/", HTTP_GET, []() {
    Serial.println("Request: GET \"/\"");
    server.send(200, "text/html", getHTML());
  });

  server.begin();
  Serial.println("HTTP Server Started");
}

void loop() {
  server.handleClient();
}
