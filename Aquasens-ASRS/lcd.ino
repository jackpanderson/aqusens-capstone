void standbyLCD() {
  lcd.setCursor(0,0);
  lcd.print("curr: ");
  lcd.print(getCurrentDateTime());
  lcd.setCursor(0, 1);
  lcd.print("next: ");
  lcd.print(getNextSampleTime());
  lcd.setCursor(1, 3);
  lcd.print("Run Sample");
  lcd.setCursor(1, 2);
  lcd.print("Settings");
  lcd.setCursor(0, cursorY);
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
      lcd.setCursor(15, 3);
      lcd.print("More>");
      break;
    
    case 2:
      lcd.setCursor(1, 0);
      lcd.print("Dry Time");
      lcd.setCursor(1, 1);
      lcd.print("Soak Time");
      lcd.setCursor(1, 2);
      lcd.print("Clock");
      lcd.setCursor(15, 3);
      lcd.print("More>");
      break;
    
    case 3:
      lcd.setCursor(1, 0);
      lcd.print("Filter Status");
      lcd.setCursor(1, 1);
      lcd.print("Brightness");
      break;
  }

  lcd.setCursor(0, cursorY);
  lcd.print("*");

  lcd.setCursor(0, 3);
  lcd.print("<Back");
}

void releaseLcd(String position) {
  lcd.setCursor(0,1);
  lcd.print("Releasing device...");
  lcd.setCursor(0,2);
  lcd.print("curr position:");
  lcd.setCursor(15,2);
  lcd.print(position);
}

void soakLcd(String time) {
  lcd.setCursor(0,1);
  lcd.print("Soaking...");
  lcd.setCursor(0,2);
  lcd.print(time);
  lcd.setCursor(6,2);
  lcd.print("remaining");
}

void recoverLcd(String position) {
  lcd.setCursor(0,1);
  lcd.print("Recovering device...");
  lcd.setCursor(0,2);
  lcd.print("curr position:");
  lcd.setCursor(15,2);
  lcd.print(position);
}

void sampleLcd() {
  lcd.setCursor(0,1);
  lcd.print("Sampling...");
}

void flushLcd() {
  lcd.setCursor(0,1);
  lcd.print("Flushing...");
}

void dryLcd(String time) {
  lcd.setCursor(0,1);
  lcd.print("Drying...");
  lcd.setCursor(0,2);
  lcd.print(time);
  lcd.setCursor(7,2);
  lcd.print("remaining");
}

void alarmLcd(String reason) {

}

void manualLcd() {

}

void initSetClockLcd() {
  lcd.setCursor(6, 0);
  lcd.print("Set Time");
  lcd.setCursor(3, 2);
  lcd.print("Mo Da Yr Hr Mn");
  lcd.setCursor(3, 3);
  lcd.print("SEL to confirm");
}

void updateSetClockLCD(uint8_t cursorPos, tmElements_t adjustedTime) {
  lcd.noBlink();
  lcd.setCursor(3, 1);
  char buff[15];

  uint16_t realYear = adjustedTime.Year + 1970; //Gets the calendar year.
  sprintf(buff, "%02d-%02d-%02d %02d:%02d", adjustedTime.Month, adjustedTime.Day, realYear - 2000, adjustedTime.Hour, adjustedTime.Minute);
  lcd.print(buff);

  switch (cursorPos) {
    case 0:
      lcd.setCursor(3, 1); //Month Tens Place
      break;
    
    case 1:
      lcd.setCursor(4, 1); //Month Ones Place
      break;
    
    case 2:
      lcd.setCursor(6, 1); //Day Tens Place
      break;

    case 3:
      lcd.setCursor(7, 1); //Day Ones Place
      break;

    case 4:
      lcd.setCursor(9, 1); //Year Tens Place
      break;

    case 5:
      lcd.setCursor(10, 1); //Year Ones Place
      break;

    case 6:
      lcd.setCursor(12, 1); //Hour Tens Place
      break;

    case 7:
      lcd.setCursor(13, 1); //Hour Ones Place
      break;

    case 8:
      lcd.setCursor(15, 1); //Min Tens Place
      break;
    
    case 9:
      lcd.setCursor(16, 1); //Min Ones Place
      break;
    default:
      break;
  }
  
  lcd.blink();
}

// void activeLCD(String nextDropTime) {
//   lcd.setCursor(0,0);
//   lcd.print("Next: ");
//   lcd.print(nextDropTime);
//   lcd.setCursor(0,1);
//   lcd.print("< Back");
// }

// void intervalLCD(String timeType, int timeComponent) {

//   lcd.setCursor(0,0);
//   lcd.print(timeType);
//   lcd.print(timeComponent);

//   lcd.setCursor(0,1);
//   lcd.print("Press SEL to set.");

// }

// void intervalMenuLCD() {
//   lcd.setCursor(0,0);
//   //lcd.print(interval);
//   lcd.setCursor(0,1);
//   lcd.print("< back | SEL to set.");
// }

// void clearTopLine() {
//   lcd.setCursor(0,0);
//   lcd.print("                    ");
// }