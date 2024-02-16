#include <WiFi.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>

// Import sensitive data
#include "WiFiCredentials.h"
#include "HueApiCredentials.h"

// Common variables
#define LED_BUILTIN 2
#define BUTTON_PIN 4
String jsonOn;
String jsonOff;
WiFiClientSecure client;
int buttonState = 0;
bool lastButtonValue = false;
bool buttonValue = false;

void setup() {
	Serial.begin(9600);
	pinMode(LED_BUILTIN, OUTPUT);
	pinMode(BUTTON_PIN, INPUT);
  create_json_objects();
  setup_wifi();
}

void create_json_objects() {
  DynamicJsonDocument doc(1024);
  JsonObject on = doc.createNestedObject("on");
  on["on"].createNestedObject("on");

  on["on"]["on"] = true;
  serializeJson(on, jsonOn);

  on["on"]["on"] = false;
  serializeJson(on, jsonOff);
}

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WIFI_SSID);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
	client.setInsecure();
}

void loop() {
	HTTPClient http;
  buttonState = digitalRead(BUTTON_PIN);
  
  if (buttonState == HIGH) {
    buttonValue = !buttonValue;
    delay(200);
  }

  if (lastButtonValue != buttonValue) {
    lastButtonValue = buttonValue;

    if (buttonValue == 1) {
      digitalWrite(LED_BUILTIN, HIGH);

      Serial.println(jsonOn);

			String url = "https://" + HUE_BRIDGE_IP + "/clip/v2/resource/light/" + LIGHT_1_ID;

      http.begin(client, url);
      http.addHeader("hue-application-key", HUE_APPLICATION_KEY);
      http.addHeader("Content-Type", "application/json");

			http.PUT(jsonOn);
    } else {
      digitalWrite(LED_BUILTIN, LOW);

      Serial.println(jsonOff);

      String url = "https://" + HUE_BRIDGE_IP + "/clip/v2/resource/light/" + LIGHT_1_ID;

      http.begin(client, url);
      http.addHeader("hue-application-key", HUE_APPLICATION_KEY);
      http.addHeader("Content-Type", "application/json");

			http.PUT(jsonOff);
    }
  }
}
