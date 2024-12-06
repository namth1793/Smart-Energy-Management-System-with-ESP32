#include <Arduino.h>
#include <PubSubClient.h>
#include <FirebaseClient.h>
#include <WiFiClientSecure.h>
#include <WiFi.h>
#include <PZEM004Tv30.h>

#define RelayPin 2
// PZEM connection parameters
#define PZEM_RX_PIN 16
#define PZEM_TX_PIN 17
#define PZEM_SERIAL Serial2
PZEM004Tv30 pzem(PZEM_SERIAL, PZEM_RX_PIN, PZEM_TX_PIN);

// 2.4 GHz WiFi name and password
#define WIFI_SSID  "SevenTeen DuPhong" // "Hoai Nam" 
#define WIFI_PASSWORD "xincamon" // "17092003"
// The API key can be obtained from Firebase console > Project Overview > Project settings.
#define API_KEY "AIzaSyC8TiPqUtvYuUmQnKv9OXLULc1ENnltZ6k"

// User Email and password that already registerd or added in your project.
#define USER_EMAIL "namtranhoai179.3@gmail.com"
#define USER_PASSWORD "17092003"
#define DATABASE_URL "https://energy-system-esp32-default-rtdb.asia-southeast1.firebasedatabase.app/"

// MQTT Broker Settings
const char *mqtt_broker = "broker.emqx.io";
const char *mqtt_topic = "emqx/esp32";
const char *mqtt_username = "namth1793";
const char *mqtt_password = "17092003";
const int mqtt_port = 1883;

// Function Declarations
void connectToMQTT();
void mqttcallback(char *topic, byte *payload, unsigned int length);
void pushData(String messageData);
void getVoiceData();
int voiceControl();

void authHandler();
void printResult(AsyncResult &aResult);
void printError(int code, const String &msg);
DefaultNetwork network;  // initilize with boolean parameter to enable/disable network reconnection
UserAuth user_auth(API_KEY, USER_EMAIL, USER_PASSWORD);
FirebaseApp app;
WiFiClientSecure ssl_client;

WiFiClient espClient;
PubSubClient mqtt_client(espClient);

using AsyncClient = AsyncClientClass;
AsyncClient aClient(ssl_client, getNetwork(network));
RealtimeDatabase Database;
AsyncResult aResult_no_callback;

void setup() {
  // Connect to WiFi
  Serial.begin(9600);
  pinMode(RelayPin, OUTPUT);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();
  // Setup MQTT
  mqtt_client.setServer(mqtt_broker, mqtt_port);
  mqtt_client.setKeepAlive(60);
  mqtt_client.setCallback(mqttcallback);  // Corrected callback function name
  connectToMQTT();
  // Setup Firestore
  Firebase.printf("Firebase Client v%s\n", FIREBASE_CLIENT_VERSION);
  Serial.println("Initializing app...");
  ssl_client.setInsecure();
  initializeApp(aClient, app, getAuth(user_auth), aResult_no_callback);
  authHandler();
  app.getApp<RealtimeDatabase>(Database);
  Database.url(DATABASE_URL);
  aClient.setAsyncResult(aResult_no_callback);
}

void loop() {
  // Print the custom address of the PZEM
  digitalWrite(RelayPin, HIGH);
  Serial.print("Custom Address:");
  Serial.println(pzem.readAddress(), HEX);
  // Read the data from the sensor
  float energy = pzem.energy();
  // Check if the data is valid
  if (isnan(energy)) {
    Serial.println("Error reading energy");
  } else {
    // Print the values to the Serial console
    Serial.print("Energy: ");
    Serial.print(energy, 5);
    Serial.println("kWh");
  }
  Serial.println();
  delay(1000);
  char buffer[20];
  sprintf(buffer, "%f", energy);
  mqtt_client.publish(mqtt_topic, buffer);  // Publish message upon successful connection
  mqtt_client.subscribe(mqtt_topic);
  mqtt_client.loop();
  authHandler();
  getVoiceData();
  Database.loop();
  delay(2000);
}

void pushData(String messageData) {
  object_t json;
  JsonWriter writer;
  writer.create(json, "Today/value", random(100, 1000));
  // Save MQTT message to Firebase
  Serial.print("Push Data...\n");
  bool status = Database.update(aClient, "/Energy", json);
  if (status) {
    Serial.println("Finish Update");
  } else {
    printError(aClient.lastError().code(), aClient.lastError().message());
  }
}

void getVoiceData() {
  Serial.print("Get Voice... ");
  String voice = Database.get<String>(aClient, "Energy/Voice/Today");
  if (aClient.lastError().code() == 0)
    Serial.println(voice);
  else
    printError(aClient.lastError().code(), aClient.lastError().message());
}

void mqttcallback(char *topic, byte *payload, unsigned int length) {
  Serial.print("Message received on mqtt_topic: ");
  Serial.println(mqtt_topic);
  Serial.print("Message: ");
  for (unsigned int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println("\n-----------------------");
  // Convert payload to string
  String message;
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  pushData(message);
}

void authHandler() {
  // Blocking authentication handler with timeout
  unsigned long ms = millis();
  while (app.isInitialized() && !app.ready() && millis() - ms < 120 * 1000) {
    // The JWT token processor required for ServiceAuth and CustomAuth authentications.
    // JWT is a static object of JWTClass and it's not thread safe.
    // In multi-threaded operations (multi-FirebaseApp), you have to define JWTClass for each FirebaseApp,
    // and set it to the FirebaseApp via FirebaseApp::setJWTProcessor(<JWTClass>), before calling initializeApp.
    JWT.loop(app.getAuth());
    printResult(aResult_no_callback);
  }
}

void connectToMQTT() {
  while (!mqtt_client.connected()) {
    String client_id = "esp32-client-" + String(WiFi.macAddress());
    Serial.printf("Connecting to MQTT Broker as %s.....\n", client_id.c_str());
    if (mqtt_client.connect(client_id.c_str(), mqtt_username, mqtt_password)) {
      Serial.println("Connected to MQTT broker");
    } else {
      Serial.print("Failed, rc=");
      Serial.print(mqtt_client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void printResult(AsyncResult &aResult) {
  if (aResult.isEvent()) {
    Firebase.printf("Event task: %s, msg: %s, code: %d\n", aResult.uid().c_str(), aResult.appEvent().message().c_str(), aResult.appEvent().code());
  }

  if (aResult.isDebug()) {
    Firebase.printf("Debug task: %s, msg: %s\n", aResult.uid().c_str(), aResult.debug().c_str());
  }

  if (aResult.isError()) {
    Firebase.printf("Error task: %s, msg: %s, code: %d\n", aResult.uid().c_str(), aResult.error().message().c_str(), aResult.error().code());
  }
}

void printError(int code, const String &msg) {
  Firebase.printf("Error, msg: %s, code: %d\n", msg.c_str(), code);
}
