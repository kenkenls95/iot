
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include<string>

#include "DHTesp.h"

#define ssid "abc"
#define password "123456789"
#define mqtt_server "m14.cloudmqtt.com"
#define mqtt_topic_pub "Topic"
#define mqtt_topic_sub "Client-parse"
#define mqtt_user "maukmmii"
#define mqtt_pwd "nsSM0k2eJ2z3"

DHTesp dht;

const uint16_t mqtt_port = 15712;
String inString = "";

WiFiClient espClient;// Tạo đối tượng wificlient
PubSubClient client(espClient);// Khai báo là client có thuộc tính của PubSubClient
long lastMsg = 0;
//char jso[];




void setup() {
  Serial.begin(115200);
  dht.setup(D2);
  setup_wifi(); // Hàm tự viết ở dưới để kết nối wifi
  client.setServer(mqtt_server, mqtt_port);// Hàm kết nối vào mqtt server
  client.setCallback(callback);
  pinMode(A0,INPUT);
  pinMode(D4,OUTPUT);
  pinMode(D5,OUTPUT);
  pinMode(D6,OUTPUT);
  pinMode(D7,OUTPUT);
  pinMode(D8,OUTPUT);
}

void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  char parseMsg[80];
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  Serial.print("payload: ");
 for(int i = 0; i<length; i++){
   Serial.print((char)payload[i]);
   parseMsg[i] = (char)payload[i];
 }
  Serial.println();
  
  StaticJsonBuffer<300> jBuffer;
  JsonObject& root = jBuffer.parseObject(parseMsg);
  int led1 = root["led"][0];
  int led2 = root["led"][1];
  int led3 = root["led"][2];
  int led4 = root["led"][3];
  doLed(led1,led2,led3,led4);
  Serial.println("Status :[led 1,led 2,led 3,led 4]");
  Serial.print(led1);
  Serial.print(" - ");
  Serial.print(led2);
  Serial.print(" - ");
  Serial.print(led3);
  Serial.print(" - ");
  Serial.println(led4);
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {// Nếu chưa kết nối
    Serial.print("Attempting MQTT connection..."); // thì in ra dòng này
    // Attempt to connect
    if (client.connect("ESP8266Client",mqtt_user, mqtt_pwd)) { //nếu kết nối đúng 
      Serial.println("connected");// in ra là đã kết nối
      // Once connected, publish an announcement...
      client.publish(mqtt_topic_pub, "ESP_reconnected");// rồi gửi lên broker là ESP_reconnected
      // ... and resubscribe
      client.subscribe(mqtt_topic_sub);// và lại hóng tin từ broker
    } else {
      Serial.print("failed, rc="); // còn nếu không thì in ra cái thất bại
      Serial.print(client.state());// in ra trạng thái của client
      Serial.println(" try again in 5 seconds"); 
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}
void loop() {
  
  char msg[100];
  boolean isSuccess; 
  DynamicJsonBuffer jBuffer; 
  JsonObject& root = jBuffer.createObject();
  JsonArray& led = root.createNestedArray("led");
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  long now = millis();// gán thời gian bây giờ là millis
  if (now - lastMsg > 30000) {// Cái này là cứ 30s 
    float doamdat = 100.0-float(analogRead(A0))/1023.0*100.0;
    float humidity = dht.getHumidity();  
    float temperature = dht.getTemperature();
    if(isnan(humidity) || isnan(temperature)){
      isSuccess = false;
      temperature = 0;
      humidity = 0;
    }else {
      isSuccess = true;      
    }
    lastMsg = now;
    root["temperature"] = temperature;
    root["humidity"] =humidity;
    root["soilmoisture"] = doamdat;
    led.add(digitalRead(D4));
    led.add(digitalRead(D5));
    led.add(digitalRead(D6));
    led.add(digitalRead(D7));
    root["success"] = isSuccess;
    Serial.print("Publish message: ");
    root.prettyPrintTo(Serial);
    root.printTo(msg,sizeof(msg));
    Serial.println();
    client.publish(mqtt_topic_pub,msg);// đẩy lên broker
  }
}

void doLed(int led1, int led2, int led3 , int led4){
    digitalWrite(D4,led1);
    digitalWrite(D5,led2);
    digitalWrite(D6,led3);
    digitalWrite(D7,led4);
  }
