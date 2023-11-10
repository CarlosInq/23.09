/*
  Ethernet/WiFi Web Server
*/
#if !defined(__AVR__)
#define USE_WIFI
#endif

#ifdef USE_WIFI
#include <WiFi.h>

const char* ssid     = "PICSimLabWifi";
const char* password = "";

WiFiServer server(1001);
#else
#include <Ethernet.h>

EthernetServer server(1001);

const byte mac[] = {
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED
};
#endif

void setup()
{
  Serial.begin(115200);
  delay(10);

  // prepare GPIO2 - LED
  pinMode(2, OUTPUT);
  digitalWrite(2, 0);


#ifdef USE_WIFI
  // Connect to WiFi network
  Serial.println();
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
#else
  // start the Ethernet connection and the server:
  if (!Ethernet.begin(mac)) {
    Serial.println("Failed to configure Ethernet using DHCP");
  }

  // Check for Ethernet hardware present
  if (Ethernet.hardwareStatus() == EthernetNoHardware) {
    Serial.println("Ethernet shield was not found.  Sorry, can't run without hardware. :(");
    while (true) {
      delay(1); // do nothing, no point running without Ethernet hardware
    }
  }
  if (Ethernet.linkStatus() == LinkOFF) {
    Serial.println("Ethernet cable is not connected.");
  }

#endif

  // Start the server
  server.begin();
  Serial.println("Server started");

  // Print the IP address

#ifdef USE_WIFI
  Serial.println(WiFi.localIP());
#else
  Serial.println(Ethernet.localIP());
#endif
}

void loop() {
  // Check if a client has connected
#ifdef USE_WIFI
  WiFiClient client = server.available();
#else
  EthernetClient client = server.available();
#endif
  if (!client) {
    return;
  }

  // Wait until the client sends some data
  Serial.println("new client");
  while (!client.available()) {
    delay(1);
  }

  // Read the first line of the request
  String req = client.readStringUntil('\n');
  Serial.println(req);
  client.flush();

  String resp = "";
  int pin, value;
  switch (req[0])
  {
    case 'd': //escreve valor digital
      pin = req.substring(1, req.indexOf("=")).toInt();
      value = req.substring(req.indexOf("=") + 1).toInt();
      Serial.print("Saida digital: ");
      Serial.print(pin);
      Serial.print(" = ");
      Serial.println(value);
      digitalWrite(pin, value);
      resp = "Ok\n";
      break;
    case 'v': //lê valor digital
      pin = req.substring(1).toInt();
      Serial.print("Le entrada digital: ");
      Serial.println(pin);
      resp = digitalRead(pin);
      resp += "\nOk\n";
      break;
    case 'a': //lê valor analógico
      pin = req.substring(1).toInt();
      Serial.print("Le entrada analogica: ");
      Serial.println(pin);
      resp = analogRead(pin);
      resp += "\nOk\n";
      break;
    case 'p': //escreve valor analógico
      pin = req.substring(1, req.indexOf("=")).toInt();
      value = req.substring(req.indexOf("=") + 1).toInt();
      Serial.print("Escreve saida analogica: ");
      Serial.print(pin);
      Serial.print(" = ");
      Serial.println(value);
      analogWrite(pin, value);
      resp = "Ok\n";
      break;
    case 'n': //lê nomes
      Serial.println("Lista Sensores e Atuadores");
      resp = "Pino -  Dir. - Nome\n"
             "   2  - Saida - LED interno\n"
             "Ok\n";
      break;
    case 'h': //mostra ajuda
      Serial.println("Mostra ajuda");
      resp = "Comandos:\n"
             " d - escreve pino digital: d[pin]=valor(0-1) ex: d13=1\n"
             " v - le pino digital: v[pin] ex: v13\n"
             " a - le pino analogico: a[pin] ex: a19\n"
             " p - escreve pino analogico: p[pin]=valor(0-255) ex: p5=120\n"
             " n - lista sensores e atuadores\n"
             "Ok\n";
      break;
    default:
      Serial.println("Requisicao invalida");
      resp = "ERRO\n";
      break;
  }

  Serial.println(resp);//debug

  client.println(resp);

  client.flush();
  delay(1);
  Serial.println("Client disonnected");
  client.stop();


  // The client will actually be disconnected
  // when the function returns and 'client' object is detroyed
}
