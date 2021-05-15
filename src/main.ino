#include <Arduino.h>
#include <Ethernet.h>
#include <DHT.h>
#include <PubSubClient.h>
#include <IRremote.h>


//Raw (74): -32150 32150 -32150 32150 -32150 32150 -32150 32150 -32150 32150 -32150 32150 -32150 32150 -32150 32150 -32150 32150 -32150 32150 -32150 32150 -32150 32150 -32150 32150 -32150 32150 -32150 32150 -32150 32150 -32150 32150 -32150 32150 -32150 32150 -32150 32150 -32150 32150 -32150 32150 -32150 32150 -32150 32150 -32150 32150 -32150 32150 -32150 32150 -32150 32150 -32150 32150 -32150 32150 -32150 32150 -32150 32150 -32150 32150 -32150 32150 -32150 32150 -32150 32150 -32150 32150 
// My air conditioner refrigeration status
//Raw (74): -32150 32150 -32150 32150 -32150 32150 -32150 32150 -32150 32150 -32150 32150 -32150 32150 -32150 32150 -32150 32150 -32150 32150 -32150 32150 -32150 32150 -32150 32150 -32150 32150 -32150 32150 -32150 32150 -32150 32150 -32150 32150 -32150 32150 -32150 32150 -32150 32150 -32150 32150 -32150 32150 -32150 32150 -32150 32150 -32150 32150 -32150 32150 -32150 32150 -32150 32150 -32150 32150 -32150 32150 -32150 32150 -32150 32150 -32150 32150 -32150 32150 -32150 32150 -32150 32150 
// Automatic status of my air conditioner


byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
IPAddress ip(192,168,0,198);  // <- change to match your network

#define DHTPIN 7
// DHT11 sensor type
#define DHTTYPE DHT11
// see the flask app 
#define card_id  1  
// delay 
#define wait 5000


// your mqtt_server broker and topic to connect to  
const char* mqtt_server = "192.168.0.197";    // use ifconfig on Linux 
const char* topic = "test"; 

// create objects, instances
DHT dht(DHTPIN, DHTTYPE);
EthernetClient ethClient;
PubSubClient client(ethClient);

long lastMsg = 0;     // time last message was published
char msg[50];         // lenght of the message

int IR_SEND_PIN=3;
IRsend irsend(IR_SEND_PIN);  //set ir send port
const uint16_t air_cold[] = {32150,32150,32150,32150,32150,32150,32150,32150,32150,32150,32150,32150,32150,32150,32150,32150,32150,32150,32150,32150,32150,32150,32150,32150,32150,32150,32150,32150,32150,32150,32150,32150,32150,32150,32150,32150,32150,32150,32150,32150,32150,32150,32150,32150,32150,32150,32150,32150,32150,32150,32150,32150,32150,32150,32150,32150,32150,32150,32150,32150,32150,32150,32150,32150,32150,32150,32150,32150,32150,32150,32150,32150,32150,32150};
const uint16_t air_auto[] = {32150,32150,32150,32150,32150,32150,32150,32150,32150,32150,32150,32150,32150,32150,32150,32150,32150,32150,32150,32150,32150,32150,32150,32150,32150,32150,32150,32150,32150,32150,32150,32150,32150,32150,32150,32150,32150,32150,32150,32150,32150,32150,32150,32150,32150,32150,32150,32150,32150,32150,32150,32150,32150,32150,32150,32150,32150,32150,32150,32150,32150,32150,32150,32150,32150,32150,32150,32150,32150,32150,32150,32150,32150,32150};


// used to see the published messaged has arrived 
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
  Serial.println("------------------------------------------------------------");
  Serial.println(); 
}
void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("arduinoClient")) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("outTopic","hello world");
      // ... and resubscribe
      client.subscribe("inTopic");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}
void setup()
{
  Serial.begin(9600);
  Ethernet.begin(mac, ip);
  dht.begin();
  //client.setServer(mqtt_server, 1883);
  // client.setCallback(callback);
  // Allow the hardware to sort itself out
  delay(1500);
}

void loop() {

  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  long now = millis();
  if (now - lastMsg > wait) {  // delay 5000 
     lastMsg = now;
    // Reading temperature and humidity
    double h = dht.readHumidity();
    // Read temperature as Celsius
    double t = dht.readTemperature();
    String t_str = String(t);
    String h_str = String(int(h));
    int gaz;
    gaz=analogRead(0);
    // Check if any reads failed and exit early (to try again).
    if (isnan(h) || isnan(t) ) {
    Serial.println("Failed to read from DHT sensor!");
    return;
    }
  
    // Display data
    Serial.print("Humidity: ");
    Serial.print(h);
    Serial.print(" %\t");
    Serial.print("Temperature: ");
    Serial.print(t);
    Serial.println(" *C "); 
    snprintf (msg, 50, "%d %s %s %d", card_id, t_str.c_str(), h_str.c_str(), gaz);  // to sytle the message to send exmp : "1 23.00 54"
    Serial.print("Publish message: ");
    Serial.println(msg);
    if (h < 28.0){
      Serial.print("The air conditioning is check auto");
      irsend.sendRaw(air_auto,sizeof(air_auto)/sizeof(air_auto[0]),38);  // (data,length.khz(38))
      delay(5000);
    }
    if(h > 35.0){
      Serial.print("The air conditioning is check auto");
      irsend.sendRaw(air_cold,sizeof(air_cold)/sizeof(air_cold[0]),38);
      delay(5000);
    }
  client.publish(topic, msg);  /// publish the msg
  }
}