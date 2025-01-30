const int sensorPin = 32;   // Pin digital conectado al sensor SW-18015P

void setup() {
  pinMode(sensorPin, INPUT);   // Configurar el pin del sensor como entrada
  Serial.begin(9600);          // Iniciar comunicación serial
}

void loop() {
  int estadoSensor = digitalRead(sensorPin);  // Leer el estado del sensor
  
  if (estadoSensor == HIGH) {  // Si el sensor detecta vibración
    Serial.println("¡0!");
    delay(1000);                 // Retardo para evitar lecturas repetidas
  } else {
    Serial.println("¡Vibración detectada");
  // Apagar el LED
  }
  
  delay(100);  // Pequeño retardo para estabilizar las lecturas
}
