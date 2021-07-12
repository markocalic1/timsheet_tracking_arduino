#include <MFRC522.h> // for the RFID
#include <SPI.h> // for the RFID and SD card module

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

#include <WiFiClient.h>
#include <ArduinoJson.h>
#include <Wire.h>

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>



#define STASSID "NAVADOO"
#define STAPSK  "NAVADOMRATA23"


#define TRACK_TIME true
int lastMilisec = 0;

String uidString;

// Pins for LEDs and buzzer
const int redLED = 16; //D0
const int greenLED =10; //SD3
//// define pins for RFID
#define CS_RFID 2 //D3
#define RST_RFID 0 //D4

// Instance of the class for RFID
MFRC522 rfid(CS_RFID, RST_RFID);

const char *host = "test.nava.hr";
const char* sessionURL= "/web/session/authenticate";
const char* apiURL = "/api/attendance";

String session_id;

const int httpsPort = 443;  //HTTPS= 443 and HTTP = 80

//SHA1 finger print of certificate use web browser to view and copy
const char fingerprint[] PROGMEM = "66 6B CC 89 FF E9 6C 92 79 3D 2C DC BE 92 38 A3 E9 A9 F2 26";
//=======================================================================
//                    Power on setup
//=======================================================================


void setup() {
  initializeLEDs();
  initializeBuzzer();

   Wire.begin();
  Serial.begin(115200);
  while(!Serial);
   // Init SPI bus
  SPI.begin(); 
  initializeRFID();

  logMessage("");
  logMessage("");
  logMessage("");
  initializeLCD();
  WiFi.mode(WIFI_OFF);        //Prevents reconnection issue (taking too long to connect)
  delay(500);
  WiFi.mode(WIFI_STA);        //Only Station No AP, This line hides the viewing of ESP as wifi hotspot

  drawchar("Connectiing to Wifi");
  WiFi.begin(STASSID, STAPSK);     //Connect to your WiFi router
  logMessage("");

  Serial.print("Connecting");
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
     drawchar(".");
    blinkLed(greenLED,1);
    blinkLed(redLED,1);
  }

  //If connection successful show IP address in serial monitor
  logMessage("");
  Serial.print("Connected to ");
  logMessage(STASSID);
   drawchar(String("Connected to ") + String(STASSID));
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());  //IP address assigned to your ESP

  session_id=getSessionId();
}

void loop() {
  if (session_id.length() >10){
      digitalWrite(greenLED, HIGH);
           drawchar("READY");

    }
  //look for new cards
  if(rfid.PICC_IsNewCardPresent()) {
    digitalWrite(greenLED, LOW);
    readRFID();
    WiFiClientSecure httpsClient;    //Declare object of class WiFiClient
    drawchar("CHECKING YOUR CARD");
    logMessage(host);
  
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
      logMessage("Connection failed");
      digitalWrite(redLED, HIGH);
         drawchar("PROBLEM WITH CONNECTION TO INTERNET");

    }
    else {
      logMessage("Connected to web");
      digitalWrite(redLED, LOW);

    }

    Serial.print("UID:");
    logMessage(uidString);

    String body ="{\"jsonrpc\": \"2.0\",\"params\":{\"rfid\":\""+ uidString + "\"}}";  // a valid jsonObject
    String request = String("POST ") + apiURL + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "Content-Type: application/json"+ "\r\n" +
              "X-Openerp-Session-Id: "+ session_id +"\r\n" +
               "Content-Length: " + String(body.length()) + "\r\n\r\n" +
               body+ "\r\n";
    logMessage(request);
    httpsClient.print(request);
  
    logMessage("request sent");
                    
    while (httpsClient.connected()) {
      String line = httpsClient.readStringUntil('\n');
      if (line == "\r") {
        logMessage("headers received");
        break;
      }
    }
  
    logMessage("reply was:");
    logMessage("==========");
    String response;
    String employee;
    String action;
    String response_status = "nok";
    response = httpsClient.readStringUntil('}');//Read

    logMessage(response);
    if (response.startsWith("{\"jsonrpc\": \"2.0\"")) {
      DynamicJsonDocument doc(1024);
      deserializeJson(doc, response);
      
      response_status = String(doc["result"]["status"]);
      employee = String(doc["result"]["employee"]);
      action = String(doc["result"]["action"]);
      logMessage(response_status);
      logMessage(employee);
      logMessage(action); 
      logMessage("==========");
      logMessage("closing connection");
    }

    if(response_status == "ok"){
        logMessage("Result ok:Blink green 2 times ");

        beepSuccess();

        blinkLed(greenLED,3);
         drawchar(String("STATUS : OK \n Zaposlenik: ") + employee + " \nAkcija: " + action);
        delay(2000);
    }
    else{
        logMessage("Result nok:Blink red 3 times ");
        beepError();
        beepError();
        beepError();
              blinkLed(redLED,3);

    }

  }

 
  delay(10);
}

String getSessionId(){
   WiFiClientSecure httpsClient;    //Declare object of class WiFiClient

  
    logMessage(host);
  
    Serial.printf("Using fingerprint '%s'\n", fingerprint);
    httpsClient.setFingerprint(fingerprint);
    httpsClient.setTimeout(5000); // 5 Seconds
    delay(1000);
    
    Serial.print("HTTPS Connecting");
    int r=0; //retry counter
    while((!httpsClient.connect(host, httpsPort)) && (r < 30)){
        delay(100);
        Serial.print(".");
        r++;
    }
    if(r==30) {
      logMessage("Connection failed");
    }
    else {
      logMessage("Connected to web");
    }
    
    


    Serial.print("requesting URL: ");
    logMessage(host);

//    request for session_id
    const char *body ="{\"jsonrpc\": \"2.0\",\"params\":{\"db\":\"TIMETRACKER\",\"login\": \"admin\",\"password\": \"odoo\"}}";  // a valid jsonObject
    
    httpsClient.print(String("POST ") + sessionURL + " HTTP/1.1\r\n" +
                 "Host: " + host + "\r\n" +
                 "Content-Type: application/json"+ "\r\n" +
                 "Content-Length: " + strlen(body) + "\r\n\r\n" +
                 body + "\r\n" +
                 "\r\n\r\n");
  
    logMessage("request sent");
                    
    while (httpsClient.connected()) {
      String line = httpsClient.readStringUntil('\n');
      if (line == "\r") {
        logMessage("headers received");
        break;
      }
    }
  
    logMessage("reply was:");
    logMessage("==========");
    String response;
    response = httpsClient.readStringUntil('\n');//Read
    logMessage(response);

    if(response) {

    DynamicJsonDocument doc(2048);
    deserializeJson(doc, response);

    session_id = String(doc["result"]["session_id"]);
    logMessage("Session_id:");
    logMessage(session_id);
  
    beepSuccess();
    blinkLed(greenLED,3);


  delay(100);
   }
   else{
    logMessage("Session not parsed.");
    }       
    return session_id;
}



void logMessage(String message){
  int currMilisec = millis();
  int difference = currMilisec - lastMilisec;
  if (TRACK_TIME){
    message = message + " " + difference + " ms";
    lastMilisec = currMilisec;
    }
   Serial.println(message);
}
