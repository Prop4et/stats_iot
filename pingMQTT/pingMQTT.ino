#include <WiFi.h>
#include <WiFiUdp.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>


const char *mqtt_broker = "18.191.129.230";
const int mqtt_port = 1883;
const char *mqtt_username = "Prop4et";
const char *mqtt_password = "Progetto_IoT";
const char *ping_topic = "ping/";
const char *ping_topic_count = "count/";
const char* client_id = "esp32";

long pingSum = 0; //total rtt of the ping packets
long timeSend = 0; //time of sending the ping packet
bool received = true;//received a ping packet or not
int npingsmqtt = 0; //number of ping packets received, max 10
float sentpingmqtt = 0;
float nratiomqtt = 0;

char ssid[] = "SbalziOrmonaliA2.4G";
char pwd[] = "Lovegang126";

int status = WL_IDLE_STATUS;

//**********************************MQTT vars**********************************************
WiFiClient client;
PubSubClient mqttClient(client); //mqttClient for the mqtt publish subscribe
//**********************************MQTT functions*****************************************
//callback when receiving mqtt response
void callback_response_mqtt(char *topic, byte *payload, unsigned int length) {
    if(!strcmp(topic, ping_topic)){
        pingSum += millis() - timeSend;
        received = !received;
        npingsmqtt++;
    }
    if(!strcmp(topic, ping_topic_count)){
        sentpingmqtt++;
    }
}
//function for connecting to the mqtt mqttClient
void mqtt_connect(){
    mqttClient.setServer(mqtt_broker, mqtt_port);
    mqttClient.setCallback(callback_response_mqtt); // setup the callback for the mqttClient connection (MQTT) 
    while (!mqttClient.connected()) {
        Serial.printf("Trying to connect to the mqtt broker ... ");
        if (mqttClient.connect(client_id, mqtt_username, mqtt_password)) {
            mqttClient.subscribe(ping_topic);
            mqttClient.subscribe(ping_topic_count);
            Serial.println("Connected");
        }else{
            Serial.print("Connection failed with state ");
            Serial.print(mqttClient.state());
            delay(2000);
        }
    } 
}


void setup(){
    //serial output
	Serial.begin(115200);
    
    //WIFI CONNECTION
    Serial.print("Attempting to connect to: ");
    Serial.println(ssid);
	while (status != WL_CONNECTED){
		status = WiFi.begin(ssid, pwd);
        Serial.print(".");
		delay(5000);
	}
	Serial.print("\nConnected with ip: ");
	Serial.println(WiFi.localIP());
    delay(250);

    int err = 0;
    mqtt_connect();
}


long lastMsg = 0;
int tries = 0;

void loop(){
    if(WiFi.status() != WL_CONNECTED){
        WiFi.reconnect();
        while (WiFi.status() != WL_CONNECTED) {
            delay(5000);
            Serial.print(".");
        }
    }
    //process mqtt keepalive and check if there is an update on the different parameters with the callback
    mqttClient.loop();
    //WiFi stats 
    if(i<30)
        if(received && npingsmqtt< 10){
            received = !received;
            timeSend = millis();
            
            mqttClient.publish(ping_topic, "ping");
        }else {
            float pingms = pingSum / npingsmqtt;
            Serial.print("Done ping for the ");
            Serial.print(tries);
            Serial.print(" time, value is ");
            Serial.print(pingms);
            Serial.println(" ms");

            //reinitialization
            long pingSum = 0; //total rtt of the ping packets
            long timeSend = 0; //time of sending the ping packet
            bool received = true;//received a ping packet or not
            int npingsmqtt = 0; //number of ping packets received, max 10
            float sentpingmqtt = 0;
            float nratiomqtt = 0;
            i++;
        }
    else
        Serial.println("you can turn me off");
    /*if(npingsmqtt >= 10){
        if(nratiomqtt < 20){
            mqttClient.publish(ping_topic_count, "ping");
            delay(pingSum / (npingsmqtt));
            nratiomqtt++;
        }else{
            float ratiomqtt = sentpingmqtt/nratiomqtt; 
        }
    }*/
}
