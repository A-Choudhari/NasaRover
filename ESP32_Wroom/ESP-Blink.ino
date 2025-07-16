/*
  Read the digital output value of the vibration sensor.
  When a vibration is detected, the program will display the message 
  "Detected vibration..." on the serial monitor. Conversely, 
  if there is no vibration, the program will output "..." instead.

  Board: ESP32 Development Board
  Component: Vibration Sensor(SW-420)
*/
// Define TX and RX pins for UART (change if needed)
#define TXD2 17
#define RXD2 16
// Define the pin numbers for Vibration Sensor
const int sensorPin = 2;
HardwareSerial uart2(1);
char data[6];  // 5 bytes + null terminator
const char* START_MARKER = "IMG_START";
const char* END_MARKER = "IMG_END";
int startMatchIndex = 0;
bool receiving = false;
uint32_t imgLength = 0;
uint32_t receivedBytes = 0;
uint8_t* imgBuffer = nullptr;


void setup() {
  Serial.begin(115200);         // Start serial communication at 115200 baud rate
  uart2.begin(9600, SERIAL_8N1, RXD2, TXD2);
  delay(2000);
  pinMode(sensorPin, OUTPUT);  // Set the sensorPin as an input pin
  Serial.println("Receiver ready...");
}

#define PRINT_SER Serial.println
void loop() {
  // digitalWrite(sensorPin, HIGH);
  // delay(600);
  // digitalWrite(sensorPin, LOW);
  // delay(600);
  static String tempBuffer = ""; // To detect start marker
  while (uart2.available() > 0) {
  char c = uart2.read();
  Serial.println(c);

  // Phase 1: Detect IMG_START
  if (!receiving) {
    if (c == START_MARKER[startMatchIndex]) {
      PRINT_SER(c);
      startMatchIndex++;
      if (startMatchIndex == strlen(START_MARKER)) {
        Serial.println("[✓] Detected IMG_START");
        startMatchIndex = 0;
        receiving = true;
        receivedBytes = 0;
        imgLength = 0;

        // Wait for length bytes
        uint8_t lenBytes[4];
        while (uart2.available() < 4);  // Wait until 4 bytes arrive
        uart2.readBytes(lenBytes, 4);

        // Print raw bytes in hex
        Serial.print("[DEBUG] Raw Bytes: ");
        for (int i = 0; i < 4; i++) {
            Serial.print("0x");
            Serial.print(lenBytes[i], HEX);
            Serial.print(" ");
        }
        Serial.println();

        int received_value = *((int*)lenBytes);

        Serial.print("[i] Receiving image of size: ");
        Serial.println(received_value);

        if (imgBuffer) free(imgBuffer);
        imgBuffer = (uint8_t*)malloc(received_value);
        if (!imgBuffer) {
          Serial.println("Failed to allocate buffer");
          receiving = false;
        }
      }
    } else {
      // Reset match if mismatch
      startMatchIndex = 0;
    }
  }

  // Phase 2: Read image
  else if (receiving && receivedBytes < imgLength) {
    size_t toRead = min((size_t)uart2.available(), (size_t)(imgLength - receivedBytes));
    uart2.readBytes(imgBuffer + receivedBytes, toRead);
    receivedBytes += toRead;

    if (receivedBytes == imgLength) {
      Serial.println("[✓] Image fully received, checking end marker...");

      // Wait for end marker
      char endBuf[8] = {0};
      while (uart2.available() < 7);
      uart2.readBytes(endBuf, 7);
      endBuf[7] = '\0';

      if (strcmp(endBuf, "IMG_END") == 0) {
        Serial.println("[✓] Detected IMG_END — Success!");
      } else {
        Serial.println("[!] IMG_END not matched — possibly corrupted transfer.");
      }

      receiving = false;
      // Optional: use imgBuffer here
      free(imgBuffer);
      imgBuffer = nullptr;
    }
  }
}


  // if (uart2.available() >= 4) {
  //   // int bytesRead = uart2.readBytes(data, 4);
  //   uint8_t val = uart2.read();  // Direct raw byte
  //   // data[bytesRead] = '\0';  // Null-terminate the string
  //   // String processedData = String((char*)data);

  //   Serial.print("Received: ");
  //   Serial.println(val);
  // }
}

void stringToHex(String inputString) {
  String hexString = "";
  for (int i = 0; i < inputString.length(); i++) {
    char c = inputString.charAt(i);
    int decimalValue = int(c);
    String hexValue = String(decimalValue, HEX);
    hexString += hexValue;
  }
  Serial.println(hexString);
}