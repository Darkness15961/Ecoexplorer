#include <Adafruit_BMP085.h>
#include <Wire.h>
#include <WiFi.h>        //Librería de WiFI
#include <HTTPClient.h>  //Librería del Server
#include "DHT.h"         //Librería de Humedad y Temperatura

const char* ssid = "UCUJPA30601 1142";
const char* password = "123@4567";

int uvsensorpin = 34;   //Pin de SALIDA sensor GYML8511 en el D13 del ESP
int gassensorpin = 35;  //Pin de SALIDA sensor MQ135 en el D14 del ESP
int DHTPIN = 25;        //Pin de SALIDA sensor DHT22 en el D33 del ESP

#define DHTTYPE DHT22

DHT dht(DHTPIN, DHTTYPE);
Adafruit_BMP085 bmp;

const int pinRedUV=18;
const int pinGreenUV=19;
const int pinRedGas=17;
const int pinGreenGas=16;

int redUV=255;
int greenUV=255;
int redGas=255;
int greenGas=255;

void setColor(int RUV,int GUV,int RGas,int GGas){
  analogWrite(pinRedUV,RUV);
  analogWrite(pinGreenUV,GUV);
  analogWrite(pinRedGas,RGas);
  analogWrite(pinGreenGas,GGas);
}

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  Serial.print("Conectado ");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.print("Conectado con éxito, mi IP es: ");
  Serial.println(WiFi.localIP());
  dht.begin();

  if(!bmp.begin()){
    Serial.println("Error... Revisar conexión");
    while(1){}
  }

  pinMode(pinRedUV,OUTPUT);
  pinMode(pinGreenUV,OUTPUT);
  pinMode(pinRedGas,OUTPUT);
  pinMode(pinGreenGas,OUTPUT);
}

void loop() {
  float uvvalue = analogRead(uvsensorpin);    //Lectura analógica de UV
  float gasvalue = analogRead(gassensorpin);  //Lectura analógica de Gas
  float hum = dht.readHumidity();
  float temp = dht.readTemperature();
  float hic = dht.computeHeatIndex(temp, hum, false);

  double mac = ESP.getEfuseMac();
  Serial.println(ESP.getEfuseMac());

  String uvString = String(uvvalue);
  String macString = String(mac);
  String tempString = String(temp);
  String gasString = String(gasvalue);
  String humString = String(hum);
  String presString = String(bmp.readPressure());
  /*IMPRIMIR VALORES DE LOS SENSORES DE MANERA LOCAL*/
  Serial.print("SENSOR UV: ");
  Serial.println(uvvalue);
  Serial.print("SENSOR temperatura: ");
  Serial.println(temp);
  Serial.print("SENSOR gas: ");
  Serial.println(gasvalue);
  Serial.print("SENSOR Humedad: ");
  Serial.println(hum);
  Serial.print("SENSOR Presión: ");
  Serial.println(bmp.readPressure());
  setColor(255, 255, 255, 255);
  
  // Check CO2 levels and set colors
  if (uvvalue < 3) {
        redUV = 255;
        greenUV = 0;
    } else if (uvvalue >= 3 && uvvalue < 6) {
        redUV = 0;
        greenUV = 0;
    } else {
        redUV = 0;
        greenUV = 255;
    }

    if (gasvalue < 800) {
        redGas = 255;
        greenGas = 0;
    } else if (gasvalue >= 800 && gasvalue < 1000) {
        redGas = 0;
        greenGas = 0;
    } else {
        redGas = 0;
        greenGas = 255;
    }

    setColor(redUV, greenUV, redGas, greenGas);

  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    char datosUV[200];
    char datosTemp[200];
    char datosGas[200];
    char datosHum[200];
    char datosPres[200];

    sprintf(datosUV, "https://ecoexplorer.000webhostapp.com//registros?descripcion=IndiceUV&valor=%s&unidad=x/&mac=%s", uvString.c_str(), macString.c_str());
    http.begin(datosUV);
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    int codigo_respuestaUV = http.GET();
    Serial.println(codigo_respuestaUV);
    if (codigo_respuestaUV > 0) {
      Serial.println("Código HTTP > " + String(codigo_respuestaUV));
      Serial.println(http.getString());
      if (codigo_respuestaUV == 200) {
        String cuerpo_respuestaUV = http.getString();
        Serial.println("El servidor respondió");
        Serial.println(cuerpo_respuestaUV);

      } else {

        Serial.print("Error enviando POST, código: ");
        Serial.println(codigo_respuestaUV);
      }

      http.end();

      sprintf(datosTemp, "https://ecoexplorer.000webhostapp.com//registros?descripcion=Temperatura&valor=%s&unidad=Celsius&mac=%s", tempString.c_str(), macString.c_str());
      http.begin(datosTemp);
      http.addHeader("Content-Type", "application/x-www-form-urlencoded");
      int codigo_respuestaTemp = http.GET();
      Serial.println(codigo_respuestaTemp);
      if (codigo_respuestaTemp > 0) {
        Serial.println("Código HTTP > " + String(codigo_respuestaTemp));
        Serial.println(http.getString());
        if (codigo_respuestaTemp == 200) {
          String cuerpo_respuestaTemp = http.getString();
          Serial.println("El servidor respondió");
          Serial.println(cuerpo_respuestaTemp);

        } else {

          Serial.print("Error enviando POST, código: ");
          Serial.println(codigo_respuestaTemp);
        }

        http.end();

        sprintf(datosGas, "https://ecoexplorer.000webhostapp.com//registros?descripcion=Gas&valor=%s&unidad=PPM&mac=%s", gasString.c_str(), macString.c_str());
        http.begin(datosGas);
        http.addHeader("Content-Type", "application/x-www-form-urlencoded");

        int codigo_respuestaGas = http.GET();
        Serial.println(codigo_respuestaGas);
        if (codigo_respuestaGas > 0) {
          Serial.println("Código HTTP > " + String(codigo_respuestaGas));
          Serial.println(http.getString());
          if (codigo_respuestaGas == 200) {
            String cuerpo_respuestaGas = http.getString();
            Serial.println("El servidor respondió");
            Serial.println(cuerpo_respuestaGas);

          } else {

            Serial.print("Error enviando POST, código: ");
            Serial.println(codigo_respuestaGas);
          }

          http.end();

          sprintf(datosHum, "https://ecoexplorer.000webhostapp.com//registros?descripcion=Humedad&valor=%s&unidad=RH&mac=%s", humString.c_str(), macString.c_str());
          http.begin(datosHum);
          http.addHeader("Content-Type", "application/x-www-form-urlencoded");

          int codigo_respuestaHum = http.GET();
          Serial.println(codigo_respuestaHum);
          if (codigo_respuestaHum > 0) {
            Serial.println("Código HTTP > " + String(codigo_respuestaHum));
            Serial.println(http.getString());
            if (codigo_respuestaHum == 200) {
              String cuerpo_respuestaHum = http.getString();
              Serial.println("El servidor respondió");
              Serial.println(cuerpo_respuestaHum);

            } else {

              Serial.print("Error enviando POST, código: ");
              Serial.println(codigo_respuestaHum);
            }

            http.end();
   sprintf(datosPres, "https://ecoexplorer.000webhostapp.com//registros?descripcion=Presion&valor=%s&unidad=Pa/&mac=%s", presString.c_str(), macString.c_str());
    http.begin(datosPres);
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    int codigo_respuestaPres = http.GET();
    Serial.println(codigo_respuestaPres);
    if (codigo_respuestaPres > 0) {
      Serial.println("Código HTTP > " + String(codigo_respuestaPres));
      Serial.println(http.getString());
      if (codigo_respuestaPres == 200) {
        String cuerpo_respuestaPres = http.getString();
        Serial.println("El servidor respondió");
        Serial.println(cuerpo_respuestaPres);

      } else {

        Serial.print("Error enviando POST, código: ");
        Serial.println(codigo_respuestaPres);
      }

      http.end();
          } else {

            Serial.println("Error conexion server ");
          }

          delay(1000);
        }
      }
    }
  }
}
}