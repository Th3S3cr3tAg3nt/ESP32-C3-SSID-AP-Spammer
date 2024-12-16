# ESP32-C3 SSID / Access Point Spammer

## What is it?
It is a battery powered, very fast, and very small mini ESP32-C3 based SSID Access Point Spammer. It generates hundreds of phantom access points with random strings within seconds of being turned on.

This is intended for pentesters and security researchers.

![Photo of completed ESP32 spammer](esp32-c3-spammer.png)

## Why?
* To test your Wireless Intrusion Protection System (it should light up like a christmas tree)
* To obscure other WiFi access points, by burying the real SSID under hundreds of fake ones.
* To delay someone from connecting to a new WiFi access point, by making the correct WiFi hard to find.
* To poison SSID geo location data, by feeding war drivers with false information.
* To distract SOC teams and/or physical security teams.

## Parts needed
### ESP32 Board
Almost any ESP32 development board will do, but I used an ESP32C3 Supermini.

### Battery Holder
This project uses a single CR2032 battery holder with on/off switch to match the size of the device.

### Battery
LIR2032 coin battery.
The standard CR2032 battery only supplies 3 volts which is not enough to power the ESP32C3 board. LIR2032 batteries are rechargleable and supply up to 3.7 volts.

## Assembly
Cut the wires on the coin battery holder to a suitable length (2-3and strip the ends.
Solder the black cable to ground pin pin labeled G
Solder the red cable to the pin labeled 3V3

