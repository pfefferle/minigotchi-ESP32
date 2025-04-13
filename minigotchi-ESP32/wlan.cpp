/*
 * Minigotchi: An even smaller Pwnagotchi
 * Copyright (C) 2024 dj1ch
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * wlan.cpp: implementation of WLAN management
 */

#include "wlan.h"

// Get first instance of mood class
Mood &WLAN::mood = Mood::getInstance();

// Connection timing constants
const unsigned long WLAN::connectionTimeout = 10000; // 10 seconds
const unsigned long WLAN::reconnectInterval = 30000; // 30 seconds

unsigned long WLAN::lastConnectionAttempt = 0;

void WLAN::setup() {
  if (!Config::wlanEnabled) {
    return;
  }

  WiFi.mode(WIFI_MODE_APSTA); // Enable both AP and STA modes
  WiFi.begin(Config::wlanSsid, Config::wlanPassword);
  lastConnectionAttempt = millis();
}

void WLAN::connect() {
  if (!Config::wlanEnabled) {
    return;
  }

  if (WiFi.status() != WL_CONNECTED) {
    unsigned long currentTime = millis();

    // Check if we should attempt to reconnect
    if (currentTime - lastConnectionAttempt >= reconnectInterval) {
      Serial.println(mood.getNeutral() + " Attempting to connect to WLAN...");
      Display::updateDisplay(mood.getNeutral(), "Connecting to WLAN...");

      WiFi.disconnect();
      WiFi.begin(Config::wlanSsid, Config::wlanPassword);
      lastConnectionAttempt = currentTime;
    }
  }
}

void WLAN::disconnect() {
  WiFi.disconnect();
  Serial.println(mood.getNeutral() + " Disconnected from WLAN");
  Display::updateDisplay(mood.getNeutral(), "Disconnected from WLAN");
}

bool WLAN::isConnected() {
  return WiFi.status() == WL_CONNECTED;
}

void WLAN::loop() {
  if (!Config::wlanEnabled) {
    return;
  }

  if (WiFi.status() == WL_CONNECTED) {
    static bool wasConnected = false;
    if (!wasConnected) {
      Serial.println(mood.getHappy() + " Connected to WLAN!");
      Serial.print(mood.getHappy() + " IP address: ");
      Serial.println(WiFi.localIP());
      Display::updateDisplay(mood.getHappy(), "WLAN Connected!");
      wasConnected = true;
    }
  } else {
    connect();
  }
}

void WLAN::setCredentials(const char* ssid, const char* password) {
  Config::wlanSsid = ssid;
  Config::wlanPassword = password;
}
void WLAN::enable(bool enabled) {
  Config::wlanEnabled = enabled;
  if (!enabled) {
    disconnect();
  }
}
