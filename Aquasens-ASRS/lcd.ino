void standbyLCD(uint8_t x, uint8_t y) {

  lcd.setCursor(0,0);
  lcd.print("curr: ");
  lcd.print(getCurrentDateTime());
  lcd.setCursor(0, 1);
  lcd.print("next: ");
  lcd.print(getNextSampleTime());
  lcd.setCursor(1, 2);
  lcd.print("Run Sample");
  lcd.setCursor(1, 3);
  lcd.print("Settings");
  lcd.setCursor(0, 2);
  
  if (x == 0 && y == 2) { //Run Sample
    lcd.print("*");
  } else {
    lcd.print(" ");
  }

    if (x == 0 && y == 3) { //Settings
    lcd.print("*");
  } else {
    lcd.print(" ");
  }

  
  // lcd.setCursor(14, 0);
  // lcd.print("Manual");
  // lcd.setCursor(1,1);
  // lcd.print("Timing");
  // lcd.setCursor(11, 1);
  // lcd.print("Set Clock");

  // lcd.setCursor(0,0);
  // if (x == 0 && y == 0) {
  //   lcd.print("*");
  // } else {
  //   lcd.print(" ");
  // }
  
  // lcd.setCursor(0,1);
  // if (x == 0 && y == 1) {
  //   lcd.print("*");
  // } else {
  //   lcd.print(" ");
  // }

  // lcd.setCursor(13,0);
  // if (x == 1 && y == 0) {
  // lcd.print("*");
  // } else {
  //   lcd.print(" ");
  // }

  // lcd.setCursor(10,1);
  // if (x == 1 && y == 1) {
  //   lcd.print("*");
  // } else {
  //   lcd.print(" ");
  // }

}

void activeLCD(String nextDropTime) {
  lcd.setCursor(0,0);
  lcd.print("Next: ");
  lcd.print(nextDropTime);
  lcd.setCursor(0,1);
  lcd.print("< Back");
}

void intervalLCD(String timeType, int timeComponent) {

  lcd.setCursor(0,0);
  lcd.print(timeType);
  lcd.print(timeComponent);

  lcd.setCursor(0,1);
  lcd.print("Press SEL to set.");

}

void intervalMenuLCD() {
  lcd.setCursor(0,0);
  //lcd.print(interval);
  lcd.setCursor(0,1);
  lcd.print("< back | SEL to set.");
}

void clearTopLine() {
  lcd.setCursor(0,0);
  lcd.print("                    ");
}