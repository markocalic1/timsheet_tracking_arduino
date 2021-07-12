
////LCD pins
#define LCD_SCL  5 //D1
#define LCD_SDA  4 //D2

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels


#define OLED_RESET    -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);


#define LOGO_HEIGHT   48
#define LOGO_WIDTH    16


static const unsigned char PROGMEM logo_bmp[] =
{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x00, 0x00, 0xe1, 0x80, 0x00, 0xc0, 
  0x19, 0xfe, 0xf1, 0x80, 0x01, 0xc0, 0x19, 0xfe, 0xf1, 0x8f, 0x11, 0xde, 0x18, 0x70, 0xd9, 0x83, 
  0xb1, 0x83, 0x18, 0x70, 0xdd, 0x83, 0x99, 0x83, 0x18, 0x70, 0xcd, 0x8f, 0x9b, 0x0f, 0x18, 0x70, 
  0xc7, 0x99, 0x9b, 0x3b, 0x18, 0x70, 0xc7, 0xb1, 0x8e, 0x31, 0x18, 0x70, 0xc3, 0x99, 0x8e, 0x31, 
  0x18, 0x70, 0xc3, 0x9d, 0x86, 0x1d, 0x18, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00  };


void initializeLCD(){
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
   display.display();
  delay(1000); // Pause for 2 seconds
    // Clear the buffer
  display.clearDisplay();
//    testdrawbitmap();    // Draw a small bitmap image

}


void drawchar(String message) {
  display.clearDisplay();

  display.setTextSize(1.5);      // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE); // Draw white text
  display.setCursor(0, 30);     // Start at top-left corner
  display.cp437(true);         // Use full 256 char 'Code Page 437' font

//  // Not all the characters will fit on the display. This is normal.
//  // Library will draw what it can and the rest will be clipped.
//  for(int16_t i=0; i<256; i++) {
//    if(i == '\n') display.write(' ');
//    else          display.write(i);
//  }
  display.println(message);

  display.display();
  delay(500);
}

void testdrawbitmap(void) {
  display.clearDisplay();

  display.drawBitmap(
    (display.width()  - LOGO_WIDTH ) / 2,
    (display.height() - LOGO_HEIGHT) / 2,
    logo_bmp, LOGO_WIDTH, LOGO_HEIGHT, 1);
  display.display();
  delay(1000);
}
