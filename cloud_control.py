import firebase_admin
from firebase_admin import credentials, db
import joblib
import time
import pandas as pd

# ===== FIREBASE INIT =====
cred = credentials.Certificate("smartphcontrol-firebase-adminsdk-fbsvc-812580f4ee.json")
firebase_admin.initialize_app(cred, {
    'databaseURL': 'https://smartphcontrol-default-rtdb.firebaseio.com/'
})

# ===== LOAD MODEL =====
model = joblib.load("model.pkl")

print("Smart Cloud Control Started...")

# ===== CONTROL PARAMETERS =====
last_dose_time = 0
COOLDOWN = 15  # seconds

PH_LOW = 6.5
PH_HIGH = 7.5

while True:
    try:
        sensor = db.reference("sensor").get()

        ph = sensor.get("ph", 7)
        temp = sensor.get("temp", 30)
        hum = sensor.get("humidity", 80)
        voltage = sensor.get("voltage", 2.5)

        # ===== FIX WARNING (PANDAS) =====
        data = pd.DataFrame([[voltage, temp, hum]],
                            columns=["Voltage", "Temperature", "Humidity"])

        prediction = model.predict(data)[0]

        dosing = "OFF"
        circulation = "OFF"

        current_time = time.time()

        # ===== SENSOR SAFETY =====
        if ph < 2 or ph > 12:
            print("⚠️ Sensor abnormal → Skipping dosing")
            dosing = "OFF"
            circulation = "OFF"

        # ===== NORMAL CONTROL =====
        else:

            # 🎯 CASE 1: SAFE RANGE → EVERYTHING OFF
            if PH_LOW <= ph <= PH_HIGH:
                dosing = "OFF"
                circulation = "OFF"
                print("✅ pH Stable → System Idle")

            # 🎯 CASE 2: OUT OF RANGE
            else:

                # Only circulate when correcting
                circulation = "ON"

                # Cooldown check
                if current_time - last_dose_time > COOLDOWN:

                    if ph < PH_LOW:
                        dosing = "ON"   # add base

                    elif ph > PH_HIGH:
                        dosing = "ON"   # add acid

                    if dosing == "ON":
                        last_dose_time = current_time

        # ===== WRITE TO FIREBASE =====
        db.reference("control").update({
            "prediction": prediction,
            "dosing": dosing,
            "circulation": circulation
        })

        print(f"pH:{ph:.2f} → {prediction} | Dosing:{dosing} | Circulation:{circulation}")

    except Exception as e:
        print("Error:", e)

    time.sleep(5)