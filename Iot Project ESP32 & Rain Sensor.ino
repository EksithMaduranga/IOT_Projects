// for Arduino Microcontroller
//#define rainAnalog A1
//#define rainDigital 2

// for ESP8266 Microcontroller
//#define rainAnalog A0
//#define rainDigital D1
#include <ESP32Servo.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>
// shantha code
// Replace the next variables with your SSID/Password combination
const char* ssid = "Xperia_1";
const char* password = "12345678";

// Add your MQTT Broker IP address, example:
//const char* mqtt_server = "192.168.1.144";
//const char* mqtt_server = "192.168.245.32";
const char* mqtt_server = "192.168.139.32";

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;



// for ESP32 Microcontroller
#define rainAnalog 32
#define rainDigital 33
Servo myservo;

int pos = 0;    // variable to store the servo position
// Recommended PWM GPIO pins on the ESP32 include 2,4,12-19,21-23,25-27,32-33 
int servoPin = 21;

void setup() {
   Serial.begin(115200);  
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  
  pinMode(rainDigital,INPUT);

	// Allow allocation of all timers
	ESP32PWM::allocateTimer(0);
	ESP32PWM::allocateTimer(1);
	ESP32PWM::allocateTimer(2);
	ESP32PWM::allocateTimer(3);
	myservo.setPeriodHertz(100);    // standard 50 hz servo
	myservo.attach(servoPin, 500, 2400); // attaches the servo on pin 18 to the servo object

 
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








void callback(char* topic, byte* message, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String messageTemp;
  
  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println();

  if(String(topic) == "iot/rain")
  {
    Serial.print("Changing output to : ");
    Serial.print(messageTemp);
     if(messageTemp == "rain"){
      Serial.println("Raining");
    	for(pos = 0; pos <= 180; pos += 1) 
      { 
		    myservo.write(pos);   
		    delay(15);    
      }
    }      
    else if(messageTemp == "notrain")
    {
      Serial.println("Not raining");
      for (pos = 180; pos >= 0; pos -= 1) 
      {
		    myservo.write(pos);
		    delay(15);   
      }
    
  }
  Serial.println();
  Serial.println();
}
}            
  

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP_Client")) {
      Serial.println("connected");
      // Subscribe
      client.subscribe("iot/rain");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}



void loop() {

if (!client.connected()) {
    reconnect();
  }
  client.loop();
  
  int rainAnalogVal = analogRead(rainAnalog);
  int rainDigitalVal = digitalRead(rainDigital);

  //Serial.print(rainAnalogVal);
  //Serial.print("\t");
  //Serial.println(rainDigitalVal);
  //delay(200);
  Serial.print("Rain value : ");
  Serial.println(rainDigitalVal);
  char rain[8];
  dtostrf(rainDigitalVal, 1, 2, rain);
  client.publish("iot/rainsensor", rain);  

  delay(1000);  

}
