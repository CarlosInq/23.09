#include <ArduinoJson.h>
#include "EspMQTTClient.h"
#include <Wire.h>

//config. MQTT
EspMQTTClient client
(
  "PICSimLabWifi",//rede Wi-Fi
  "",             //senha
  "mqtt.tago.io", //MQTT Broker
  "Default",      //username
  "f77c1361-fe19-4a51-9d2d-170f2d5b26b6",    //Token
  "TestClient",   //Client name id
  1883            //MQTT port
);



// DEFINIÇÕES DE PINOS
#define pinSensorFogo 4

#define pinBuzzer 27

// DEFINIÇÕES
#define FOGO LOW

int Fog;


// DECLARAÇÃO DE FUNÇÕES
void disparaSirene(byte pin, int intervalo);
void desligaSirene(byte pin);

void setup() {
  Serial.begin(9600);
  pinMode(pinSensorFogo, INPUT);
  pinMode(pinBuzzer, OUTPUT);

  Serial.println("Esperando o Sensor se Adaptar");
  delay(5000);

  Serial.println("Fim do setup()");
}

void onConnectionEstablished()
{}

char buffer[100];

void loop() {
  client.loop();

  bool leituraSensor = digitalRead(pinSensorFogo);
  
   if ( leituraSensor == FOGO) {
    disparaSirene(pinBuzzer, 500);
    int Fogo = 1;
  } else {
    desligaSirene(pinBuzzer);
    int Fogo = 0;
  }
  StaticJsonDocument<300> dados;
    dados["variable"] = "Fogo";
    dados["value"] = Fog;
    serializeJson(dados,buffer);

    client.publish("info/fogo", buffer); 
  
}

// IMPLEMENTO DE FUNÇÕES
void disparaSirene(byte pin, int intervalo) {
  static bool nivel = HIGH;
  static unsigned long ultimaTroca = 0;

  if (millis() - ultimaTroca > intervalo) {
    nivel = !nivel;
    ultimaTroca = millis();
    Serial.println("DISPARO");
  }
  
  digitalWrite(pin, nivel);  
}

void desligaSirene(byte pin) {
  digitalWrite(pin, LOW);
}

