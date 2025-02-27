#include <Arduino.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

const char* ssid = "Wokwi-GUEST";
const char* password = "";

const int light = 13;
int buttonLastState = 0;
bool ligthState = false;

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
	<head>
		<meta charset="UTF-8" />
		<meta name="viewport" content="width=device-width, initial-scale=1.0" />
		<title>Document</title>
		<link rel="preconnect" href="https://fonts.googleapis.com" />
		<link rel="preconnect" href="https://fonts.gstatic.com" crossorigin />
		<link
			href="https://fonts.googleapis.com/css2?family=Montserrat:ital,wght@0,100..900;1,100..900&display=swap"
			rel="stylesheet"
		/>
		<style>
			* {
				font-family: 'Montserrat', serif;
				margin: 0;
				padding: 0;
				box-sizing: border-box;
			}
			body {
				width: 100vw;
				height: 100vh;
				display: flex;
				align-items: center;
				justify-content: center;
				background: #f0f0f0;
			}

			main {
				width: 600px;
				height: 700px;
				border-radius: 15px;
				background: #fff;
				box-shadow: rgba(0, 0, 0, 0.25) 0px 4px 15px;
				display: flex;
				flex-direction: column;
				align-items: center;
				justify-content: space-around;
				padding: 40px;
			}
			.container {
				padding: 15px;
				display: flex;
				align-items: center;
				justify-content: center;
				border-radius: 15px;
				overflow: hidden;
				box-shadow: rgba(0, 0, 0, 0.25) 0px 0px 15px;
			}
			h1 {
				font-size: 21px;
				color: #080808;
			}
			.Light__off {
				background: #de8774;
			}
			.Light__on {
				background: #86de74;
			}
			button {
				padding: 0;
				border: none;
				font: inherit;
				color: inherit;
				background-color: transparent;
				cursor: pointer;
				transition: 500ms;
				font-size: 21px;
				font-weight: 600;
				color: #fff;
			}
			button:active {
				opacity: 0.7;
			}
		</style>
	</head>
	<body>
		<main>
			<div class="container">
				<img
					src="https://i.postimg.cc/DZdhrtCt/idea-1.png"
					alt="on light"
					width="150px"
					height="150px"
				/>
			</div>
			<div class="container">
				<h1>Освітлення аудіторій</h1>
			</div>
			<button id="light__button" class="container Light__on">Вімкнути</button>
		</main>
		<script>
			var e=document.getElementById("light__button");var n=!1;window.addEventListener("load",(()=>{const o=new WebSocket("ws://localhost:8180/ws");o.onerror=e=>{console.log("WEBSOCKET_ERROR ",e)},o.onmessage=o=>{n="1"===o.data,e.className="container "+(n?"Light__off":"Light__on"),e.innerHTML=n?"Вимкнути":"Ввімнкути",console.log("1"===o.data&&!0)},e.addEventListener("click",(()=>{o.send("updateLite")}))}));
		</script>
	</body>
</html>
)rawliteral";

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
  switch (type)
  {
  case WS_EVT_CONNECT:
    Serial.printf("ws[%s][%u] connect\n", server->url(), client->id());
    break;
  case WS_EVT_DISCONNECT: 
  Serial.printf("ws[%s][%u] disconnect: %u\n", server->url(), client->id());
  break;
  case WS_EVT_DATA: 
  messageEvent(arg, data, len);
  break;
  
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
    delay(100);
    Serial.print(".");
  }
  
  Serial.println("");
  Serial.println("Connected..!");

  ws.onEvent(eventHandler);
  server.addHandler(&ws);
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html);
  });
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
