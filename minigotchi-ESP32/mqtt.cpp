#include "mqtt.h"
#include "config.h"

WiFiClient MQTT::wifiClient;
PubSubClient MQTT::mqttClient(wifiClient);

void MQTT::setup() {
  mqttClient.setServer(Config::mqttServer, Config::mqttPort);
  mqttClient.setCallback(callback);
  reconnect();
  publishDiscovery();
}

void MQTT::loop() {
  if (!mqttClient.connected()) {
    reconnect();
  }
  mqttClient.loop();
}

void MQTT::reconnect() {
  while (!mqttClient.connected()) {
    if (mqttClient.connect(Config::mqttClientId, Config::mqttUser, Config::mqttPassword)) {
      publishDiscovery();
    } else {
      delay(5000);
    }
  }
}

void MQTT::callback(char* topic, byte* payload, unsigned int length) {
  // Handle incoming messages if needed
}

void MQTT::publishDiscovery() {
  // Publish device info
  publishDeviceInfo();

  // Publish device tracker
  publishDeviceTracker("pwnagotchi_tracker");

  // Publish binary sensors
  publishBinarySensor("pwnagotchi_detected", false, "presence");
  publishBinarySensor("deauth_enabled", Config::deauth, "running");
  publishBinarySensor("advertise_enabled", Config::advertise, "running");
  publishBinarySensor("scan_enabled", Config::scan, "running");
  publishBinarySensor("spam_enabled", Config::spam, "running");
  publishBinarySensor("parasite_mode", Config::parasite, "running");
  publishBinarySensor("display_enabled", Config::display, "running");

  // Publish sensors
  publishSensor("uptime", String(Config::uptime).c_str(), "s", "duration");
  publishSensor("epoch", String(Config::epoch).c_str(), "", "timestamp");
  publishSensor("pwnd_run", String(Config::pwnd_run).c_str(), "", "total");
  publishSensor("pwnd_total", String(Config::pwnd_tot).c_str(), "", "total");
  publishSensor("current_channel", String(Config::channel).c_str(), "", "signal_strength");
  publishSensor("free_heap", String(ESP.getFreeHeap()).c_str(), "bytes", "data_size");
  publishSensor("cpu_frequency", String(ESP.getCpuFreqMHz()).c_str(), "MHz", "frequency");
}

void MQTT::publishDeviceInfo() {
  StaticJsonDocument<512> doc;

  // Device info
  doc["identifiers"] = Config::mqttDeviceId;
  doc["name"] = Config::mqttDeviceName;
  doc["manufacturer"] = Config::mqttDeviceManufacturer;
  doc["model"] = Config::mqttDeviceModel;
  doc["sw_version"] = Config::mqttDeviceVersion;

  char topic[128];
  snprintf(topic, sizeof(topic), "%s/sensor/%s/config", Config::mqttDiscoveryPrefix, Config::mqttDeviceId);

  char buffer[512];
  serializeJson(doc, buffer);

  mqttClient.publish(topic, buffer, true);
}

void MQTT::publishDeviceTracker(const char* name) {
  StaticJsonDocument<512> doc;

  char objectId[64];
  snprintf(objectId, sizeof(objectId), "%s_%s", Config::mqttDeviceId, name);

  doc["name"] = "Pwnagotchi Tracker";
  doc["unique_id"] = objectId;
  doc["state_topic"] = Config::mqttPwnagotchiTopic;
  doc["value_template"] = "{{ value_json.pwnagotchi_name }}";
  doc["json_attributes_topic"] = Config::mqttPwnagotchiTopic;
  doc["json_attributes_template"] = "{{ value_json | tojson }}";
  doc["device"]["identifiers"] = Config::mqttDeviceId;
  doc["source_type"] = "bluetooth_le";

  char topic[128];
  snprintf(topic, sizeof(topic), "%s/device_tracker/%s/config", Config::mqttDiscoveryPrefix, objectId);

  char buffer[512];
  serializeJson(doc, buffer);

  mqttClient.publish(topic, buffer, true);
}

void MQTT::publishBinarySensor(const char* name, bool state, const char* deviceClass) {
  StaticJsonDocument<512> doc;

  char objectId[64];
  snprintf(objectId, sizeof(objectId), "%s_%s", Config::mqttDeviceId, name);

  doc["name"] = name;
  doc["unique_id"] = objectId;
  doc["state_topic"] = Config::mqttStateTopic;
  doc["value_template"] = "{{ value_json." + String(name) + " }}";
  doc["device_class"] = deviceClass;
  doc["device"]["identifiers"] = Config::mqttDeviceId;

  char topic[128];
  snprintf(topic, sizeof(topic), "%s/binary_sensor/%s/config", Config::mqttDiscoveryPrefix, objectId);

  char buffer[512];
  serializeJson(doc, buffer);

  mqttClient.publish(topic, buffer, true);

  // Publish current state
  StaticJsonDocument<256> stateDoc;
  stateDoc[name] = state;
  char stateBuffer[256];
  serializeJson(stateDoc, stateBuffer);
  mqttClient.publish(Config::mqttStateTopic, stateBuffer);
}

void MQTT::publishSensor(const char* name, const char* value, const char* unit, const char* deviceClass) {
  StaticJsonDocument<512> doc;

  char objectId[64];
  snprintf(objectId, sizeof(objectId), "%s_%s", Config::mqttDeviceId, name);

  doc["name"] = name;
  doc["unique_id"] = objectId;
  doc["state_topic"] = Config::mqttStateTopic;
  doc["value_template"] = "{{ value_json." + String(name) + " }}";
  if (unit) doc["unit_of_measurement"] = unit;
  if (deviceClass) doc["device_class"] = deviceClass;
  doc["device"]["identifiers"] = Config::mqttDeviceId;

  char topic[128];
  snprintf(topic, sizeof(topic), "%s/sensor/%s/config", Config::mqttDiscoveryPrefix, objectId);

  char buffer[512];
  serializeJson(doc, buffer);

  mqttClient.publish(topic, buffer, true);

  // Publish current state
  StaticJsonDocument<256> stateDoc;
  stateDoc[name] = value;
  char stateBuffer[256];
  serializeJson(stateDoc, stateBuffer);
  mqttClient.publish(Config::mqttStateTopic, stateBuffer);
}

void MQTT::publishState() {
  if (!mqttClient.connected()) {
    return;
  }

  StaticJsonDocument<1024> doc;

  // Add minigotchi state information
  doc["name"] = Config::name;
  doc["face"] = Config::face;
  doc["mood"] = Config::face;
  doc["uptime"] = Config::uptime;
  doc["epoch"] = Config::epoch;
  doc["pwnd_run"] = Config::pwnd_run;
  doc["pwnd_tot"] = Config::pwnd_tot;
  doc["version"] = Config::version;
  doc["current_channel"] = Config::channel;
  doc["free_heap"] = ESP.getFreeHeap();
  doc["cpu_frequency"] = ESP.getCpuFreqMHz();
  doc["deauth_enabled"] = Config::deauth;
  doc["advertise_enabled"] = Config::advertise;
  doc["scan_enabled"] = Config::scan;
  doc["spam_enabled"] = Config::spam;
  doc["parasite_mode"] = Config::parasite;
  doc["display_enabled"] = Config::display;

  char buffer[1024];
  serializeJson(doc, buffer);

  mqttClient.publish(Config::mqttStateTopic, buffer);
}

void MQTT::publishPwnagotchiDetected(const char* pwnagotchiName) {
  if (!mqttClient.connected()) {
    return;
  }

  StaticJsonDocument<256> doc;
  doc["pwnagotchi_name"] = pwnagotchiName;
  doc["timestamp"] = Config::epoch;
  doc["pwnagotchi_detected"] = true;
  doc["state"] = "home"; // Device tracker state

  char buffer[256];
  serializeJson(doc, buffer);

  mqttClient.publish(Config::mqttPwnagotchiTopic, buffer);
  mqttClient.publish(Config::mqttStateTopic, buffer); // Also update the state topic
}

bool MQTT::isConnected() {
  return mqttClient.connected();
}