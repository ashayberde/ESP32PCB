#include <LiquidCrystal_I2C.h>
#include <WiFi.h>
#include <WiFiManager.h>
#include <HTTPClient.h>
#include <SPI.h>
#include <ArduinoJson.h>
#include <MFRC522.h>

#include <ESP32Servo.h>
#include <SD.h>
Servo myServo;
const int servoPin = 26; // Replace with the actual GPIO pin number

#define buzzer 27
#define relayPin 14
#define SS_PIN 17
#define RST_PIN 4
int tag_length = 8, flag;
int current_index_val = 0;
int prev_index_val = 0;
const int chipSelect = 5;
const String str_temp = ",60B4AD21,AAE06018";//DAEC4B18,FA726218
String ashay;
byte arr[15];
String fin_arr;
String RFID;
String SerialRFID;
String one_at_a_time_str;
//const int relayPin = 14;

// Static WiFi removed. WiFiManager will handle WiFi setup.
// const char* ssid = "Berde";
// const char* password = "2416@ashay";

String response;
const char* host = "https://drab-erin-bunny-cuff.cyclic.app/api/";
const char* name ;
const char* approved;

// Length (with one extra character for the null terminator)
int payload_len;

// Prepare the character array (the buffer)
char payloadchar[80];
const char* json_doc = payloadchar;

//String endpoint="" ;
//String ABB= endpoint ;
//String ashay_web = host + ABB;
StaticJsonDocument<512> doc;

LiquidCrystal_I2C lcd(0x27, 16, 2); // I2C address 0x27, 16 column and 2 rows
MFRC522 rfid(SS_PIN, RST_PIN); // Instance of the class

MFRC522::MIFARE_Key key;

// Init array that will store new NUID
byte nuidPICC[4];


void setup() {
  myServo.attach(servoPin);
  lcd.init(); // initialize the lcd
  lcd.backlight();
  lcd.clear();                  // clear display
  lcd.setCursor(3, 0);          // move cursor to   (3, 0)
  lcd.print("RFID");        // print message at (3, 0)
  lcd.setCursor(0, 1);          // move cursor to   (0, 1)
  lcd.print("Parking Access"); // print message at (0, 1)
  delay(2000);                  // display the above for two seconds
  pinMode(relayPin, OUTPUT);
  pinMode(buzzer, OUTPUT);
  Serial.begin(9600);

  // WiFiManager instead of static WiFi credentials
  WiFiManager wm;

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("WiFi Manager");
  lcd.setCursor(0, 1);
  lcd.print("Connecting");

  /*
    If saved WiFi credentials exist, ESP32 will connect automatically.
    If not, ESP32 will create an access point:
    SSID: RFID_Parking_Setup
    Password: 12345678
  */
  bool res = wm.autoConnect("RFID_Parking_Setup", "12345678");

  if (!res) {
    Serial.println("Failed to connect to WiFi");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("WiFi Failed");
    lcd.setCursor(0, 1);
    lcd.print("Restarting");
    delay(3000);
    ESP.restart();
  }

  lcd.clear();
  lcd.print("Connected to WiFi");
  Serial.println("Connected to WiFi");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
  delay(2000);

  SPI.begin(); // Init SPI bus
  //
  //  if (!SD.begin(chipSelect)) {
  //    Serial.println("Initialization failed!");
  //    while (1);
  //  }
  //
  //  File file = SD.open("/RFID.txt");
  //  if (!file) {
  //    Serial.println("Failed to open file for reading");
  //    return;
  //  }
  //
  //  Serial.println("File Content:");
  //  if (file.available()) {
  //    str_temp = file.readStringUntil('\n');
  Serial.println(str_temp );
  //  }
  //  file.close();

  rfid.PCD_Init(); // Init MFRC522

  for (byte i = 0; i < 6; i++) {
    key.keyByte[i] = 0xFF;
  }

  //  Serial.println(F("This code scan the MIFARE Classsic NUID."));
  //  Serial.print(F("Using the following key:"));
  //printHex(key.keyByte, MFRC522::MF_KEY_SIZE);
}

void printHex(byte * buffer, byte bufferSize) {
  char temp;
  byte hexx;
  for (byte i = 0; i < bufferSize; i++) {
    hexx = buffer[i];
    if (hexx < 0x0A)
    {
      temp = hexx + 0x30;
      //        Serial.write(temp);
    }
    else if ((hexx > 0x09) && (hexx < 0x10))
    {
      temp = hexx + 0x37;
      //        Serial.write(temp);
    }
    fin_arr += String(buffer[i], HEX);
  }
  fin_arr.toUpperCase();
  //Serial.println(fin_arr);
  fin_arr.trim();
  Serial.println(fin_arr);
  RFID = fin_arr;
  fin_arr = "";
}

void loop() {
  // Reset the loop if no new card present on the sensor/reader. This saves the entire process when idle.
  if ( ! rfid.PICC_IsNewCardPresent())
    return;

  // Verify if the NUID has been readed
  if ( ! rfid.PICC_ReadCardSerial())
    return;

  //  Serial.print(F("PICC type: "));
  MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);
  //  Serial.println(rfid.PICC_GetTypeName(piccType));

  // Check is the PICC of Classic MIFARE type
  if (piccType != MFRC522::PICC_TYPE_MIFARE_MINI &&
      piccType != MFRC522::PICC_TYPE_MIFARE_1K &&
      piccType != MFRC522::PICC_TYPE_MIFARE_4K) {
    //    Serial.println(F("Your tag is not of type MIFARE Classic."));
    return;
  }

  //  if (rfid.uid.uidByte[0] != nuidPICC[0] ||
  //      rfid.uid.uidByte[1] != nuidPICC[1] ||
  //      rfid.uid.uidByte[2] != nuidPICC[2] ||
  //      rfid.uid.uidByte[3] != nuidPICC[3] ) {
  //    Serial.println(F("A new card has been detected."));
  //
  //    // Store NUID into nuidPICC array
  for (byte i = 0; i < 1; i++) {
    nuidPICC[i] = rfid.uid.uidByte[i];
    //    }

    //    Serial.println(F("The NUID tag is:"));
    //    Serial.print(F("In hex: "));
    printHex(rfid.uid.uidByte, rfid.uid.size);
    //    Serial.println();
  }
  // Halt PICC
  rfid.PICC_HaltA();

  // Stop encryption on PCD
  rfid.PCD_StopCrypto1();
  if ( ! rfid.PICC_ReadCardSerial())
  {
    String abcd = RFID;
  //  Serial.println(abcd);
    //    String tag_id_str = Serial.readStringUntil('\n');
    //    tag_id_str.trim();
    //Serial.print("Tag:");
    Serial.println(abcd);
    makeApiRequest();
   // API_1();
    //    int str_temp_len =  str_temp.length();
    //    int loop_limit = str_temp_len / tag_length;
    //    Serial.println("String Length of Reference tags: ");
    //    Serial.println(str_temp_len);

    //    for (int i = 0; i < loop_limit; i++)
    //    {
    //      current_index_val = str_temp.indexOf(',', prev_index_val);
    //      //      Serial.print(prev_index_val);
    //      //      Serial.print("\t");
    //      //      Serial.println(current_index_val);
    //      one_at_a_time_str = str_temp.substring(prev_index_val, current_index_val);
    //      one_at_a_time_str.trim();
    //      Serial.println(one_at_a_time_str);
    //      Serial.println(".");
    //      if (abcd.compareTo(one_at_a_time_str) == 0)
    //      {
    //        Serial.println("Success SD Card accessed Directly");
    //        flag = 1;
    //        delay(5000);
    //        break;
    //      }
    //      prev_index_val = current_index_val + 1;
    //      //      Serial.println("Exiting");
    //      //      Serial.println(str_temp.substring(prev_index_val, current_index_val));
    //    }
    //    if (flag == 1)
    //    {
    //      Serial.print("Access Granted for: ");
    //      Serial.println(RFID);
    //      lcd.clear();                  // clear display
    //      lcd.setCursor(0, 0);          // move cursor to   (3, 0)
    //      lcd.print("Access Granted for: ");        // print message at (0, 0)
    //      lcd.setCursor(0, 1);          // move cursor to   (0, 1)
    //      lcd.print(RFID); // print message at (0, 1)
    //      delay(2000);
    //      lcd.clear();                  // clear display
    //      flag = 0;

    //    }
    //    else
    //    {
    //      Serial.println("Checking API");
    //      Serial.println("Not Found in SD Trying API");
    //      delay(1000);
    //      API_1();
    //    }
  }
}

void API_1()
{

  HTTPClient http;

  //    Serial.print("[HTTP] begin...\n");
  // configure traged server and url
  //http.begin("https://www.howsmyssl.com/a/check", ca); //HTTPS
 // Serial.print(host + RFID); // host + RFID
  //    Serial.print(endpoint);
  http.begin(host + RFID); //HTTP
  //    Serial.print("[HTTP] GET...\n");                              // start connection and send HTTP header
  int httpCode = http.GET();
  // httpCode will be negative on error
  if (httpCode > 0) {                                         // HTTP header has been send and Server response header has been handled
    //                Serial.printf("[HTTP] GET... code: %d\n", httpCode);

    // file found at server
    if (httpCode == HTTP_CODE_OK) {
      String payload = http.getString();
      Serial.println(payload);
      payload_len =  payload.length() + 1;
      //  Serial.print(payload_len);
      payload.toCharArray(payloadchar, payload_len);

    //  Serial.print(payloadchar);
      delay(100);
      Deserial();
     // Serial.println("Access Granted");
      lcd.clear();                   // clear display
      lcd.setCursor(0, 0);          // move cursor to   (3, 0)
      lcd.print("Access Granted");        // print message at (3, 0)
      ServoLogic();
      lcd.setCursor(0, 1);          // move cursor to   (0, 1)
      lcd.print(name); // print message at (0, 1)
      digitalWrite(relayPin, HIGH);
      delay(2000);
      digitalWrite(relayPin, LOW);
      delay(5000);                  // display the above for two seconds
      lcd.clear();                  // clear display
    }
  }
  else {
    Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
    lcd.clear();                  // clear display
    lcd.setCursor(3, 0);          // move cursor to   (3, 0)
    lcd.print(RFID);        // print message at (3, 0)
    lcd.setCursor(0, 1);          // move cursor to   (0, 1)
    lcd.print("Parking Access Rejected"); // print message at (0, 1)

    delay(2000);                  // display the above for two seconds
    lcd.clear();                  // clear display
  }

  http.end();
  RFID = "";
}

void ServoLogic()
{
  myServo.write(0);      // Move the servo to 0 degrees
  delay(1000);           // Wait for 1 second
  myServo.write(100);    // Move the servo to 180 degrees
  delay(1000);           // Wait for 1 second
}

void makeApiRequest() {
  // Make a GET request to the API

  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi disconnected. Reconnecting...");
    WiFi.reconnect();
    delay(3000);

    if (WiFi.status() != WL_CONNECTED) {
      Serial.println("WiFi reconnect failed");
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("WiFi Error");
      lcd.setCursor(0, 1);
      lcd.print("Try Again");
      delay(2000);
      lcd.clear();
      return;
    }
  }

  HTTPClient http;
  String url = "https://drab-erin-bunny-cuff.cyclic.app/api/";
  String AshayP=url+RFID;
  http.begin(AshayP);

  // Check the HTTP status code
int statusCode = http.GET();
  Serial.print("HTTP Status Code: ");
  Serial.println(statusCode);

  if (statusCode == HTTP_CODE_OK) {
    // Data found, print the response
     response = http.getString();
    
    //Serial.print("Response: ");
    //Serial.println(response);
    Deserial();
     lcd.clear();                   // clear display
      lcd.setCursor(0, 0);          // move cursor to   (3, 0)
      lcd.print("Access Granted");        // print message at (3, 0)
      ServoLogic();
      lcd.setCursor(0, 1);          // move cursor to   (0, 1)
      lcd.print(name); // print message at (0, 1)
      digitalWrite(relayPin, HIGH);
     digitalWrite(buzzer,HIGH);
      delay(2000);
      digitalWrite(buzzer, LOW);
      digitalWrite(relayPin, LOW);
      delay(2000);                  // display the above for two seconds
      lcd.clear();                  // clear display
  } else if (statusCode == HTTP_CODE_NOT_FOUND) {
    // Data not found, print "Access Rejected"
    Serial.println("Access Rejected");
  } else {
    // Other error occurred
    Serial.println("Error occurred during the API request");
  }

  http.end();
}


void Deserial()
{
//  Serial.print(response);

  // Allocate a buffer to store the JSON document

  // Deserialize the JSON document into the buffer
  DeserializationError error = deserializeJson(doc, response);

  // Check if there was an error deserializing the JSON
  if (error) {
    Serial.print("deserializeJson() failed: ");
    Serial.println(error.c_str());
    return;
  }

  // Access the data in the JSON document
  JsonObject obj = doc[0];
  name = obj["name"];
  approved = obj["Approved"];

  // Print the data to the serial monitor
//  Serial.println(name);
//  Serial.println(approved);
}
