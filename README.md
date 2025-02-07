# EcoExplorer

## Descripción del Proyecto
EcoExplorer es un sistema de monitoreo ambiental basado en IoT que permite la recolección, procesamiento y visualización de datos en tiempo real. Su objetivo es proporcionar información precisa sobre factores ambientales como la calidad del aire, temperatura, humedad, radiación y presión atmosférica, facilitando la toma de decisiones informadas para mitigar el impacto ambiental.

## Características Principales
- Monitoreo en tiempo real de diversas variables ambientales.
- Uso de sensores de alta precisión conectados a un ESP32.
- Envío de datos a una base de datos en la nube.
- Visualización de datos a través de una página web.
- Implementación de algoritmos para el análisis de datos ambientales.

## Tecnologías Utilizadas
- **Hardware**: ESP32, sensores ambientales (DHT22, MQ-135, BMP280, entre otros).
- **Software**: C++ (Arduino IDE), Python.
- **Base de Datos**: MySQL.
- **Interfaz Web**: HTML, CSS, JavaScript.
- **Cloud & IoT**: Firebase o servicios en la nube para almacenamiento y procesamiento de datos.

## Instalación y Configuración
1. **Clonar el Repositorio**
   ```bash
   git clone https://github.com/Darkness15961/Ecoexplorer.git
   cd EcoExplorer
   ```
2. **Configurar el ESP32**
   - Instalar Arduino IDE y agregar la placa ESP32.
   - Cargar el firmware desde la carpeta `/firmware` al ESP32.
   - Configurar las credenciales Wi-Fi en el código fuente.
3. **Configurar la Base de Datos**
   - Crear una base de datos MySQL.
   - Ejecutar el script SQL proporcionado en `/database/setup.sql`.
4. **Ejecutar el Servidor Web**
   ```bash
   cd web
   npm install
   npm start
   ```

## Uso del Sistema
1. Encender el dispositivo y verificar la conexión Wi-Fi.
2. Acceder a la página web para visualizar los datos.
3. Analizar las gráficas y reportes generados.

## Contribuciones
Las contribuciones son bienvenidas. Para colaborar, por favor sigue estos pasos:
1. Haz un fork del repositorio.
2. Crea una rama con tu nueva funcionalidad (`git checkout -b nueva-funcionalidad`).
3. Realiza los cambios y haz un commit (`git commit -m 'Agrega nueva funcionalidad'`).
4. Envía un pull request.

## Licencia
Este proyecto está bajo la licencia MIT. Consulta el archivo `LICENSE` para más detalles.
