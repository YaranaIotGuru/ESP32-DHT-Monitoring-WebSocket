# Real-Time DHT Sensor Monitoring with WebSocket Integration

![Thumbnail](http://img.youtube.com/vi/gOhSKx6GzaU/sddefault.jpg)

This project demonstrates real-time monitoring of DHT sensor data using an ESP32 microcontroller, WebSocket communication, and Web technologies.

## About
This repository contains the Arduino code for ESP32 microcontroller to read temperature and humidity data from the DHT sensor, transmit it over WebSocket, and a web interface to visualize the live sensor readings.

## Requirements
- ESP32 Development Board
- DHT Sensor (DHT11)
- Buzzer
- WiFi Network

## Installation and Setup
1. Connect the DHT sensor to the ESP32 development board.
2. Upload the provided Arduino sketch `esp32_dht_ws.ino` to your ESP32 board.
3. Ensure the required libraries are installed: `WiFi.h`, `WebSocketsServer.h`, `ESPAsyncWebServer.h`, and `DHT.h`.
4. Update the WiFi credentials in the code (`ssid` and `password` variables) to match your network.
5. Upload the code to your ESP32 board and open the Serial Monitor to view the IP address.
6. Access the web interface by entering the ESP32's IP address in a web browser.

## Usage
- Access the web interface using any web browser.
- The web interface displays real-time temperature and humidity readings.
- If the temperature exceeds 30°C, a buzzer connected to pin 5 will be triggered.
- Additionally, the WebSocket connection allows for real-time updates without refreshing the page.

## Video Tutorial on Yarana IoT Guru YouTube Channel
For a comprehensive step-by-step tutorial on setting up this project, check out the video on [Yarana IoT Guru YouTube channel](https://youtu.be/gOhSKx6GzaU).

## Contributors
- Your Name (@YaranaIotGuru)

## License
This project is licensed under the [MIT License](LICENSE).
