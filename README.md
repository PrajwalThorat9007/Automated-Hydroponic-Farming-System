# Automated-Hydroponic-Farming-System

## Overview

This project is an IoT-based smart hydroponic monitoring and automated pH control system built using ESP32, Firebase Realtime Database, Machine Learning, and cloud-based automation.

The system continuously monitors:

* pH level
* Temperature
* Humidity
* Sensor voltage

Sensor data is sent from the ESP32 to Firebase in real time. A cloud-based Python controller reads the data, applies machine learning prediction and safety logic, then sends control commands back to the ESP32. The ESP32 controls dosing and circulation pumps using relay modules.

---

# System Architecture

```text
Sensors → ESP32 → Firebase Realtime Database → Python Cloud Controller
                                                      ↓
                                             ML Prediction + Safety Logic
                                                      ↓
ESP32 ← Firebase Realtime Database ← Pump Control Commands
                                                      ↓
                                          Relay Module → Pumps
```

---

# Hardware Components

## Main Components

* ESP32 Development Board
* pH Sensor Module
* DHT22 Temperature and Humidity Sensor
* 4-Channel Relay Module
* 12V DC Submersible Pump
* 12V DC Peristaltic Pump
* Buck Converter
* Breadboards and Jumper Wires
* 12V Battery Supply
* 1N4007 Diode for pump protection

---

# Arduino Firmware Summary

The ESP32 firmware performs the following operations:

1. Reads pH sensor analog voltage
2. Converts voltage into estimated pH value
3. Reads temperature and humidity from DHT22
4. Connects to WiFi
5. Sends sensor data to Firebase Realtime Database
6. Reads cloud control commands from Firebase
7. Controls relay outputs for dosing and circulation pumps
8. Implements safety timing for pump activation

## Main Functionalities

### Sensor Monitoring

* Real-time pH monitoring
* Temperature monitoring
* Humidity monitoring
* Voltage tracking

### Cloud Communication

* Firebase Realtime Database integration
* Real-time sensor upload
* Real-time control command retrieval

### Pump Automation

* Dosing pump control
* Circulation pump control
* Timed dosing
* Cooldown protection

---

# Firebase Setup

## Step 1: Create Firebase Project

1. Open Firebase Console
2. Create a new project
3. Enable Realtime Database
4. Select test mode during initial setup

---

## Step 2: Enable Realtime Database

1. Navigate to:

```text
Build → Realtime Database
```

2. Create database
3. Choose nearest region
4. Start in test mode

---

## Step 3: Database Structure

Create the following structure:

```json
{
  "sensor": {
    "ph": 7.0,
    "temp": 30,
    "humidity": 80,
    "voltage": 2.5
  },
  "control": {
    "prediction": "Neutral",
    "dosing": "OFF",
    "circulation": "OFF"
  }
}
```

---

## Step 4: Obtain Firebase Credentials

### Realtime Database URL

Copy the Firebase Realtime Database URL from:

```text
Firebase Console → Realtime Database
```

### Firebase Service Account Key

1. Open:

```text
Project Settings → Service Accounts
```

2. Generate new private key
3. Download JSON credential file
4. Store it in the project directory

---

# ESP32 and Firebase Integration

## Required Arduino Libraries

Install the following libraries in Arduino IDE:

* Firebase ESP Client
* DHT Sensor Library
* Adafruit Unified Sensor
* WiFi Library (ESP32 core)

---

## ESP32 Workflow

The ESP32 firmware:

1. Connects to WiFi
2. Synchronizes time using NTP
3. Initializes Firebase
4. Reads sensor data
5. Uploads sensor values to Firebase
6. Reads control commands from Firebase
7. Controls relay outputs accordingly

---

# Machine Learning Workflow

## Dataset Collection

Sensor data was collected in CSV format containing:

* Voltage
* pH
* Temperature
* Humidity

The dataset was labeled into:

* Acidic
* Neutral
* Basic

---

## Model Training

A Decision Tree Classifier was trained using:

* Voltage
* Temperature
* Humidity

The trained model was saved using:

```python
joblib.dump(model, "model.pkl")
```

---

# cloud_control.py Summary

The `cloud_control.py` script acts as the cloud-based intelligent controller.

## Responsibilities

* Reads sensor data from Firebase
* Loads trained machine learning model
* Predicts solution condition
* Applies safety and threshold logic
* Sends pump control commands back to Firebase

---

## Safety Features Implemented

### Cooldown Protection

Prevents continuous dosing by enforcing delay intervals between dosing cycles.

### Timed Dosing

Pumps are activated only for a fixed duration.

### Sensor Validation

Extreme abnormal pH values are ignored to avoid incorrect dosing.

### Target pH Range

Automation stops when pH enters safe range.

---

# Cloud Control Logic

## Neutral Range

```text
pH between 6.5 and 7.5
```

Actions:

* Dosing OFF
* Circulation OFF

---

## Acidic Range

```text
pH below 6.5
```

Actions:

* Dosing ON
* Circulation ON

---

## Basic Range

```text
pH above 7.5
```

Actions:

* Dosing ON
* Circulation ON

---

# Final Workflow

## Step-by-Step System Operation

1. Sensors read environmental values
2. ESP32 uploads data to Firebase
3. Cloud controller reads Firebase sensor data
4. ML model predicts water condition
5. Safety logic validates prediction
6. Control commands are written to Firebase
7. ESP32 reads control commands
8. Relay module activates pumps
9. System continuously monitors and adjusts pH

---

# Running the Project

## Arduino Firmware

1. Open Arduino IDE
2. Install required libraries
3. Select ESP32 board
4. Upload firmware to ESP32

---

## Python Cloud Controller

Install dependencies:

```bash
pip install firebase-admin pandas scikit-learn joblib
```

Run cloud controller:

```bash
python cloud_control.py
```

---

# Project Features

* Real-time sensor monitoring
* Cloud-based automation
* Firebase integration
* Machine learning prediction
* Automated pH control
* Pump safety logic
* Remote monitoring capability
* Scalable IoT architecture

---

# Future Improvements

* Mobile dashboard
* Historical graph visualization
* SMS/Email alerts
* Separate acid and base pumps
* Automatic calibration system
* Cloud deployment on AWS
* Web dashboard for analytics

---

# Technologies Used

## Embedded Systems

* ESP32
* Arduino IDE
* Embedded C++

## Cloud and IoT

* Firebase Realtime Database
* WiFi Communication

## Machine Learning

* Python
* Scikit-learn
* Pandas
* Joblib

## Sensors and Automation

* pH Sensor
* DHT22
* Relay Module
* DC Pumps

---

# Conclusion

This project demonstrates an intelligent hydroponic automation system integrating embedded systems, IoT cloud communication, machine learning, and automated control.

The architecture enables real-time monitoring, cloud-based decision making, and automated pH regulation with safety mechanisms suitable for smart agriculture and hydroponic applications.

