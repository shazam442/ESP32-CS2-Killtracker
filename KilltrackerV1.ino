/*
    Go to thingspeak.com and create an account if you don't have one already.
    After logging in, click on the "New Channel" button to create a new channel for your data. This is where your data will be stored and displayed.
    Fill in the Name, Description, and other fields for your channel as desired, then click the "Save Channel" button.
    Take note of the "Write API Key" located in the "API keys" tab, this is the key you will use to send data to your channel.
    Replace the channelID from tab "Channel Settings" and privateKey with "Read API Keys" from "API Keys" tab.
    Replace the host variable with the thingspeak server hostname "api.thingspeak.com"
    Upload the sketch to your ESP32 board and make sure that the board is connected to the internet. The ESP32 should now send data to your Thingspeak channel at the intervals specified by the loop function.
    Go to the channel view page on thingspeak and check the "Field1" for the new incoming data.
    You can use the data visualization and analysis tools provided by Thingspeak to display and process your data in various ways.
    Please note, that Thingspeak accepts only integer values.

    You can later check the values at https://thingspeak.com/channels/2005329
    Please note that this public channel can be accessed by anyone and it is possible that more people will write their values.
 */

// KilltrackerV1.ino

#include "SevSeg.h"
#include "KilltrackerWebManager.h"


SevSeg sevseg; //Instantiate a seven segment object

void startSevenSegmentDisplay() {
  byte numDigits = 4;
  byte digitPins[] = {4, 18, 19, 26};
  byte segmentPins[] = {16, 21, 14, 23, 22, 17, 27, 13};
  bool resistorsOnSegments = false; // 'false' means resistors are on digit pins
  byte hardwareConfig = COMMON_CATHODE; // See README.md for options
  bool updateWithDelays = false; // Default 'false' is Recommended
  bool leadingZeros = false; // Use 'true' if you'd like to keep the leading zeros
  bool disableDecPoint = false; // Use 'true' if your decimal point doesn't exist or isn't connected. Then, you only need to specify 7 segmentPins[]

  sevseg.begin(hardwareConfig, numDigits, digitPins, segmentPins, resistorsOnSegments,
  updateWithDelays, leadingZeros, disableDecPoint);


  sevseg.setBrightness(80);
}

void setup() {
  Serial.begin(115200);
  while (!Serial) { delay(50); }

  Serial.println();
  Serial.println("******************************************************");

  KilltrackerWebManager::connectToWifi();

  startSevenSegmentDisplay();
  xTaskCreate(KilltrackerWebManager::updateKillcountTask, "FetchKillcount", 2048, NULL, 1, NULL);
  xTaskCreate(refreshDisplayTask, "RefreshDisplay", 2048, NULL, 1, NULL);
}

void refreshDisplayTask(void *parameter) {
  while (true) {
    sevseg.setNumber(KilltrackerWebManager::getKills());
    sevseg.refreshDisplay();
    vTaskDelay(1 / portTICK_PERIOD_MS); // Refresh every 1 ms
  }
}



void loop() {
  
}