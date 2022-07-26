#include <LiquidCrystal.h>

#include <WiFi.h>
#include <HTTPClient.h>
#include "DHT.h"
#define DHTPIN 4
#define echoPin 33
#define trigPin 32
#define DHTTYPE DHT11
#define bomba2 12

DHT dht(DHTPIN, DHTTYPE);
int sensorPinSuelo = 34;
const int rs = 5, en = 18, d4 = 19, d5 = 21, d6 = 22, d7 = 23;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

const char* ssid = "LAPTOP-J0LGPH7L 7284";
const char* passwd = "e2528R)5";

int distance1,suelo,s;
float h,t;
String bomba;

void setup() {
conect_to_wifi();
Serial.begin(115200);
pinMode(trigPin, OUTPUT);
pinMode(bomba2, OUTPUT);
pinMode(echoPin, INPUT);
Serial.println(F("DHTxx test!"));
lcd.begin(16, 2);
dht.begin();
}

void loop() {
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());
  
  distance1 = getDistance();
   if (distance1 >= 100 || distance1 <= 0){
        Serial.println("Fuera de rango");   delay(2000);
   }
   else{
        Serial.print(distance1 );
        Serial.print(" cm; ");
    }
    suelo = analogRead(sensorPinSuelo);
    delay(3000); 
    h = dht.readHumidity();
    t = dht.readTemperature();

    if (isnan(h) || isnan(t)) {
        Serial.println(F("Failed to read from DHT sensor!"));
        return;
    }

    if (suelo > 3600){
        bomba = "encendido";
    }else{
        bomba = "apagado";
    }
    
    Serial.print(F("Humedad: "));
    Serial.print(h);
    Serial.print(F("% Temperatura: "));
    Serial.print(t);
    Serial.println(F("°C "));
    Serial.print("Suelo: ");
    Serial.println(suelo);

    lcd.setCursor(0, 0);
    lcd.print("H:");
    lcd.print(h);
    lcd.print("%");
    lcd.print(" Temp:");
    lcd.print(t);
    lcd.print("C");
    lcd.setCursor(0, 1);
    lcd.print("S:");
    lcd.print(suelo);
    lcd.print(" D:");
    lcd.print(distance1);
    lcd.print("cm");
    enviar_datos();

    if (bomba=="encendido"){
      digitalWrite(bomba2,HIGH);
      delay(3000);
      digitalWrite(bomba2,LOW);
    }

    delay(10000);
}

int getDistance(){
  int duration, distance;
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(1000);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distance = (duration/2)/29.1; // CM
  return distance;
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
    String datos = "humedad=" + String(h) + "&temp=" + String(t)+ "&h_suelo="+String(suelo) + "&n_agua="+String(distance1)+"&e_bomba="+bomba;
  
    http.begin("http://192.168.0.4:3001/api/datos");
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
