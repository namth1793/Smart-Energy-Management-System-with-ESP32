# Smart Energy Management System with ESP32

This repository contains the source code and project files for a **Smart Energy Management System** based on the **ESP32 microcontroller**. This project was developed as part of a university course on IoT Architecture and demonstrates real-time monitoring and control of energy consumption using IoT techniques. :contentReference[oaicite:0]{index=0}

---

## Project Summary

The aim of this system is to provide a **smart way to monitor and manage energy usage** of electrical devices using an ESP32 board. The system:
- Reads electrical parameters (e.g., power consumption) using connected sensors,
- Sends data to a cloud backend for storage and visualization,
- Enables device control via mobile applications or network interfaces. :contentReference[oaicite:1]{index=1}

Fundamental components include:
- **ESP32 development board**
- **Energy/current sensors**
- **Wi-Fi connectivity**
- **Firebase backend**
- **MQTT communication** :contentReference[oaicite:2]{index=2}

---

## Key Features

The system currently supports:

- **Real-time energy consumption monitoring**
- **Control of connected electrical devices**
- **Voice control from smartphone app**
- **Data storage on Firebase**
- **MQTT based communication between device and server** :contentReference[oaicite:3]{index=3}

---

## Hardware Requirements

To build and deploy this system, you will need:

| Component | Purpose |
|-----------|---------|
| ESP32 | Main controller and IoT node |
| Power / Current Sensor | Measures device energy consumption |
| Relay Module | Controls device power on/off |
| USB Power Supply | Provides power to ESP32 |
| Smartphone or Web App | For monitoring and control |

> Ensure sensor modules are compatible with the ESP32’s voltage levels and that all wiring is secure before powering the system.

---

## Software Requirements

Before building and uploading the code, install the following tools and libraries:

### Development Tools
- **Arduino IDE** or **PlatformIO**
- Install the **ESP32 board package**
- Enable **Wi-Fi and MQTT libraries**

### Backend & Services
- **Firebase** (Realtime Database or Firestore) for data storage
- **MQTT broker** (e.g., Mosquitto, Cloud MQTT, Firebase integration)

### Optional
- Mobile app for data visualization
- Voice or Smart Home integration

---

## Below is my PCB layout of end device which include ESP32 and sensor to monitor power consumption of target devices.
![Image](https://github.com/user-attachments/assets/c9c5d15d-dd0c-44dc-94df-3ea04119b49d)

---
## Installation and Setup

1. **Clone the Repository**
   ```bash
   git clone https://github.com/namth1793/Smart-Energy-Management-System-with-ESP32.git

