

#include <MFRC522.h> // for the RFID
#include <SPI.h> // for the RFID and SD card module

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WiFiMulti.h>

#include <WiFiClient.h>
#include <ArduinoJson.h>

ESP8266WiFiMulti WiFiMulti;


#define STASSID "NAVADOO"
#define STAPSK  "NAVADOMRATA23"

//// define pins for RFID
#define CS_RFID 4
#define RST_RFID 5
// Instance of the class for RFID
MFRC522 rfid(CS_RFID, RST_RFID);

String uidString;

// Pins for LEDs and buzzer
//const int redLED = 6;
//const int greenLED = 7;
//const int buzzer = 5;



const char* sessionURL= "https://test.nava.hr/web/session/authenticate";
const char* apiURL = "http://test.nava.hr/api/attendance";

String session_id;

//Link to read data from https://jsonplaceholder.typicode.com/comments?postId=7
//Web/Server address to read/write from
const char *host = "test.nava.hr";

const int httpsPort = 443;  //HTTPS= 443 and HTTP = 80

//SHA1 finger print of certificate use web browser to view and copy
const char fingerprint[] PROGMEM = "66 6B CC 89 FF E9 6C 92 79 3D 2C DC BE 92 38 A3 E9 A9 F2 26";
//=======================================================================
//                    Power on setup
//=======================================================================


void setup() {
//
//  // Set LEDs and buzzer as outputs
//  pinMode(redLED, OUTPUT);
//  pinMode(greenLED, OUTPUT);
//  pinMode(buzzer, OUTPUT);
  Serial.begin(115200);
  while(!Serial);
    // Init SPI bus
  SPI.begin(); 
  // Init MFRC522 
  rfid.PCD_Init();

 Serial.println();
  Serial.println();
  Serial.println();

delay(1000);
  Serial.begin(115200);
  WiFi.mode(WIFI_OFF);        //Prevents reconnection issue (taking too long to connect)
  delay(1000);
  WiFi.mode(WIFI_STA);        //Only Station No AP, This line hides the viewing of ESP as wifi hotspot
  
  WiFi.begin(STASSID, STAPSK);     //Connect to your WiFi router
  Serial.println("");

  Serial.print("Connecting");
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  //If connection successful show IP address in serial monitor
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(STASSID);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());  //IP address assigned to your ESP

  session_id=getSessionId();
}

void loop() {
  // put your main code here, to run repeatedly:
  //look for new cards
  if(rfid.PICC_IsNewCardPresent()) {
    readRFID();
    WiFiClientSecure httpsClient;    //Declare object of class WiFiClient
  
    Serial.println(host);
  
    Serial.printf("Using fingerprint '%s'\n", fingerprint);
    httpsClient.setFingerprint(fingerprint);
    httpsClient.setTimeout(15000); // 15 Seconds
    delay(1000);
    
    Serial.print("HTTPS Connecting");
    int r=0; //retry counter
    while((!httpsClient.connect(host, httpsPort)) && (r < 30)){
        delay(100);
        Serial.print(".");
        r++;
    }
    if(r==30) {
      Serial.println("Connection failed");
    }
    else {
      Serial.println("Connected to web");
    }
    
    String getData, Link;
    
    //POST Data
    Link = "/web/session/authenticate";

    Serial.print("requesting URL: ");
    Serial.println(host);


    Serial.print("UID:");
    Serial.println(uidString);

    Link= "/api/attendance";
    String body ="{\"jsonrpc\": \"2.0\",\"params\":{\"rfid\":\""+ uidString + "\",\"time\": \"2020-02-02 20:23:23\"}}";  // a valid jsonObject
    String request = String("POST ") +Link + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "Content-Type: application/json"+ "\r\n" +
              "X-Openerp-Session-Id: "+ session_id +"\r\n" +
               "Content-Length: " + String(body.length()) + "\r\n\r\n" +
               body+ "\r\n";
    Serial.println(request);
    httpsClient.print(String("POST ") + Link + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "Content-Type: application/json"+ "\r\n" +
               "X-Openerp-Session-Id: "+ session_id +"\r\n" +
               "Content-Length: " + String(body.length()) + "\r\n\r\n" +
               body+ "\r\n" 
               );
  
    Serial.println("request sent");
                    
    while (httpsClient.connected()) {
      String line = httpsClient.readStringUntil('\n');
      if (line == "\r") {
        Serial.println("headers received");
        break;
      }
    }
  
    Serial.println("reply was:");
    Serial.println("==========");
    String line;
    while(httpsClient.available()){        
      line = httpsClient.readStringUntil('\n');  //Read Line by Line
      Serial.println(line); //Print response
    }
    Serial.println("==========");
    Serial.println("closing connection");
    

  }

 
  delay(10);
}

String getSessionId(){
   WiFiClientSecure httpsClient;    //Declare object of class WiFiClient
  
    Serial.println(host);
  
    Serial.printf("Using fingerprint '%s'\n", fingerprint);
    httpsClient.setFingerprint(fingerprint);
    httpsClient.setTimeout(15000); // 15 Seconds
    delay(1000);
    
    Serial.print("HTTPS Connecting");
    int r=0; //retry counter
    while((!httpsClient.connect(host, httpsPort)) && (r < 30)){
        delay(100);
        Serial.print(".");
        r++;
    }
    if(r==30) {
      Serial.println("Connection failed");
    }
    else {
      Serial.println("Connected to web");
    }
    
    String getData, Link;
    
    //POST Data
    Link = "/web/session/authenticate";

    Serial.print("requesting URL: ");
    Serial.println(host);

//    request for session_id
    const char *body ="{\"jsonrpc\": \"2.0\",\"params\":{\"db\":\"TIMETRACKER\",\"login\": \"admin\",\"password\": \"odoo\"}}";  // a valid jsonObject
    
    httpsClient.print(String("POST ") + Link + " HTTP/1.1\r\n" +
                 "Host: " + host + "\r\n" +
                 "Content-Type: application/json"+ "\r\n" +
                 "Content-Length: " + strlen(body) + "\r\n\r\n" +
                 body + "\r\n" +
                 "\r\n\r\n");
  
    Serial.println("request sent");
                    
    while (httpsClient.connected()) {
      String line = httpsClient.readStringUntil('\n');
      if (line == "\r") {
        Serial.println("headers received");
        break;
      }
    }
  
    Serial.println("reply was:");
    Serial.println("==========");
    String line;
//    while(httpsClient.available()){ 
//      line      
      line = httpsClient.readString();  //Read Line by Line
      DynamicJsonDocument doc(2048);
     deserializeJson(doc, line);
    
      String session_id = doc["result"]["session_id"];
      Serial.println(session_id);
//    }
  
  return session_id;
}


void readRFID() {
  rfid.PICC_ReadCardSerial();
  Serial.print("Tag UID: ");
  uidString = String(rfid.uid.uidByte[0]) + " " + String(rfid.uid.uidByte[1]) + " " + 
    String(rfid.uid.uidByte[2]) + " " + String(rfid.uid.uidByte[3]);
  Serial.println(uidString);
 
  // Sound the buzzer when a card is read
//  tone(buzzer, 2000); 
  delay(100);        
//  noTone(buzzer);
//  
  delay(100);
}
