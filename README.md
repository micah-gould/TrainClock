# TrainClock 🕒

TrainClock is a simple tool to display train arrival times on a microcontroller display. It fetches data from your favorite train platform and shows the next train times directly on your device.

## Features

* Displays next train arrival times
* Works with any supported microcontroller (like Raspberry Pi Pico)
* Configurable for a specific platform
* WiFi-enabled for live updates

## Getting Started

### 1. Install the Executable

Copy `trainclock` to your system PATH and make it executable:

```bash
cp trainclock /usr/local/bin/
chmod +x /usr/local/bin/trainclock
```

### 2. Initialize the Server

Set up the server for your device:

```bash
trainclock initialize
```

This will configure everything needed to start fetching train data.

### 3. Start the Server

Once initialized, start the server:

```bash
trainclock start
```

The server will begin running and providing train times to your display.

### 4. Optional: Configure a Specific Platform

If you only want train times from a specific platform:

1. Find the **latitude** of the platform.
2. Set it in the .env file

This ensures you only see times relevant to your chosen platform.

### 5. Optional: Configure Your Microcontroller

If you're using your own Raspberry Pi Pico (or other microcontroller), you may need to adjust the `PinMap` to ensure the display shows the correct numbers.

Check the `PinMap` in the code and update according to your wiring.

### 6. WiFi Configuration

Set your WiFi credentials in the `config.h` file:

```c
#define WIFI_SSID "YourWiFiName"
#define WIFI_PASSWORD "YourWiFiPassword"
```

This allows your device to connect to the internet and fetch live train times.

## Pico Error Codes

| Code | Description                                                          |
| ---- | -------------------------------------------------------------------- |
| 0    | Failure to initialize                                                |
| 1    | Failure to connect to WiFi                                           |
| 2    | Error with API                                                       |
| 3    | Error with server (No data present)                                  |
| 4    | Error with server (Data available, but missing `nextTrainInMinutes`) |
