#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

/** 
 * TODO: Métricas
 * volume de pessoas por horário
 * ocupação atual
 * horários de fluxo
 * quantidade máxima de pessoas
 * entradas/saídas por minuto
 */

const char* ssid = "Familia_Ribeiro";
const char* password = "100200Ribeiro#";

const char* mqtt_server = "192.168.0.54";
const int mqtt_port = 1883;
const char* mqtt_topic = "monitor/traffic-flow";
const char* mqtt_client_id = "esp8266-traffic-monitor";

const int BUTTON_1_PIN = D3;  // GPIO 0
const int BUTTON_2_PIN = D5; // GPIO 14
const int LED_1_PIN = D6; // GPIO 12
const int LED_2_PIN = D7; // GPIO 13

WiFiClient espClient;
PubSubClient client(espClient);

struct MQTTPayload
{
  int event;
  long timestamp;

};


volatile unsigned long lastPress1 = 0;
volatile unsigned long lastPress2 = 0;

void connectMQTT() {
  while (!client.connected()) {
    Serial.print("Connecting to MQTT...");
    if (client.connect(mqtt_client_id)) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      delay(1000);
    }
  }
}

void setupWiFi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi connected");
}

void IRAM_ATTR handleEnter() {
  if (millis() - lastPress1 > 300) {
    lastPress1 = millis();
    Serial.println("Entrada detectada.");
    digitalWrite(LED_1_PIN, HIGH);
    client.publish(mqtt_topic, "entrada");
    delay(500);
    digitalWrite(LED_1_PIN, LOW);
  }
}

void IRAM_ATTR handleExit() {
  if (millis() - lastPress2 > 300) {
    lastPress2 = millis();
    Serial.println("Saída detectada.");
    digitalWrite(LED_2_PIN, HIGH);
    client.publish(mqtt_topic, "saida");
    delay(500);
    digitalWrite(LED_2_PIN, LOW);
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(BUTTON_1_PIN, INPUT_PULLUP);
  pinMode(BUTTON_2_PIN, INPUT_PULLUP);
  pinMode(LED_1_PIN, OUTPUT);
  pinMode(LED_2_PIN, OUTPUT);

  setupWiFi();
  client.setServer(mqtt_server, mqtt_port);
  connectMQTT();

  attachInterrupt(digitalPinToInterrupt(BUTTON_1_PIN), handleEnter, FALLING);
  attachInterrupt(digitalPinToInterrupt(BUTTON_2_PIN), handleExit, FALLING);

  Serial.println("Sistema de monitoramento iniciado.");
}

void loop() {
  if (!client.connected()) {
    connectMQTT();
  }
  client.loop();
}
