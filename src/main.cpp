#include <Arduino.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>

const char* ssid     = "rohmxx-GalaxyA51";
const char* password = "hahahihi";

const char*  server = "https://handwash-lxz6xwlfka-et.a.run.app";

String jsonreceived = "";

int state_pump = 0;
long prevMillis = 0;

WiFiClientSecure client;

void init_WiFi(){
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(". ");
    delay(500);
  }
  Serial.print("Connected to ");
  Serial.println(ssid);
}

void POST_UpdateState(int state){
  client.setInsecure();
  if (!client.connect(server, 443)) {
    Serial.println("Connection failed");
    return;
  }
  const char* data = "{\"battery\":24}";
  client.print(String("PUT https://handwash-lxz6xwlfka-et.a.run.app/v1/state/63e53f299fc8d14031593816 HTTP/1.1\r\n") +
                "Host: handwash-lxz6xwlfka-et.a.run.app\r\n" +
                "Content-Type: application/json\r\n" +
                "Content-Length: " + strlen(data) + "\r\n" +
                "Connection: close\r\n\r\n" +
                data);
  while (client.connected()) {
    String line = client.readStringUntil('\n');
    if (line == "\r") {
      break;
    }
  }
  while(client.available()){
    char c = client.read();
    Serial.print(c);
  }
  client.stop();
}

int GET_CurrentState(){
  int state = 0;
  client.setInsecure();
  if(!client.connect(server, 443)){
    Serial.println("Connection failed!");
    return state;
  }
  String msg = "GET https://handwash-lxz6xwlfka-et.a.run.app/v1/state/63e53f299fc8d14031593816 HTTP/1.1\r\n"
              "Host: handwash-lxz6xwlfka-et.a.run.app\r\n"
              "Connection: Close\r\n";
  client.println(msg);
  client.println();
  while (client.connected()) {
    String line = client.readStringUntil('\n');
    if (line == "\r") {
      break;
    }
  }
  jsonreceived = "";
  while(client.available()){
    char c = client.read();
    jsonreceived+=c;
  }
  Serial.println(jsonreceived);
  client.stop();

  // Pass the state from JSON
  if(jsonreceived[17] == 'f'){
    state = 0;
  }
  else if(jsonreceived[17] == 't'){
    state = 1;
  }
  return state;
}

void setup() {
  Serial.begin(115200);
  delay(100);

  pinMode(2, OUTPUT);
  pinMode(4, OUTPUT);

  init_WiFi();
}

void loop() {
  if(WiFi.status() != WL_CONNECTED){
    init_WiFi();
  }
  else{
    if(millis() - prevMillis >= 2000){
      state_pump = GET_CurrentState();
      digitalWrite(2, state_pump);
      prevMillis = millis();
    }
  }
  // digitalWrite(2,1);
  // digitalWrite(4,1);
  // delay(1000);
  // digitalWrite(2,0);
  // digitalWrite(4,0);
  // delay(1000);
}