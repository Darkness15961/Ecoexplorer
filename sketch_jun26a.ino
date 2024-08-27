#include <Adafruit_BMP085.h>
#include <Wire.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include "DHT.h"

const char* ssid = "Rutihsoc";
const char* password = "Red,Gold&Green";

int uvsensorpin = 34;
int gassensorpin = 35;
int DHTPIN = 25;

#define DHTTYPE DHT22

DHT dht(DHTPIN, DHTTYPE);
Adafruit_BMP085 bmp;

const int pinRedUV = 18;
const int pinGreenUV = 19;
const int pinRedGas = 17;
const int pinGreenGas = 16;

int redUV = 255;
int greenUV = 255;
int redGas = 255;
int greenGas = 255;

unsigned long previousCaptureMillis = 0;
unsigned long previousSendMillis = 0;
const long captureInterval = 5000; // 5 seconds
const long sendInterval = 300000; // 5 minutes

float sumUV = 0;
float sumCO2 = 0;
float sumTemp = 0;
float sumHum = 0;
float sumPres = 0;
int captureCount = 0;

void setColor(int RUV, int GUV, int RGas, int GGas) {
    analogWrite(pinRedUV, RUV);
    analogWrite(pinGreenUV, GUV);
    analogWrite(pinRedGas, RGas);
    analogWrite(pinGreenGas, GGas);
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
    if (!bmp.begin()) {
        Serial.println("Error... Revisar conexión");
        while (1) {}
    }
    pinMode(pinRedUV, OUTPUT);
    pinMode(pinGreenUV, OUTPUT);
    pinMode(pinRedGas, OUTPUT);
    pinMode(pinGreenGas, OUTPUT);
}

void loop() {
    unsigned long currentMillis = millis();

    if (currentMillis - previousCaptureMillis >= captureInterval) {
        previousCaptureMillis = currentMillis;
        captureData();
    }

    if (currentMillis - previousSendMillis >= sendInterval) {
        previousSendMillis = currentMillis;
        sendData();
        resetSums();
    }
}

void captureData() {
    float uvValue = analogRead(uvsensorpin);
    float OutVoltage = uvValue * (3.3 / 4095);
    float divisores[] = {0.35, 0.45, 0.6, 0.75, 0.8, 0.9};
    float uvIntensities[6];
    float sum = 0.0;
    for (int i = 0; i < 6; i++) {
        uvIntensities[i] = (OutVoltage - 0.89) / divisores[i];
        sum += uvIntensities[i];
    }
    float averageUV = sum / 6;
    float IndexUV = (averageUV * 40) / 2.5;

    float gasValue = analogRead(gassensorpin);
    float VoltageGas = gasValue * 5 / 4095;
    float Rs = 10 * ((5 - VoltageGas) / VoltageGas);
    float CO2 = 124.75 * pow(Rs / 3.94, -2.805);

    float hum = dht.readHumidity();
    float temp = dht.readTemperature();
    float pres = bmp.readPressure();

    sumUV += IndexUV;
    sumCO2 += CO2;
    sumTemp += temp;
    sumHum += hum;
    sumPres += pres;
    captureCount++;

    Serial.print("SENSOR UV: ");
    Serial.print(uvValue);
    Serial.print(" - ");
    Serial.print(OutVoltage);
    Serial.print(" - ");
    Serial.print(averageUV);
    Serial.print(" - ");
    Serial.println(IndexUV);
    Serial.print("SENSOR temperatura: ");
    Serial.println(temp);
    Serial.print("SENSOR gas: ");
    Serial.print(gasValue);
    Serial.print(" - ");
    Serial.print(VoltageGas);
    Serial.print(" - ");
    Serial.print(Rs);
    Serial.print(" - ");
    Serial.println(CO2);
    Serial.print("SENSOR Humedad: ");
    Serial.println(hum);
    Serial.print("SENSOR Presión: ");
    Serial.println(pres);
    
    if (IndexUV < 3) {
        redUV = 255;
        greenUV = 0;
    } else if (IndexUV >= 3 && IndexUV < 6) {
        redUV = 0;
        greenUV = 0;
    } else {
        redUV = 0;
        greenUV = 255;
    }

    if (CO2 < 800) {
        redGas = 255;
        greenGas = 0;
    } else if (CO2 >= 800 && CO2 < 1000) {
        redGas = 0;
        greenGas = 0;
    } else {
        redGas = 0;
        greenGas = 255;
    }

    setColor(redUV, greenUV, redGas, greenGas);
}

void sendData() {
    if (captureCount == 0) return; // avoid division by zero
    float avgUV = sumUV / captureCount;
    float avgCO2 = sumCO2 / captureCount;
    float avgTemp = sumTemp / captureCount;
    float avgHum = sumHum / captureCount;
    float avgPres = sumPres / captureCount;

    String uvString = String(avgUV);
    String tempString = String(avgTemp);
    String gasString = String(avgCO2);
    String humString = String(avgHum);
    String presString = String(avgPres);

    Serial.print("Promedio UV: ");
    Serial.println(uvString);
    Serial.print("Promedio Temperatura: ");
    Serial.println(tempString);
    Serial.print("Promedio Gas: ");
    Serial.println(gasString);
    Serial.print("Promedio Humedad: ");
    Serial.println(humString);
    Serial.print("Promedio Presión: ");
    Serial.println(presString);

    if (WiFi.status() == WL_CONNECTED) {
        HTTPClient http;

        char datosUV[200];
        char datosTemp[200];
        char datosGas[200];
        char datosHum[200];
        char datosPres[200];
        String macString = String(ESP.getEfuseMac());
        
        sprintf(datosUV, "https://ecox.ccore.store/registros?descripcion=IndiceUV&valor=%s&unidad=x/&mac=%s", uvString.c_str(), macString.c_str());
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

      sprintf(datosTemp, "https://ecox.ccore.store/registros?descripcion=Temperatura&valor=%s&unidad=Celsius&mac=%s", tempString.c_str(), macString.c_str());
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

        sprintf(datosGas, "https://ecox.ccore.store/registros?descripcion=Gas&valor=%s&unidad=PPM&mac=%s", gasString.c_str(), macString.c_str());
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

          sprintf(datosHum, "https://ecox.ccore.store/registros?descripcion=Humedad&valor=%s&unidad=RH&mac=%s", humString.c_str(), macString.c_str());
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
   sprintf(datosPres, "https://ecox.ccore.store/registros?descripcion=Presion&valor=%s&unidad=Pa/&mac=%s", presString.c_str(), macString.c_str());
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
    }else{
      Serial.println("Error conexion server");
    }
}}}}}}

void sendHTTPPost(HTTPClient &http, const char* descripcion, const String &valor, const String &mac) {
    char url[200];
    sprintf(url, "https://ecox.ccore.store/registros?descripcion=%s&valor=%s&unidad=&mac=%s", descripcion, valor.c_str(), mac.c_str());
    http.begin(url);
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    int codigo_respuesta = http.GET();
    Serial.println(codigo_respuesta);
    if (codigo_respuesta > 0) {
        Serial.println("Código HTTP > " + String(codigo_respuesta));
        if (codigo_respuesta == 200) {
            String cuerpo_respuesta = http.getString();
            Serial.println("El servidor respondió");
            Serial.println(cuerpo_respuesta);
        } else {
            Serial.print("Error enviando POST, código: ");
            Serial.println(codigo_respuesta);
        }
    } else {
        Serial.println("Error conexion server ");
    }
    http.end();
}

void resetSums() {
    sumUV = 0;
    sumCO2 = 0;
    sumTemp = 0;
    sumHum = 0;
    sumPres = 0;
    captureCount = 0;
}
