 
#include <ESP8266WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"

#define Relay1            D3

#define WLAN_SSID       "Joukahainen"             // Your SSID
#define WLAN_PASS       "r0kr0kr0k"        // Your password

/************************* Adafruit.io Setup *********************************/

#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  1883                   // use 8883 for SSL
#define AIO_USERNAME    "OldFarts"            // Replace it with your username
#define AIO_KEY         "cd75e6f2005a4513a8dd8bb705e2da48"   // Replace with your Project Auth Key
#define MQTT_SERVER      "192.168.1.68"        // static ip address
#define MQTT_PORT         1883                    
#define MQTT_USERNAME    "" 
#define MQTT_PASSWORD         "" 

/************ Global State (you don't need to change this!) ******************/
// Create an ESP8266 WiFiClient class to connect to the MQTT server and adafruit server. 
WiFiClient client;
WiFiClient client2;

// Setup the MQTT client class by passing in the WiFi client and MQTT server and login details. 
Adafruit_MQTT_Client mqtt(&client, MQTT_SERVER, MQTT_PORT, MQTT_USERNAME, MQTT_PASSWORD); 
// Setup the Adafruit client class by passing in the WiFi client and MQTT server and login details. 
Adafruit_MQTT_Client adamqtt(&client2, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY); 


// Notice MQTT paths for AIO follow the form: <username>/feeds/<feedname> 
Adafruit_MQTT_Publish pi_led = Adafruit_MQTT_Publish(&mqtt, MQTT_USERNAME "/leds/pi"); 
// Setup a feed called 'esp8266_led' for subscribing to changes. 
//Adafruit_MQTT_Publish pi_led_aio = Adafruit_MQTT_Publish(&adamqtt, AIO_USERNAME "/feeds/pi"); 

// Setup a feed called 'esp8266_led' for subscribing to changes. 
Adafruit_MQTT_Subscribe esp8266_led = Adafruit_MQTT_Subscribe(&mqtt, MQTT_USERNAME "/leds/esp8266");
// Setup a feed called 'esp8266_led' for subscribing to changes. 
Adafruit_MQTT_Subscribe esp8266_led_aio = Adafruit_MQTT_Subscribe(&adamqtt, AIO_USERNAME "/feeds/esp8266");

/****************************** Feeds ***************************************/


// Setup a feed called 'onoff' for subscribing to changes.
//Adafruit_MQTT_Subscribe Light1 = Adafruit_MQTT_Subscribe(&adamqtt, AIO_USERNAME "/feeds/LightsOnn"); // FeedName

static const uint8_t D3   = 0;

void MQTT_Connect();
void AdaMQTT_Connect();
void ConnectToMQTTs();
void listNetworks();

void setup() {
  Serial.begin(115200);

  pinMode(Relay1, OUTPUT);
  delay(10); 
  digitalWrite(D3, LOW); 
 
  // Connect to WiFi access point.
  Serial.println(); 
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WLAN_SSID);
  Serial.print("Passwd ");
  Serial.println(WLAN_PASS);

  Serial.println("Scanning available networks...");
  listNetworks(); // For debugging purposes
  
  WiFi.begin(WLAN_SSID, WLAN_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  Serial.println("WiFi connected");
  Serial.println("IP address: "); 
  Serial.println(WiFi.localIP());
 

   // Setup MQTT subscription for esp8266_led feed. 
 adamqtt.subscribe(&esp8266_led_aio);
 mqtt.subscribe(&esp8266_led);
}

void loop() {

 // Connect to Adafruit and local MQTT 
 // MQTT_Connect();
 // AdaMQTT_Connect();
 ConnectToMQTTs();
  
  Adafruit_MQTT_Subscribe *subscription;
  Adafruit_MQTT_Subscribe *subscription2;
     
   /*while( subscription2 = adamqtt.readSubscription()){ 
   if (subscription2 == &esp8266_led_aio) { 
     char *message = (char *)esp8266_led_aio.lastread; 
     Serial.print(F("Got: ")); 
     Serial.println(message); 
        // Check if the message was ON, OFF, or TOGGLE. 
     if (strncmp(message, "ON", 2) == 0) {
      Serial.println("Turning lights on");
       // Turn the LED on. 
       digitalWrite(D3, HIGH); 
     } 
     else if (strncmp(message, "OFF", 3) == 0) { 
      Serial.println("Turning lights off") ;
       // Turn the LED off. 
       digitalWrite(D3, LOW); 
     }
   }*/

   while(subscription = mqtt.readSubscription(5000)){
   if (subscription == &esp8266_led) { 
     char *message = (char *)esp8266_led.lastread; 
     Serial.print(F("Got: ")); 
     Serial.println(message); 
        // Check if the message was ON, OFF, or TOGGLE. 
     if (strncmp(message, "ON", 2) == 0) {
      Serial.println("Turning lights on");
       // Turn the LED on. 
       digitalWrite(Relay1, HIGH); 
     } 
     else if (strncmp(message, "OFF", 3) == 0) { 
      Serial.println("Turning lights off") ;
       // Turn the LED off. 
       digitalWrite(Relay1, LOW); 
     }
    }
   }
   while ((subscription2 = adamqtt.readSubscription(5000))) {
    if (subscription2 == &esp8266_led_aio) {
      Serial.print(F("Got: "));
      Serial.println((char *)esp8266_led_aio.lastread);
      int Light1_State = atoi((char *)esp8266_led_aio.lastread);
      digitalWrite(Relay1, !(Light1_State));
      
    }
  }
}

void AdaMQTT_Connect() {
  int8_t ret;

  // Stop if already connected.
  if (adamqtt.connected()) {
    return;
  }

  uint8_t retries = 3;
   while ((ret = adamqtt.connect()) != 0) { // connect will return 0 for connected
    Serial.println(adamqtt.connectErrorString(ret));
    Serial.println("Retrying Adafruit MQTT connection in 5 seconds...");
    adamqtt.disconnect();
    delay(5000);  // wait 5 seconds
    retries--;
    if (retries == 0) {
      // basically die and wait for WDT to reset me
      while (1);
    }
  }
  Serial.println("Adafruit MQTT Connected!");
}
void MQTT_Connect(){
  int8_t ret;
    // Stop if already connected.
  if (mqtt.connected()) {
    return;
  }

  uint8_t retries = 3;
  while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
    Serial.println(mqtt.connectErrorString(ret));
    Serial.println("Retrying MQTT connection in 5 seconds...");
    mqtt.disconnect();
    delay(5000);  // wait 5 seconds
    retries--;
    if (retries == 0) {
      // basically die and wait for WDT to reset me
      while (1);
    }
  }
  Serial.println("MMQTT Connected!");
}
void ConnectToMQTTs(){
  int8_t ret;
  int8_t ret2;
    // Stop if already connected.
  if (mqtt.connected() || adamqtt.connected()) {
    return;
  }
  
  uint8_t retries = 3;
  while ((ret = mqtt.connect() !=0) || (ret2 = adamqtt.connect() != 0)) { // connect will return 0 for connected
    Serial.println("Retrying MQTTs connection in 5 seconds...");
    mqtt.disconnect();
    adamqtt.disconnect();
    delay(5000);  // wait 5 seconds
    retries--;
    if (retries == 0) {
      // basically die and wait for WDT to reset me
      while (1);
    }
  }
  Serial.println("MMQTTs Connected!");
}
  
void printMacAddress() {
  // the MAC address of your Wifi shield
  byte mac[6];                    

  // print your MAC address:
  WiFi.macAddress(mac);
  Serial.print("MAC: ");
  Serial.print(mac[5],HEX);
  Serial.print(":");
  Serial.print(mac[4],HEX);
  Serial.print(":");
  Serial.print(mac[3],HEX);
  Serial.print(":");
  Serial.print(mac[2],HEX);
  Serial.print(":");
  Serial.print(mac[1],HEX);
  Serial.print(":");
  Serial.println(mac[0],HEX);
}

void listNetworks() {
  // scan for nearby networks:
  Serial.println("** Scan Networks **");
  byte numSsid = WiFi.scanNetworks();

  // print the list of networks seen:
  Serial.print("number of available networks:");
  Serial.println(numSsid);

  // print the network number and name for each network found:
  for (int thisNet = 0; thisNet<numSsid; thisNet++) {
    Serial.print(thisNet);
    Serial.print(") ");
    Serial.print(WiFi.SSID(thisNet));
    Serial.print("\tSignal: ");
    Serial.print(WiFi.RSSI(thisNet));
    Serial.print(" dBm");
    Serial.print("\tEncryption: ");
    Serial.println(WiFi.encryptionType(thisNet));
  }
}
