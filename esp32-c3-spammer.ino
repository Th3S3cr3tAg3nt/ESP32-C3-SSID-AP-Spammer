#include <WiFi.h>
#include "esp_wifi.h"

esp_err_t esp_wifi_80211_tx(wifi_interface_t ifx, const void *buffer, int len, bool en_sys_seq);

String alfa = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789~~~~~~~";
char* suffix = "~"; // Just use this to help idetify our own SSIDs
byte channel;
char c;
byte p;

// List of SSIDs to rotate
const char* ssids[] = {
  "1 Never gonna give you up ",
  "2 Never gonna let you down",
  "3 Never gonna run around  ",
  "4 and desert you          ",
  "5 Never gonna make you cry",
  "6 Never gonna say goodbye ",
  "7 Never gonna tell a lie  ",
  "8 and hurt you            "
};

// Number of SSIDs
const int ssidCount = sizeof(ssids) / sizeof(ssids[0]);

int currentSSIDIndex = 0;

// Beacon Packet buffer
uint8_t my_packet[128] = { 0x80, 0x00, 0x00, 0x00, 
                /*4*/   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
                /*10*/  0x01, 0x02, 0x03, 0x04, 0x05, 0x06,
                /*16*/  0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 
                /*22*/  0xc0, 0x6c, 
                /*24*/  0x83, 0x51, 0xf7, 0x8f, 0x0f, 0x00, 0x00, 0x00, 
                /*32*/  0x64, 0x00, 
                /*34*/  0x01, 0x04, 
                /* SSID */
                /*36*/  0x00, 0x12, 0x72, 0x72, 0x72, 0x72, 0x72, 0x5f, 0x52, 0x65, 0x64, 0x54, 0x65, 0x61, 0x6d, 0x5f, 0x46, 0x61, 0x6b, 0x65,
                        0x01, 0x08, 0x82, 0x84,
                        0x8b, 0x96, 0x24, 0x30, 0x48, 0x6c, 0x03, 0x01, 
                /*56*/  0x04};                       

// barebones packet
uint8_t packet[128] = { 0x80, 0x00, 0x00, 0x00, //Frame Control, Duration
                /*4*/   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, //Destination address 
                /*10*/  0x01, 0x02, 0x03, 0x04, 0x05, 0x06, //Source address - overwritten later
                /*16*/  0x01, 0x02, 0x03, 0x04, 0x05, 0x06, //BSSID - overwritten to the same as the source address
                /*22*/  0xc0, 0x6c, //Seq-ctl
                /*24*/  0x83, 0x51, 0xf7, 0x8f, 0x0f, 0x00, 0x00, 0x00, //timestamp - the number of microseconds the AP has been active
                /*32*/  0x64, 0x00, //Beacon interval
                /*34*/  0x01, 0x04, //Capability info
                /* SSID */
                /*36*/  0x00
                };


void setup() {
  Serial.begin(115200);
  Serial.setTimeout(100);
  Serial.println("Sending the stuff...");
  WiFi.mode(WIFI_AP_STA);
 
  esp_wifi_set_promiscuous(true);
  esp_wifi_set_max_tx_power(82);
  
  //Select external antenna
  //this can change between ESP32 dev boards (often 15, or 21)
  pinMode(21,OUTPUT);
  digitalWrite(21, LOW);
  
  //set the onboard LED PIN, this can change between boards
  pinMode(8, OUTPUT);
  digitalWrite(8, LOW);
}

// Function to send SSIDs from the SSID array (slowest)
void broadcastSSIDArray(const char* ssid) {
  // Randomise WiFi channel
  channel = random(1,12); 
  esp_wifi_set_channel(channel, WIFI_SECOND_CHAN_NONE);
  delay(1);  

  // Randomise SRC MAC
  my_packet[10] = my_packet[16] = random(256);
  my_packet[11] = my_packet[17] = random(256);
  my_packet[12] = my_packet[18] = random(256);
  my_packet[13] = my_packet[19] = random(256);
  my_packet[14] = my_packet[20] = random(256);
  my_packet[15] = my_packet[21] = random(256);

  my_packet[37] = 25; // Length
  
  for(int i = 0; i < 25; i++){
    // Get the SSID, randomly upper and lowercase to make it unique
    c = ssid[i];
    if (random(10)>5) {
      if ('a'<=c && c<='z'){
        c=char(((int)c)-32);
      }
      else if ('A'<=c && c<='Z'){
        c=char(((int)c)+32);
      }
    }
    my_packet[38 + i] = c;
  }
  
  // Update the packet with the right channel
  my_packet[38 + 25 + 12] = channel;

  uint8_t postSSID[13] = {0x01, 0x08, 0x82, 0x84, 0x8b, 0x96, 0x24, 0x30, 0x48, 0x6c, //supported rate
                      0x03, 0x01, 0x04 /*DSSS (Current Channel)*/ };

  // Add everything that goes after the SSID
  for(int i = 0; i < 12; i++) 
    my_packet[38 + 25 + i] = postSSID[i];

  esp_wifi_80211_tx(WIFI_IF_AP, my_packet, sizeof(my_packet), false);
  esp_wifi_80211_tx(WIFI_IF_AP, my_packet, sizeof(my_packet), false);
  esp_wifi_80211_tx(WIFI_IF_AP, my_packet, sizeof(my_packet), false);
}

// Function to send random beacons with fixed ESSID length
void broadcastSSIDFixedRandom() {
  channel = random(1,12); 
  esp_wifi_set_channel(channel, WIFI_SECOND_CHAN_NONE);
  delay(1);  

  // Randomize SRC MAC
  packet[10] = packet[16] = random(256);
  packet[11] = packet[17] = random(256);
  packet[12] = packet[18] = random(256);
  packet[13] = packet[19] = random(256);
  packet[14] = packet[20] = random(256);
  packet[15] = packet[21] = random(256);

  packet[37] = 6;
  
  // Randomize SSID (Fixed size 6)
  packet[38] = alfa[random(26)];
  packet[39] = alfa[random(26)];
  packet[40] = alfa[random(26)];
  packet[41] = alfa[random(26)];
  packet[42] = alfa[random(26)];
  packet[43] = alfa[random(26)];
  
  packet[56] = channel;

  uint8_t postSSID[13] = {0x01, 0x08, 0x82, 0x84, 0x8b, 0x96, 0x24, 0x30, 0x48, 0x6c, //supported rate
                      0x03, 0x01, 0x04 /*DSSS (Current Channel)*/ };

  // Add everything that goes after the SSID
  for(int i = 0; i < 12; i++) 
    packet[38 + 6 + i] = postSSID[i];

  esp_wifi_80211_tx(WIFI_IF_AP, packet, sizeof(packet), false);
  esp_wifi_80211_tx(WIFI_IF_AP, packet, sizeof(packet), false);
  esp_wifi_80211_tx(WIFI_IF_AP, packet, sizeof(packet), false);
}

// Function to send random beacons with random SSID length
void broadcastSSIDRandom() {
  int result;
  channel = random(1,12); 
  esp_wifi_set_channel(channel, WIFI_SECOND_CHAN_NONE);
  delay(1);  

  // Randomize SRC MAC
  packet[10] = packet[16] = random(256);
  packet[11] = packet[17] = random(256);
  packet[12] = packet[18] = random(256);
  packet[13] = packet[19] = random(256);
  packet[14] = packet[20] = random(256);
  packet[15] = packet[21] = random(256);

  
  // pick a random length
  int essid_len = random(6, 10);

  // add random characters from alfa string
  uint8_t rand_reg[essid_len] = {};
  for (int i = 0; i < essid_len; i++)
    rand_reg[i] = alfa[random(64)];

  int ssidLen = strlen(suffix);
  int rand_len = sizeof(rand_reg);
  int fullLen = ssidLen + rand_len;
  packet[37] = fullLen;

  // Insert generated string
  for (int i = 0; i < rand_len; i++)
    packet[38+i] = rand_reg[i];

  // Add identifier
  packet[38 + rand_len] = suffix[0];

  packet[50 + fullLen] = channel;

  uint8_t postSSID[13] = {0x01, 0x08, 0x82, 0x84, 0x8b, 0x96, 0x24, 0x30, 0x48, 0x6c, //supported rate
                      0x03, 0x01, 0x04 /*DSSS (Current Channel)*/ };

  // Add everything that goes after the SSID
  for(int i = 0; i < 12; i++) 
    packet[38 + fullLen + i] = postSSID[i];
  
  result = esp_wifi_80211_tx(WIFI_IF_AP, packet, sizeof(packet), false);
  result = esp_wifi_80211_tx(WIFI_IF_AP, packet, sizeof(packet), false);
  result = esp_wifi_80211_tx(WIFI_IF_AP, packet, sizeof(packet), false);
  result = esp_wifi_80211_tx(WIFI_IF_AP, packet, sizeof(packet), false);
  result = esp_wifi_80211_tx(WIFI_IF_AP, packet, sizeof(packet), false);
}

void loop() {
  // Flash the onboard LED on/off each cycle
  if (currentSSIDIndex == 0){
  p = 1 - p;
    if (p == 1) {
      digitalWrite(8, LOW); // Turn the LED on
    } else {
      digitalWrite(8, HIGH); // Turn the LED off
    }
  }

  // Broadcast SSIDs from the array (slow)
  currentSSIDIndex = (currentSSIDIndex + 1) % ssidCount;
  broadcastSSIDArray(ssids[currentSSIDIndex]);

  // Braodcast fixed length random SSIDs (very fast)
  //broadcastSSIDFixedRandom();

  // Broadcat random length, random SSIDs (medium)
  broadcastSSIDRandom();
  delay(3);
}
