#include <Arduino.h>
#include <WiFi.h>
#include <time.h>

static const char* apSsid = "Montecito-Pool";
static const char* bridgeSsid = "Montecito-Pool-Logger";
static const char* wifiPassword = "montecito_pool_2047";

// Wi-Fi IP Configuration
// Set to true to use Static IPs, or false to use DHCP (requires a DHCP server enabled on the CPE210)
static const bool useStaticIP = true;
static const IPAddress localIP(192, 168, 0, 30);
static const IPAddress subnetMask(255, 255, 255, 0);
static const IPAddress apGatewayIP(192, 168, 0, 101);      // IP of Node A (Access Point)
static const IPAddress bridgeGatewayIP(192, 168, 0, 102);  // IP of Node B (Bridge)

static const unsigned long switchIntervalMs = 30000UL;
static const unsigned long connectTimeoutMs = 20000UL;
static const unsigned long statusReportIntervalMs = 5000UL;
static const unsigned int retryDelaySec = 5;

enum TargetDevice {
  DEVICE_NONE,
  DEVICE_AP,
  DEVICE_BRIDGE,
};

bool testRunning = false;
TargetDevice currentDevice = DEVICE_NONE;
TargetDevice nextDevice = DEVICE_AP;
bool statusPrinted = false;
unsigned long connectedAtMs = 0;
unsigned long connectionStartMs = 0;
unsigned long lastStatusReportMs = 0;

char serialBuffer[80];
uint8_t serialIndex = 0;

const char* wifiStatusToString(wl_status_t status) {
  switch (status) {
    case WL_IDLE_STATUS: return "IDLE";
    case WL_NO_SSID_AVAIL: return "NO_SSID_AVAILABLE";
    case WL_SCAN_COMPLETED: return "SCAN_COMPLETED";
    case WL_CONNECTED: return "CONNECTED";
    case WL_CONNECT_FAILED: return "CONNECT_FAILED";
    case WL_CONNECTION_LOST: return "CONNECTION_LOST";
    case WL_DISCONNECTED: return "DISCONNECTED";
    default: return "UNKNOWN";
  }
}

void printCurrentDateTime() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo, 1000)) {
    Serial.println("Current date/time: NTP time not available yet");
    return;
  }
  char timeString[64];
  strftime(timeString, sizeof(timeString), "%Y-%m-%d %H:%M:%S UTC", &timeinfo);
  Serial.print("Current date/time: ");
  Serial.println(timeString);
}

void printConnectionInfo(const char* targetName) {
  Serial.printf("Connected to %s\n", targetName);
  Serial.printf("SSID: %s\n", WiFi.SSID().c_str());
  Serial.printf("Status: %s\n", wifiStatusToString(WiFi.status()));
  Serial.printf("IP address: %s\n", WiFi.localIP().toString().c_str());
  Serial.printf("RSSI: %d dBm\n", WiFi.RSSI());
  printCurrentDateTime();
  Serial.println("------------------------------");
}

void connectToNetwork(TargetDevice device) {
  const char* ssid = device == DEVICE_AP ? apSsid : bridgeSsid;
  const char* deviceName = device == DEVICE_AP ? "Access Point" : "Bridge";

  Serial.printf("Connecting to %s: %s\n", deviceName, ssid);
  // Scan for networks and report whether the target SSID is visible
  int n = WiFi.scanNetworks();
  bool ssidFound = false;
  for (int i = 0; i < n; ++i) {
    if (WiFi.SSID(i) == String(ssid)) {
      ssidFound = true;
      break;
    }
  }
  Serial.printf("Scan: found %s: %s (%d networks found)\n", deviceName, ssidFound ? "YES" : "NO", n);

  WiFi.disconnect(true, true);
  WiFi.mode(WIFI_STA);

  if (useStaticIP) {
    IPAddress gateway = (device == DEVICE_AP) ? apGatewayIP : bridgeGatewayIP;
    IPAddress dns = gateway; // Set DNS to matching gateway/CPE IP
    if (!WiFi.config(localIP, gateway, subnetMask, dns)) {
      Serial.println("Static IP configuration failed!");
    } else {
      Serial.printf("Static IP configuration applied. Gateway: %s\n", gateway.toString().c_str());
    }
  }

  WiFi.setAutoReconnect(true);
  WiFi.begin(ssid, wifiPassword);
  currentDevice = device;
  nextDevice = (device == DEVICE_AP) ? DEVICE_BRIDGE : DEVICE_AP;
  statusPrinted = false;
  connectionStartMs = millis();
  lastStatusReportMs = millis();
  Serial.printf("Connection attempt started for %s.\n", deviceName);
}

void stopTest() {
  if (!testRunning) {
    Serial.println("Test is not running.");
    return;
  }

  testRunning = false;
  currentDevice = DEVICE_NONE;
  nextDevice = DEVICE_AP;
  statusPrinted = false;
  WiFi.disconnect(true, true);
  Serial.println("Test stopped and Wi-Fi disconnected.");
}

void startTest() {
  if (testRunning) {
    Serial.println("Test is already running.");
    return;
  }

  testRunning = true;
  currentDevice = DEVICE_NONE;
  nextDevice = DEVICE_AP;
  statusPrinted = false;
  Serial.println("Starting Wi-Fi switch test.");
}

void printStatus() {
  Serial.println("--- Test Status ---");
  Serial.printf("Test running: %s\n", testRunning ? "YES" : "NO");
  Serial.printf("Current target: %s\n", currentDevice == DEVICE_AP ? "Access Point" : currentDevice == DEVICE_BRIDGE ? "Bridge" : "None");
  Serial.printf("Next target: %s\n", nextDevice == DEVICE_AP ? "Access Point" : nextDevice == DEVICE_BRIDGE ? "Bridge" : "None");
  Serial.printf("Wi-Fi status: %s\n", wifiStatusToString(WiFi.status()));
  if (WiFi.status() == WL_CONNECTED) {
    Serial.printf("Connected SSID: %s\n", WiFi.SSID().c_str());
    Serial.printf("IP: %s\n", WiFi.localIP().toString().c_str());
  }
  printCurrentDateTime();
  Serial.println("--------------------");
}

String readSerialLine() {
  while (Serial.available() > 0) {
    char c = Serial.read();
    if (c == '\r') {
      continue;
    }
    if (c == '\n') {
      if (serialIndex == 0) {
        continue;
      }
      serialBuffer[serialIndex] = '\0';
      String line = String(serialBuffer);
      serialIndex = 0;
      return line;
    }
    if (serialIndex < sizeof(serialBuffer) - 1) {
      serialBuffer[serialIndex++] = c;
    }
  }
  return String();
}

void processSerialCommands() {
  String line = readSerialLine();
  if (line.length() == 0) {
    return;
  }

  line.trim();
  line.toUpperCase();

  if (line == "START") {
    startTest();
  } else if (line == "STOP") {
    stopTest();
  } else if (line == "STATUS") {
    printStatus();
  } else if (line == "HELP") {
    Serial.println("Commands: START, STOP, STATUS, HELP");
  } else {
    Serial.printf("Unknown command: %s\n", line.c_str());
    Serial.println("Available commands: START, STOP, STATUS, HELP");
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("ESP32 Wi-Fi switch test ready.");
  Serial.println("Type START to begin, STOP to stop, STATUS for current state.");

  WiFi.mode(WIFI_STA);
  WiFi.disconnect(true, true);

  configTime(0, 0, "pool.ntp.org", "time.google.com");
}

void loop() {
  processSerialCommands();

  if (!testRunning) {
    return;
  }

  if (currentDevice == DEVICE_NONE) {
    connectToNetwork(nextDevice);
    return;
  }

  wl_status_t status = WiFi.status();
  if (status == WL_CONNECTED) {
    if (!statusPrinted) {
      const char* deviceName = currentDevice == DEVICE_AP ? "Access Point" : "Bridge";
      printConnectionInfo(deviceName);
      statusPrinted = true;
      connectedAtMs = millis();
    }

    if (millis() - connectedAtMs >= switchIntervalMs) {
      const char* deviceName = currentDevice == DEVICE_AP ? "Access Point" : "Bridge";
      Serial.printf("%u seconds elapsed on %s. Disconnecting...\n", switchIntervalMs / 1000, deviceName);
      WiFi.disconnect(true, true);
      TargetDevice previousDevice = currentDevice;
      currentDevice = DEVICE_NONE;
      nextDevice = (previousDevice == DEVICE_AP) ? DEVICE_BRIDGE : DEVICE_AP;
      statusPrinted = false;
      delay(500);
      return;
    }
  } else {
    if (millis() - lastStatusReportMs >= statusReportIntervalMs) {
      const char* deviceName = currentDevice == DEVICE_AP ? "Access Point" : "Bridge";
      Serial.printf("Still connecting to %s... Wi-Fi status: %s\n", deviceName, wifiStatusToString(status));
      lastStatusReportMs = millis();
    }

    if (currentDevice != DEVICE_NONE && millis() - connectionStartMs >= connectTimeoutMs) {
      Serial.printf("Connection to %s timed out with status %s. Will retry in %u seconds...\n", currentDevice == DEVICE_AP ? "Access Point" : "Bridge", wifiStatusToString(status), retryDelaySec);
      // Countdown before retrying so user can see the delay
      for (unsigned int s = retryDelaySec; s > 0; --s) {
        Serial.printf("Retrying in %u...\n", s);
        delay(1000);
        // allow processing of serial commands (e.g., STOP) during countdown
        processSerialCommands();
      }
      connectToNetwork(nextDevice);
    }
  }
}
