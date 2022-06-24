#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <ArduinoJson.h>

//Sonido Alarma
int frecmin = 1000;
int frecmax = 4000;

// Ingresamos el SSID y Password de nuestra red WI-FI
const char* ssid = "REDMI 9C";
const char* password = "21seam24";

// iniciamos el Bot de Telegram ingresando el Token que recibimos de Botfather
#define BOTtoken "5474915855:AAEFLzEkM8qGlagUwbwlQxKV5UQ4pQMCt4c"  

// Ingresamos el ID que recibimos del Chat con IDBot
#define CHAT_ID "5276235949"

X509List cert(TELEGRAM_CERTIFICATE_ROOT);
WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);

const int led = D6; //pin del led
int pinAlarma = D1; // pin del parlante
int i; 
const int movimSensor = D5; // Pin para el sensor de movimiento
bool motionDetected = false; // desde el principio la alarma debe estar en falso para evitar que suene al Iniciar el circuito

// si se detecta un movimiento es llamada esta parte del codigo. 
ICACHE_RAM_ATTR void detectaMovimiento() {
  //Serial.println("MOTION DETECTED!!!");
  motionDetected = true;
  digitalWrite(led, 1);

  //Se activa la alarma
  for (i = frecmin; i <= frecmax; i++){
    tone(pinAlarma, i); 
  }
  for (i = frecmax; i >= frecmin; i--){
    tone(pinAlarma, i);
  }
}

void setup() {
  Serial.begin(115200);
  configTime(0, 0, "pool.ntp.org");      // obtenemos la Hora UTC via NTP
  client.setTrustAnchors(&cert); // Agregamos el certificado Raiz de api.telegram.org

  // colocamos el sensor PIR en modo INPUT_PULLUP
  pinMode(movimSensor, INPUT_PULLUP);
  // establecemos el pin movimSensor como interrupcion, asignamos la interrupcion en modo RISING 
  attachInterrupt(digitalPinToInterrupt(movimSensor), detectaMovimiento, RISING);

  // conexion con la red WI-FI:
  Serial.print("Connecting Wifi: ");
   Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  bot.sendMessage(CHAT_ID, "Bot started up", "");
}

void loop() {
  if(motionDetected){
    bot.sendMessage(CHAT_ID, "Se Detecto un Movimiento, Alarma Activa!!", "");
    Serial.println("Movimiento Detectado");
    motionDetected = false;
    digitalWrite(led, 0);

    //Alarma apagada
    noTone(pinAlarma);
  }
}
