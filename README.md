# Smart School ESP32

This project uses ESP32 to monitor and control devices in a smart classroom, connecting to Firebase for sensor data storage and synchronization.

## Objectives
- Measure and monitor temperature, humidity, air quality, and sound level in the classroom.
- Control devices such as lights, ceiling fans, TV, and exhaust fans based on sensor data or remote commands via Firebase.

## Hardware Used
- ESP32 DevKit
- DHT11 sensor (temperature, humidity)
- MQ135 sensor (air quality)
- Sound sensor
- LCD I2C
- Buzzer, LED, relay for device control

## Libraries Used
- [Firebase ESP Client](https://github.com/mobizt/Firebase-ESP-Client)
- DHT sensor library
- ArduinoJson
- LiquidCrystal_I2C

## Source Structure
- `src/main.cpp`: Main project code
- `include/secrets.h`: Manage secret information (API key, email, password, project id)

## Configuration Guide
1. **Install PlatformIO** on VSCode.
2. **Install required libraries** (PlatformIO will auto-install via `platformio.ini`).
3. **Configure secret information:**
   - Edit `include/secrets.h` with your Firebase information:
     ```cpp
     #define API_KEY "<your-api-key>"
     #define FIREBASE_PROJECT_ID "<your-project-id>"
     #define USER_EMAIL "<your-email>"
     #define USER_PASSWORD "<your-password>"
     ```
4. **Build and upload:**
   - Connect ESP32 to your computer.
   - Click `Upload` in PlatformIO or use the command:
     ```sh
     pio run --target upload
     ```

## Security Note
- Do not commit the `include/secrets.h` file to a public repository.
- Add the following line to `.gitignore` if needed:
  ```
  include/secrets.h
  ```

## Author
- Tran Dai Vi
