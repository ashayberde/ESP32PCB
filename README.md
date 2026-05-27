ESP32-Based IoT Control Board with RFID, Relay, Servo, LCD, and WiFiManager
On Board Communication interfaces: I2C,SPI,UART

## Overview

This project is an ESP32-based IoT control board designed for RFID-based access control, automation, and remote monitoring applications. Although the current firmware demonstrates a parking/access control use case, the hardware is not limited to access control. Since the board includes WiFi connectivity, relay output, servo control, buzzer indication, RFID input, LCD feedback, and API communication, it can be adapted for several other embedded and IoT applications.

The board can also be extended with MQTT support to enable real-time communication with cloud platforms, mobile apps, or home automation systems for remote monitoring and device control.

The board reads RFID card/tag IDs using an MFRC522 RFID reader and can verify the scanned ID through a web API. Based on the response, it can activate external devices using the relay, move a servo motor, provide buzzer indication, and display system status on a 16x2 I2C LCD.

WiFi connectivity is handled using WiFiManager, so the ESP32 does not require hardcoded WiFi credentials. This makes the board easier to configure and deploy in different locations.
