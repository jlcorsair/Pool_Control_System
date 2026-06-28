To achieve your goal, the best topology is a **Point-to-Point (PtP) link** where Node A handles your sensor, Node B handles the ESP32, and they bridge the connection wirelessly. Because both nodes need to accept local Wi-Fi connections (from the sensor and the ESP32), you should configure them using the **Access Point** and **Bridge** modes.

Here is the exact step-by-step configuration layout for your TP-Link CPE210 devices.

---

## 1. Network Topology Overview

* **Sensor** $\rightarrow$ *(Wi-Fi)* $\rightarrow$ **Node A (Access Point)** * **Node A** $\rightarrow$ *(Wireless Link)* $\rightarrow$ **Node B (Bridge Mode)**
* **ESP32** $\rightarrow$ *(Wi-Fi)* $\rightarrow$ **Node B (Bridge Mode)**

> **Crucial Setup Note:** Because the CPE210 uses a highly directional antenna (a 65-degree horizontal beam), **Node A and Node B must face each other physically.** The Sensor must be in front of Node A, and the ESP32 must be in front of Node B to get a reliable Wi-Fi signal.

---

## 2. Configuring Node A (Access Point)

This node will broadcast the primary Wi-Fi network that the Sensor connects to.

1. Connect your computer via Ethernet to the **LAN** port of the PoE injector for Node A.
2. Set your computer's IP manually to `192.168.0.10` (Subnet: `255.255.255.0`).
3. Open a browser, go to `192.168.0.254`, and log in (default is `admin`/`admin`, it will prompt you to change it).
4. Go to the **Quick Setup** tab.
5. Select **Access Point** and click Next.
6. **LAN Settings:** Change the IP Address to `192.168.0.101` (to prevent an IP conflict with Node B). Click Next.
7. **Wireless AP Settings:**
* **SSID:** Create a unique name (e.g., `Sensor_Net_A`).
* **Security:** `WPA-PSK/WPA2-PSK`.
* **PSK Password:** Set a secure password for your sensor.
* *Important:* Uncheck/Disable **Pharos MAXtream**. (If enabled, standard Wi-Fi devices like your sensor and ESP32 will not be able to see the network).


8. Click **Next**, then click **Finish**.

---

## 3. Configuring Node B (Bridge Mode)

Node B will connect wirelessly to Node A to bridge the network, but it will also broadcast its own Wi-Fi SSID for your ESP32 to connect to.

1. Disconnect Node A, and connect your computer to **Node B**’s PoE injector via Ethernet.
2. Navigate to `192.168.0.254` in your browser.
3. Go to **Quick Setup**.
4. Select **Bridge** and click Next.
5. **LAN Settings:** Change the IP Address to `192.168.0.102`. Click Next.
6. **Wireless Client Settings (Connecting to Node A):**
* Click the **Survey** button. A list of nearby networks will appear.
* Find your Node A network (`Sensor_Net_A`) and click **Connect**.
* Select the matching security type and type in the **exact password** you set on Node A.


7. **Wireless AP Settings (For the ESP32):**
* **SSID:** Create a different name (e.g., `ESP32_Net_B`).
* **Security:** `WPA-PSK/WPA2-PSK`.
* **PSK Password:** Set a password for your ESP32 to log in.


8. Click **Next**, then click **Finish**.

---

## 4. Connecting the ESP32 and Sensor

Because you aren't plugging these CPE210s into a main internet router with a active DHCP server, **there is no device automatically handing out IP addresses**. You have two options for your endpoints:

### Option 1: Static IPs (Recommended)

Assign static IPs manually in your Sensor and ESP32 code so they can talk directly across the bridge:

* **Sensor IP:** `192.168.0.20`
* **ESP32 IP:** `192.168.0.30`
* **Subnet Mask:** `255.255.255.0`

### Option 2: Turn on DHCP on Node A

If your sensor or ESP32 absolutely requires DHCP, log back into Node A (`192.168.0.101`), go to the **Network** tab $\rightarrow$ **DHCP Server**, enable it, and set the IP range from `192.168.0.10` to `192.168.0.90`.

---

## 5. ESP32 Code Workflow for Data Hopping

Since the CPE210 network is completely isolated from the internet, your ESP32 will handle the data transfer in two distinct steps:

```text
Step 1: Connect to "ESP32_Net_B" -> Grab sensor logs from 192.168.0.20 -> Save locally to ESP32 RAM/SPIFFS/LittleFS.
Step 2: Disconnect from "ESP32_Net_B".
Step 3: Connect to your Home Local Internet Wi-Fi -> Push stored data to GitHub API -> Sleep/Repeat.

```

Make sure that your home internet Wi-Fi uses a **different subnet** (like `192.168.1.x`) than the CPE210 network (`192.168.0.x`) so the ESP32 doesn't experience routing confusion when switching networks.

---

### What "Lock to AP" Actually Does

**No, you do not need "Lock to AP" enabled** on Node B for this specific setup, but it can be highly beneficial depending on your environment.

Here is why, along with a quick breakdown of whether you should toggle it on:

When Node B connects to Node A, it usually looks for the network name (**SSID**: `Sensor_Net_A`). If "Lock to AP" is **disabled**, Node B will connect to *any* nearby router broadcasting that exact same name.

If you turn "Lock to AP" **enabled**, Node B binds directly to Node A’s unique physical hardware address (**MAC Address**).

---

### Should you use it?

* **Leave it Disabled (Default):** If you ever need to replace Node A in the future because it breaks, you won't have to change any settings on Node B. As long as you give the new Node A the same SSID and password, Node B will automatically find it and bridge the gap.
* **Enable it Only If:** You live in an area with lots of competing Wi-Fi networks and you are worried someone else might accidentally broadcast a network with the exact same name as yours, which could cause Node B to get confused and try to connect to their router instead.

> ⚠️ **Note:** If you do decide to enable "Lock to AP", remember that if you ever swap out Node A for a new device, Node B will completely lose its connection until you log back into its settings and update it with the new MAC address.

For a simple local sensor setup, leaving it **disabled** keeps your maintenance much easier!