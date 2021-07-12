


void initializeRFID(){
    // Init MFRC522 
  rfid.PCD_Init();

}


void readRFID() {
  rfid.PICC_ReadCardSerial();
  Serial.print("Tag UID: ");
  uidString = String(rfid.uid.uidByte[0]) + " " + String(rfid.uid.uidByte[1]) + " " + 
  String(rfid.uid.uidByte[2]) + " " + String(rfid.uid.uidByte[3]);
  logMessage(uidString);
 
  beepSuccess();
}
