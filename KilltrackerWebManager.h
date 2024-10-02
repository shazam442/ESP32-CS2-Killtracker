// KilltrackerWebManager.h

#ifndef PORT
#define PORT 80 // Default to 80 if PORT is not defined
#endif

#include <WiFi.h>
#include "config.h"
#include <ArduinoJson.h>

namespace KilltrackerWebManager {
    int kills = 6969;
    const String steam_id = "76561198291413327";

    JsonDocument readResponse(NetworkClient *client) {
        JsonDocument doc;

        unsigned long timeout = millis();
        while (client->available() == 0) {
            if (millis() - timeout > 5000) {
                Serial.println(">>> Client Timeout !");
                client->stop();
                return doc;
            }
        }

        // Read headers
        while (client->available()) {
            String header = client->readStringUntil('\n');  // Read until newline (\n)
            
            // Check for an empty line which indicates the end of headers
            if (header == "\r") {
                break;
            }
        }

        // Now, read the body (JSON)
        String rawJsonResponse = client->readString();
        DeserializationError error = deserializeJson(doc, rawJsonResponse);

        if (error) {
            Serial.print(F("deserializeJson() failed: "));
            Serial.println(error.f_str());
            return doc;
        }

        Serial.print("PARSED JSON: ");
        serializeJson(doc, Serial);
        Serial.printf("\nClosing connection\n\n");
        return doc;
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

    void fetchKillcountTask(void *parameter) {
        while (true) {
            NetworkClient client;
            String footer = String(" HTTP/1.1\r\n") + "HOST: " + String(HOST) + "\r\n" + "Connection: close\r\n\r\n";

            String getSteamUserRequestURL = "GET /steam_account/" + steam_id + ".json" + footer;

            if (!client.connect(HOST, PORT)) {
                Serial.println("COULD NOT CONNECT TO HOST");
                return;
            }

            Serial.println("CONNECTING TO " + getSteamUserRequestURL);
            client.print(getSteamUserRequestURL);
            JsonDocument data = readResponse(&client);

            // Update kills from response
            kills = data["kills"];
            Serial.print("KILLS: ");
            Serial.println(kills);

            vTaskDelay(2000 / portTICK_PERIOD_MS); // Refresh every 2 seconds
        }
    }

    int getKills() {
        return kills;
    }
};