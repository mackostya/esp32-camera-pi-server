/*
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com/esp32-cam-post-image-photo-server/
  
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files.
  
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.

  Modified by Kostiantyn Lavronenko
  Modified to reset on any error conditions (capture or send).
*/

#include <Arduino.h>
#include <WiFi.h>
#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"
#include "esp_camera.h"

// Update these to match your network and server settings
const char* ssid = "SSID";
const char* password = "PASSWORD";

String serverName = "IP";   // REPLACE WITH YOUR Raspberry Pi IP ADDRESS
//String serverName = "example.com";   // OR REPLACE WITH YOUR DOMAIN NAME

String serverPath = "/upload.php";     // The default serverPath should be upload.php
const int serverPort = 80;

WiFiClient client;

// CAMERA_MODEL_AI_THINKER pins
#define PWDN_GPIO_NUM     32
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM      0
#define SIOD_GPIO_NUM     26
#define SIOC_GPIO_NUM     27
#define Y9_GPIO_NUM       35
#define Y8_GPIO_NUM       34
#define Y7_GPIO_NUM       39
#define Y6_GPIO_NUM       36
#define Y5_GPIO_NUM       21
#define Y4_GPIO_NUM       19
#define Y3_GPIO_NUM       18
#define Y2_GPIO_NUM        5
#define VSYNC_GPIO_NUM    25
#define HREF_GPIO_NUM     23
#define PCLK_GPIO_NUM     22

const int timerInterval = 60*60000;   // time (ms) between each HTTP POST
unsigned long previousMillis = 0;

void setup() {
  // Disable brownout detector
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0); 
  Serial.begin(115200);

  // Attempt Wi-Fi connection
  WiFi.mode(WIFI_STA);
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);  

  // If you want to force a reset when Wi-Fi fails to connect after a while:
  unsigned long startAttemptTime = millis();
  const unsigned long wifiTimeout = 60000; // e.g. 20s

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
    if (millis() - startAttemptTime >= wifiTimeout) {
      Serial.println("\nWi-Fi connection timed out. Restarting...");
      delay(1000);
      ESP.restart();
    }
  }

  Serial.println();
  Serial.print("ESP32-CAM IP Address: ");
  Serial.println(WiFi.localIP());

  // Camera config
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer   = LEDC_TIMER_0;
  config.pin_d0       = Y2_GPIO_NUM;
  config.pin_d1       = Y3_GPIO_NUM;
  config.pin_d2       = Y4_GPIO_NUM;
  config.pin_d3       = Y5_GPIO_NUM;
  config.pin_d4       = Y6_GPIO_NUM;
  config.pin_d5       = Y7_GPIO_NUM;
  config.pin_d6       = Y8_GPIO_NUM;
  config.pin_d7       = Y9_GPIO_NUM;
  config.pin_xclk     = XCLK_GPIO_NUM;
  config.pin_pclk     = PCLK_GPIO_NUM;
  config.pin_vsync    = VSYNC_GPIO_NUM;
  config.pin_href     = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn     = PWDN_GPIO_NUM;
  config.pin_reset    = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;

  // init with higher specs when PSRAM is available
  if(psramFound()){
    config.frame_size = FRAMESIZE_SVGA;   // 800x600
    config.jpeg_quality = 10;            // 0-63 (lower = higher quality)
    config.fb_count = 2; 
  } else {
    config.frame_size = FRAMESIZE_CIF;   // 352x288
    config.jpeg_quality = 12;
    config.fb_count = 1;
  }
  
  // Camera init
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x. Restarting...\n", err);
    delay(1000);
    ESP.restart();
  }

  // First photo as soon as we start
  sendPhoto(); 
}

void loop() {
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= timerInterval) {
    sendPhoto();
    previousMillis = currentMillis;
  }
}

String sendPhoto() {
  String getAll;
  String getBody;

  // Attempt to capture a frame
  camera_fb_t * fb = esp_camera_fb_get();
  if(!fb) {
    Serial.println("Camera capture failed. Restarting...");
    delay(1000);
    ESP.restart();
  }
  
  Serial.println("Connecting to server: " + serverName);

  // Attempt to connect to the server
  if (client.connect(serverName.c_str(), serverPort)) {
    Serial.println("Connection successful!");
    
    // Prepare headers
    String head = "--RandomNerdTutorials\r\n"
                  "Content-Disposition: form-data; name=\"imageFile\"; filename=\"esp32-cam.jpg\"\r\n"
                  "Content-Type: image/jpeg\r\n\r\n";
    String tail = "\r\n--RandomNerdTutorials--\r\n";

    // Calculate content length
    uint32_t imageLen = fb->len;
    uint32_t extraLen = head.length() + tail.length();
    uint32_t totalLen = imageLen + extraLen;
  
    // Send POST header
    client.println("POST " + serverPath + " HTTP/1.1");
    client.println("Host: " + serverName);
    client.println("Content-Length: " + String(totalLen));
    client.println("Content-Type: multipart/form-data; boundary=RandomNerdTutorials");
    client.println();
    client.print(head);
  
    // Send image in small chunks
    uint8_t *fbBuf = fb->buf;
    size_t fbLen = fb->len;
    for (size_t n = 0; n < fbLen; n += 1024) {
      if (n + 1024 < fbLen) {
        client.write(fbBuf, 1024);
        fbBuf += 1024;
      }
      else if (fbLen % 1024 > 0) {
        size_t remainder = fbLen % 1024;
        client.write(fbBuf, remainder);
      }
    }
    client.print(tail);

    // Return the frame buffer back to the driver for reuse
    esp_camera_fb_return(fb);
    
    // Wait for server response
    const int timeoutMs = 10000;
    unsigned long startTimer = millis();
    bool state = false;
    
    while ((millis() - startTimer) < timeoutMs) {
      while (client.available()) {
        char c = client.read();
        if (c == '\n') {
          if (getAll.length() == 0) {
            // Start reading the body
            state = true;
          }
          getAll = "";
        }
        else if (c != '\r') {
          getAll += String(c);
        }
        if (state == true) {
          getBody += String(c);
        }
        // Reset the timeout timer if we’re still getting data
        startTimer = millis();
      }
      if (getBody.length() > 0) {
        // We got something from the server
        break;
      }
      delay(100);
    }
    client.stop();
    Serial.println("Server response:");
    Serial.println(getBody);

    // Optional: if you want to force a reboot if no server response arrived
    if (getBody.length() == 0) {
      Serial.println("No response from server (timeout). Restarting...");
      delay(1000);
      ESP.restart();
    }
    
  } else {
    // If the connection failed
    getBody = "Connection to " + serverName + " failed. Restarting...";
    Serial.println(getBody);
    delay(1000);
    ESP.restart();
  }
  return getBody;
}
