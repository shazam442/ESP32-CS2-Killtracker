// KilltrackerWebManager.h

#ifndef PORT
#define PORT (uint16_t)80 // Default to 80 if PORT is not defined
#endif

#include <WiFi.h>
#include <WiFiClientSecure.h>
#include "config.h"
#include <ArduinoJson.h>

namespace KilltrackerWebManager {
    int kills = 6969;
    const String steam_id = "76561198291413327";
    WiFiClientSecure client;

    unsigned long lastRequestTime = 0;
    const unsigned long postingIntervalMS = 5L * 1000L;

    void readResponse() {
        // JsonDocument doc;

        while (client.available()) {
          char c = client.read();
          Serial.print(c);
        }

        // DeserializationError error = deserializeJson(doc, rawJsonResponse);
        // if (error) {
        //     Serial.print(F("deserializeJson() failed: "));
        //     Serial.println(error.f_str());
        //     return doc;
        // }

        // Serial.print("PARSED JSON: ");
        // serializeJson(doc, Serial);
    }

    void connectToWifi() {
      Serial.print("CONNECTING TO WIFI: ");
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
    }

    void fetchKillcount() {
      client.stop();
      
      Serial.println("Attempting to connect to: " + String(HOSTNAME) + " at port " + String(PORT));
      if (client.connect(SERVER, PORT)) {
        Serial.println("CONNECTION SUCCESS");
        
        client.println("GET /steam_accounts/" + steam_id + ".json" + " HTTP/1.1");
        client.println("Host: " + String(HOSTNAME));
        client.println("User-Agent: ArduinoWiFi/1.1");
        client.println("Connection: close");
        client.println();

      } else {
        Serial.println("CONNECTION FAILED");
      }
      lastRequestTime = millis();
    }

    void updateKillcountTask(void *parameter) {
      while (true) {
        readResponse();

        if (millis() - lastRequestTime > postingIntervalMS) {
          fetchKillcount();
        }
        
        // Serial.print("KILLS: ");
        // Serial.println(kills);
        vTaskDelay(50 / portTICK_PERIOD_MS);
      }
    }

    int getKills() {
        return kills;
    }
};