// Include required libraries
#if defined(ESP32)
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#endif
#include <Firebase_ESP_Client.h>
#include <addons/TokenHelper.h>
#include <DHT.h>  // Include the DHT sensor library
#include <WiFiMulti.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <ArduinoJson.h>

// Define DHT sensor parameters
#define DHTPIN 2
#define DHTTYPE DHT11

// Define MQ-135 Sensor
#define MQ135 35
#define BUZZER 18
int threshold = 2200; //Threshold level for Air Quality

// Define Sound Sensor
#define SOUNDSENSOR 34

// Define Led
#define LED 19 // Not pin ADC
#define TV 17
#define CEILINGFAN 16

// Define Fan
#define FAN 5

// Define Firebase API Key, Project ID, and user credentials
#define API_KEY "AIzaSyBglT5JoUEMpSIeYqwQ5nU-XD8g8Sram6w"
#define FIREBASE_PROJECT_ID "greenhouse-16b3b"
#define USER_EMAIL "vitran6366@gmail.com"
#define USER_PASSWORD "Trandaivi0910@"

// Define Firebase Data object, Firebase authentication, and configuration
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

// Initialize the DHT sensor
DHT dht(DHTPIN, DHTTYPE);

WiFiMulti wifiMulti;

unsigned long time1;
unsigned long time2;

void setup() {
  // Initialize serial communication for debugging
  Serial.begin(115200);

  // Initialize the DHT sensor
  dht.begin();

  // Set up MQ135
  pinMode(MQ135, INPUT);
  pinMode(BUZZER, OUTPUT);

  // Setup Sound Sensor
  pinMode(SOUNDSENSOR, INPUT);

  //  Setup Led
  pinMode(LED, OUTPUT);
  pinMode(FAN, OUTPUT);
  pinMode(CEILINGFAN, OUTPUT);
  pinMode(TV, OUTPUT);

  // Connect to Wi-Fi
  wifiMulti.addAP("PTIT.HCM_SV", "");
  wifiMulti.addAP("PTIT.HCM_CanBo", "");
  wifiMulti.addAP("Thai Bao", "0869334749");
  wifiMulti.addAP("Mixue 107 Man Thien", "107manthien");
  wifiMulti.addAP("Big Nest Tea House", "camonquykhach");
  wifiMulti.addAP("Van_Phong", "19731973");
  wifiMulti.addAP("Furin Blanc", "mmmmmmmm");
  Serial.print("Connecting to Wi-Fi");
  while (wifiMulti.run() != WL_CONNECTED) {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  // Print Firebase client version
  Serial.printf("Firebase Client v%s\n\n", FIREBASE_CLIENT_VERSION);

  // Assign the API key
  config.api_key = API_KEY;

  // Assign the user sign-in credentials
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;

  // Begin Firebase with configuration and authentication
  Firebase.begin(&config, &auth);

  // Reconnect to Wi-Fi if necessary
  Firebase.reconnectWiFi(true);

}

void loop() {

  int ppm = analogRead(MQ135); //read MQ135 analog outputs at A0 and store it in ppm
  String ppmString = String(ppm);
  int soundSensor = analogRead(SOUNDSENSOR);
  String ssString = String(soundSensor);
  // Define the path to the Firestore document
  String documentPath = "2A08/Sensor";
  // Create a FirebaseJson object for storing data
  FirebaseJson content;

  String path = "2A08";
  Serial.print("Get entire collection... ");
  if (Firebase.Firestore.getDocument(&fbdo, FIREBASE_PROJECT_ID, "", path.c_str(), ""))
  {
    Serial.println("ok");

    StaticJsonDocument<1024> doc;
    DeserializationError error = deserializeJson(doc, fbdo.payload().c_str());

    if (!error)
    {
      for (JsonObject document : doc["documents"].as<JsonArray>())
      {
        const char *document_name = document["name"];

        const bool stateValue = document["fields"]["state"]["booleanValue"];

        if (strstr(document_name, "Led") != nullptr)
        {
          digitalWrite(LED, stateValue);
          stateValue ? Serial.print("Led On"): Serial.print("Led OFF");  
        } else if (strstr(document_name, "Fan") != nullptr)
        {
          digitalWrite(CEILINGFAN, stateValue);
          stateValue ? Serial.println("Ceiling Fan On"): Serial.println("Ceiling Fan Off");  
        } else if (strstr(document_name, "TV") != nullptr)
        {
          digitalWrite(TV, stateValue);
          stateValue ? Serial.println("TV On"): Serial.println("TV Off");  
        }

      }
    }
  }
  
  if (soundSensor > threshold){
    Serial.println("Sound Level HIGH");
    digitalWrite(BUZZER, HIGH);
    time1 = millis();
  }
  if (millis() - time1 > 10000){
    digitalWrite(BUZZER, LOW);
  }
  
  if (ppm > threshold){ 
    Serial.println("AQ Level HIGH");                                                                                                                                                                                                     
    digitalWrite(FAN, HIGH);
    time2 = millis();
  }
  if (millis() - time2 > 20000){
    digitalWrite(FAN, LOW);
  }

  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();
  char bufferTemperature[20];
  char bufferHumidity[20];

  sprintf(bufferTemperature, "%.1f", temperature);
  sprintf(bufferHumidity, "%.1f", humidity);

  Serial.println(temperature);
  Serial.println(humidity);

  // Check if the values are valid (not NaN)
  if (!isnan(temperature) && !isnan(humidity) && !isnan(ppm) && !isnan(soundSensor)) {

    // Set the 'Temperature' and 'Humidity' fields in the FirebaseJson object
    content.set("fields/Temperature/stringValue", String(bufferTemperature));
    content.set("fields/Humidity/stringValue", String(bufferHumidity));
    content.set("fields/AirQuality/stringValue", ppmString);
    content.set("fields/SoundQuality/stringValue", ssString);

    Serial.print("Update/Add Data... ");

    // Use the patchDocument method to update the Temperature and Humidity Firestore document
    if (
      Firebase.Firestore.patchDocument(&fbdo, FIREBASE_PROJECT_ID, "", documentPath.c_str(), content.raw(), "Temperature") 
      && Firebase.Firestore.patchDocument(&fbdo, FIREBASE_PROJECT_ID, "", documentPath.c_str(), content.raw(), "Humidity")
      && Firebase.Firestore.patchDocument(&fbdo, FIREBASE_PROJECT_ID, "", documentPath.c_str(), content.raw(), "AirQuality")
      && Firebase.Firestore.patchDocument(&fbdo, FIREBASE_PROJECT_ID, "", documentPath.c_str(), content.raw(), "SoundQuality")
    ) {
      Serial.printf("ok\n%s\n\n", fbdo.payload().c_str());
    } else {
      Serial.println(fbdo.errorReason());
    }
  } else {
    Serial.println("Failed to read data.");
  }
}
