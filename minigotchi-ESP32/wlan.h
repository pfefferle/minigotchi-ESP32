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
 * wlan.h: header file for WLAN management
 */

#ifndef WLAN_H
#define WLAN_H

#include "config.h"
#include "display.h"
#include "mood.h"
#include <WiFi.h>

// forward declaration of mood class
class Mood;

class WLAN {
public:
  static void setup();
  static void connect();
  static void disconnect();
  static bool isConnected();
  static void loop();
  static void setCredentials(const char* ssid, const char* password);
  static void enable(bool enabled);

private:
  static Mood &mood;
  static unsigned long lastConnectionAttempt;
  static const unsigned long connectionTimeout;
  static const unsigned long reconnectInterval;
};

#endif // WLAN_H