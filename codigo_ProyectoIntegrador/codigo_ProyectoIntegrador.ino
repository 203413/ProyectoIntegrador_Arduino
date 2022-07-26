#include <LiquidCrystal.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include "DHT.h" //DHT11

#define DHTPIN 5
#define DHTTYPE DHT11 
#define trigPin 32 //ultrasonico trig
#define echoPin 13  //ultrasonico echo
const int pinLDR = 35; //luminosidad
const int pinBomba = 25;

const char* ssid = "LAPTOP-J0LGPH7L 7284";
const char* passwd = "e2528R)5";

const int lcdColumns = 16;
const int lcdRows = 2;

byte porcentaje[] = {
    B01100,
    B01101,
    B00010,
    B00100,
    B01000,
    B10110,
    B00110,
    B00000};

int duracion,distancia,luminosidad,humedad_suelo,estado_bomba;
float humedad,temperatura;


DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(115200);
  
  conect_to_wifi(); //funcion para conectarse a wifi
  
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(pinBomba, OUTPUT);
  pinMode(pinLDR, INPUT);
  pinMode(34, INPUT); //entrada de sensor de humedad de suelo

  dht.begin();  

  estado_bomba = 0;
  
}

void loop() {
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());

  digitalWrite(trigPin, HIGH); 
  delayMicroseconds(1000);
  digitalWrite(trigPin, LOW);
  duracion = pulseIn(echoPin, HIGH);
  distancia = (duracion/2)/29.1;//distancia en cm - ultrasonico

  luminosidad = analogRead(pinLDR);
  temperatura = dht.readTemperature(); //temperatura dht11
  humedad = dht.readHumidity();
  float hic = dht.computeHeatIndex(temperatura, humedad, false);  // Calcular el índice de calor en grados centígrados
  humedad_suelo = analogRead(34); //humedad de suelo
  
  int nivel_agua = ((distancia-11)*100)/11;
  //int porcentaje_humedad = ((4095 - humedad_suelo)*100)/2300;
  

  

  //impresin en lcd
  
}


void regar(){
  
}

void conect_to_wifi(){
  WiFi.begin(ssid, passwd);
  Serial.println("Connecting");
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
}

void enviar_datos(){
   if (WiFi.status() == WL_CONNECTED){
    
    HTTPClient http;
    // String datos = "user=" + user + "&pass=" + pass;
    String datos = "temperatura=" + String(temperatura) + "&humedad=" + String(humedad)+ "&humedad_suelo="+String(humedad_suelo) + "&estado="+String(estado_bomba);

    // http.begin("http://192.168.8.1/waterlettuce/esp32.php");        //Indicamos el destino
    http.begin("http://192.168.0.123/api/insertDatos");
    http.addHeader("Content-Type", "application/x-www-form-urlencoded"); // Preparamos el header text/plain si solo vamos a enviar texto plano sin un paradigma llave:valor.

    int status_response = http.POST(datos); // Enviamos el post pasándole, los datos que queremos enviar. (esta función nos devuelve un código que guardamos en un int)

    if (status_response > 0){
      Serial.println("Código HTTP ► " + String(status_response)); // Print return code

      if (status_response == 200){
        String data_response= http.getString();
        Serial.println("El servidor respondió ▼ ");
        Serial.println(data_response);
      }
    }else{

      Serial.print(" Error enviando POST, código: ");
      Serial.println(status_response);
    }
    http.end(); 
  }
}
