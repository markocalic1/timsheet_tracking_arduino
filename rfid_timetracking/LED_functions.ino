


void initializeLEDs(){
  // Set LEDs and buzzer as outputs
  pinMode(redLED, OUTPUT);
  pinMode(greenLED, OUTPUT);
}


void blinkLed(int pin, int times){
    for (int i=0;i<=times;i++){ 
      digitalWrite(pin, HIGH);    // sets the LED on
      delay(100);
      digitalWrite(pin, LOW);    // sets the LED off
      delay(100);
    }
}
