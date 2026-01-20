# TinyML-Gesture-Controlled-Robot-
ESP32 and TinyML based wireless robot control system - Bachelor's Thesis Project

# TinyML Based Wireless Gesture Controlled Robot 🤖✋

**Bachelor's Thesis Project - Çukurova University**
**Student:** Cemre İldem İmeç
**Supervisor:** Prof. Dr. Mustafa Gök

## 📖 Project Overview
This project aims to control a mobile robot using hand gestures captured by a smart glove. It utilizes **TinyML** technology running on an **ESP32** microcontroller to classify gestures in real-time without cloud dependency.

## 🛠️ Hardware Components
- **Microcontroller:** ESP32 (x2)
- **Sensors:** MPU6050 (Accelerometer & Gyroscope)
- **Communication:** NRF24L01 Transceiver Modules
- **Motor Driver:** L298N
- **Feedback:** Haptic Vibration Motor

## 🧠 Software & AI
- **Platform:** Edge Impulse (Neural Network Training)
- **Framework:** Arduino IDE (C++)
- **Model Accuracy:** 99.2%
- **Latency:** ~2ms

## 📂 Folder Structure
- `/Transmitter_Glove`: Source code for the wearable glove unit.
- `/Receiver_Robot`: Source code for the mobile robot unit.

## 🚀 How to Run
1. Upload the `Transmitter` code to the glove ESP32.
2. Upload the `Receiver` code to the robot ESP32.
3. Power on both devices.
4. Perform gestures (Forward, Backward, Left, Right, Stop) to control the robot.

---
*Created for Electrical and Electronics Engineering Department Graduation Project, 2026.*
