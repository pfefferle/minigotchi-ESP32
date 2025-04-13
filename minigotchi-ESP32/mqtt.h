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
 * mqtt.h: header file for MQTT functionality
 */

#ifndef MQTT_H
#define MQTT_H

#include <PubSubClient.h>
#include <WiFi.h>
#include <ArduinoJson.h>
#include "config.h"

class MQTT {
public:
  static void setup();
  static void loop();
  static void publishState();
  static void publishPwnagotchiDetected(const char* pwnagotchiName);
  static bool isConnected();
  static void publishDiscovery();
  static void publishBinarySensor(const char* name, bool state, const char* deviceClass = nullptr);
  static void publishSensor(const char* name, const char* value, const char* unit = nullptr, const char* deviceClass = nullptr);
  static void publishDeviceInfo();

private:
  static WiFiClient wifiClient;
  static PubSubClient mqttClient;
  static const char* mqttServer;
  static const int mqttPort;
  static const char* mqttUser;
  static const char* mqttPassword;
  static const char* mqttClientId;
  static const char* mqttStateTopic;
  static const char* mqttPwnagotchiTopic;
  static const char* mqttDiscoveryPrefix;
  static const char* mqttDeviceName;
  static const char* mqttDeviceId;
  static const char* mqttDeviceManufacturer;
  static const char* mqttDeviceModel;
  static const char* mqttDeviceVersion;
  static void reconnect();
  static void callback(char* topic, byte* payload, unsigned int length);
  static void publishDiscoveryMessage(const char* component, const char* objectId, JsonDocument& doc);
};

#endif // MQTT_H