void standbyLCD() {
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
  lcd.setCursor(cursorX, cursorY);
  lcd.print("*");
}

void settingsLCD(uint8_t page) {

  switch (page) {
    case 1:
      lcd.setCursor(1, 0);
      lcd.print("Interval");
      lcd.setCursor(1, 1);
      lcd.print("Add Events");
      lcd.setCursor(1, 2);
      lcd.print("View Events");
      break;
    
    case 2:
      lcd.setCursor(1, 0);
      lcd.print("Dry Time");
      lcd.setCursor(1, 1);
      lcd.print("Soak Time");
      lcd.setCursor(1, 2);
      lcd.print("Clock");
      break;
    
    case 3:
      lcd.setCursor(1, 0);
      lcd.print("Filter Status");
      lcd.setCursor(1, 1);
      lcd.print("Brightness");
      break;
  }

  lcd.setCursor(cursorX, cursorY);
  lcd.print("*");

  lcd.setCursor(0, 3);
  lcd.print("<Back");

  lcd.setCursor(15, 3);
  lcd.print("More>");
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