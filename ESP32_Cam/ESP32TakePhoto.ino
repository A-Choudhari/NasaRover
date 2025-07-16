#define CAMERA_MODEL_AI_THINKER

#include "esp_camera.h"
#include "Arduino.h"
#include "FS.h"                // SD Card ESP32
#include "SD_MMC.h"            // SD Card ESP32
#include "soc/soc.h"           // Disable brownour problems
#include "soc/rtc_cntl_reg.h"  // Disable brownour problems
#include "driver/rtc_io.h"
#include <EEPROM.h>            // read and write from flash memory
#include <pins.h>
// define the number of bytes you want to access
#define EEPROM_SIZE 1
// Define TX and RX pins for UART (change if needed)
#define TXD0 1
#define RXD0 3

// Pin definition for CAMERA_MODEL_AI_THINKER
uint8_t counter = 0;
int pictureNumber = 0;
HardwareSerial mySerial(0);
void setup() {
  // WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0); //disable brownout detector
  mySerial.begin(9600, SERIAL_8N1, RXD0, TXD0);  // UART setup''
  //Serial.setDebugOutput(true);
  delay(2000);
  
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sccb_sda = SIOD_GPIO_NUM;
  config.pin_sccb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG; 
  
  if(psramFound()){
    config.frame_size = FRAMESIZE_QVGA; // FRAMESIZE_ + QVGA|CIF|VGA|SVGA|XGA|SXGA|UXGA
    config.jpeg_quality = 10;
    config.fb_count = 2;
    // Serial.print("ram was found");
  } else {
    config.frame_size = FRAMESIZE_QVGA;
    config.jpeg_quality = 12;
    config.fb_count = 1;
    // Serial.print("ram was not found");
  }


  // // Initialize the camera
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
  //   Serial.printf("Camera init failed with error 0x%x\n", err);
    return;
  }
  // Serial.print("hi 2");


  // Serial.println("-----BEGIN JPG IMAGE-----");
  // // Serial.write(fb->buf, fb->len);
  // String hexString = "";

  // for (int i = 0; i < fb->len; i++) {
  //   char hexBuffer[3];
  //   sprintf(hexBuffer, "%02X", fb->buf[i]);
  //   hexString += hexBuffer;
  // }
  // Serial.print(hexString);
  // Serial.println("-----END JPG IMAGE-----");
  // Serial.printf("JPEG image size: %d bytes\n", fb->len);

  // esp_camera_fb_return(fb);

  // delay(1000);
  // Serial.println("Done. Entering sleep.");
  // esp_deep_sleep_start(); // Optional: disable if you want continuous capture
}

void loop() {
  camera_fb_t *fb = esp_camera_fb_get();
  if (!fb) {
  //   Serial.println("Camera capture failed");
    return;
  }
   // Step 1: Send start marker
  mySerial.write((uint8_t *)"IMG_START", 9);  // 9 bytes including \0
  // mySerial.println("IMG_START");
  // mySerial.println(fb->len);
  // Step 2: Send the length of the image as 4 bytes (MSB first)
  uint32_t len = fb->len;
  mySerial.write((byte *) &len, 4);
  // uint8_t* ptr = (uint8_t*)&len;
  // for (size_t i = 0; i < sizeof(int); i++) {
  //     mySerial.write(ptr[i]);  // replace with your UART write function
  // }

  // const size_t CHUNK_SIZE = 4;
  // for (size_t i = 0; i < fb->len; i += CHUNK_SIZE) {
  //   size_t bytesToSend = min(CHUNK_SIZE, fb->len - i);
  //   mySerial.write(fb->buf + i, bytesToSend);
  //   delay(5);  // Give receiver time to breathe
  // }


  // // Step 4: Optional end marker
  // mySerial.write((uint8_t *)"IMG_END", 7);  // 7 bytes including \0

  esp_camera_fb_return(fb);
  delay(100000);  // Send 1 frame per 100 seconds
}