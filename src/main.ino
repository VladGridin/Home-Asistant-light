#include <Arduino.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

const char* ssid = "Wokwi-GUEST";
const char* password = "";

const int light = 13;
int buttonLastState = 0;
bool ligthState = false;
// TODO: fix button reaction
AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

void messageEvent(void *arg, uint8_t *data, size_t len) {
  AwsFrameInfo *info = (AwsFrameInfo*)arg;
  if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
    data[len] = 0;
    if (strcmp((char*)data, "updateLite") == 0) {
      ligthState = !ligthState;
      ws.textAll(String(ligthState));
    }
  }
}
// web socket state handling
void eventHandler(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len) {
  if(type == WS_EVT_CONNECT) {
    Serial.printf("ws[%s][%u] connect\n", server->url(), client->id());
  }
  else if(type== WS_EVT_DISCONNECT) {
    Serial.printf("ws[%s][%u] disconnect: %u\n", server->url(), client->id());
  }
  else if(type == WS_EVT_DATA) {
    messageEvent(arg, data, len);
  }
}


void setup() {
  Serial.begin(9600);
  pinMode(light, OUTPUT);
  pinMode(12, INPUT_PULLUP);

  // Connect to wifi
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  
  Serial.println("");
  Serial.println("Connected..!");

  ws.onEvent(eventHandler);
  server.addHandler(&ws);
  server.begin();
}

void loop() {

  boolean buttonState = !digitalRead(12);
  // on click function
  if(buttonState==1 && buttonLastState==0) {
    buttonLastState=1;
  } if(buttonState==0 && buttonLastState==1) {
    buttonLastState=0;
    ligthState = !ligthState;
    ws.textAll(String(ligthState));
  }

  digitalWrite(light, ligthState ? HIGH : LOW);
  delay(10); 
  ws.cleanupClients();
}
