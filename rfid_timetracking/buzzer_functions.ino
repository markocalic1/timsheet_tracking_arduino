const int buzzer = 15; //D8

void initializeBuzzer(){
  // Set  buzzer as outputs
  pinMode(buzzer, OUTPUT);
}



void beepError(){
  // Sound the buzzer 
  tone(buzzer, 30, 5000);
  delay(100);
  noTone(buzzer);   
}
void beepSuccess(){
  // Sound the buzzer 
  tone(buzzer, 800, 1000);
  delay(100);
  noTone(buzzer);
  delay(100);   
}
