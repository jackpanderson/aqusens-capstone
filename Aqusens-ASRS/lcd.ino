void standbyLCD() {
  lcd.setCursor(0,0);
  lcd.print("CURR: ");
  lcd.print(getCurrentDateTime());
  lcd.setCursor(0, 1);
  lcd.print("NEXT: ");
  lcd.print(getNextSampleTime());
  lcd.setCursor(1, 2);
  lcd.print("SETTINGS");
  lcd.setCursor(1, 3);
  lcd.print("RUN SAMPLE");
  lcd.setCursor(13, 2);
  lcd.print("STANDBY");
  lcd.setCursor(16, 3);
  lcd.print("MODE");
  lcd.setCursor(0, cursorY);
  lcd.print("*");
}

void ensureSampleStartLCD() {
  lcd.setCursor(4, 0);
  lcd.print("START SAMPLE");
  lcd.setCursor(3, 1);
  lcd.print("ARE YOU SURE?");
  lcd.setCursor(1, 2);
  lcd.print("START SAMPLE");
  lcd.setCursor(1, 3);
  lcd.print("EXIT");

  lcd.setCursor(0, cursorY);
  lcd.print("*");

}

void settingsLCD(uint8_t page) {

  switch (page) {
    case 1:
      lcd.setCursor(1, 0);
      lcd.print("INTERVAL");
      lcd.setCursor(1, 1);
      lcd.print("START TIME");
      lcd.setCursor(1, 2);
      lcd.print("SET CLOCK");
      lcd.setCursor(0, 3);
      lcd.print("<EXIT");
      lcd.setCursor(15, 3);
      lcd.print("MORE>");
      break;
    
    case 2:
      lcd.setCursor(1, 0);
      lcd.print("SOAK TIME");
      lcd.setCursor(1, 1);
      lcd.print("FLUSH TIME");
      lcd.setCursor(1, 2);
      lcd.print("DRY TIME");
      lcd.setCursor(0, 3);
      lcd.print("<BACK");
      lcd.setCursor(15, 3);
      lcd.print("MORE>");
      break;
    
    case 3:
      lcd.setCursor(1, 0);
      lcd.print("ADD EVENT");
      lcd.setCursor(1, 1);
      lcd.print("VIEW EVENTS");
      lcd.setCursor(1, 2);
      lcd.print("FILTER STATUS");
      lcd.setCursor(0, 3);
      lcd.print("<BACK");
      lcd.setCursor(15, 3);
      lcd.print("MORE>");
      break;

    case 4:
      lcd.setCursor(1, 0);
      lcd.print("SET BRIGHTNESS");
      lcd.setCursor(1, 1);
      lcd.print("SET CONTRAST");
      lcd.setCursor(0, 3);
      lcd.print("<BACK");
      break;
    
  }

  lcd.setCursor(0, cursorY);
  lcd.print("*");

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
  lcd.print("SET TIME");
  lcd.setCursor(3, 2);
  lcd.print("MO DA YR HR MN");
  lcd.setCursor(3, 3);
  lcd.print("SEL TO CONFIRM");
}

void initSetIntervalLCD() {
  lcd.setCursor(0, 0);
  lcd.print("SET SAMPLE INTERVAL");
  lcd.setCursor(6, 2);
  lcd.print("DA HR MN");
  lcd.setCursor(3, 3);
  lcd.print("SEL TO CONFIRM");
}

void initSetSoakOrDryLCD() {
  if (state == SET_SOAK_TIME) {
    lcd.setCursor(3, 0);
    lcd.print("SET SOAK TIME");
  }
  else if (state == SET_DRY_TIME) {
    lcd.setCursor(4, 0);
    lcd.print("SET DRY TIME");
  }
  lcd.setCursor(7, 2);
  lcd.print("HR MN");
  lcd.setCursor(3, 3);
  lcd.print("SEL TO CONFIRM");
}

void initSetBrightnessOrConstrastLCD() {
  resetLcd();

  if (state == SET_BRIGHTNESS) {
    lcd.setCursor(3, 0);
    lcd.print("SET BRIGHTNESS");

    for (int i = 0; i < screenBrightness; i++) {
      lcd.setCursor(i, 1);
      lcd.print(char(255));
    }
  }

  else if (state == SET_CONTRAST) {
    lcd.setCursor(4, 0);
    lcd.print("SET CONTRAST");
    
    for (int i = 0; i < screenBrightness; i++) {
      lcd.setCursor(i, 1);
      lcd.print(char(255));
    }
  }

  lcd.setCursor(3, 3);
  lcd.print("SEL TO CONFIRM"); 
}

void updateSetClockLCD(uint8_t cursorPos, tmElements_t adjustedTime) {
  lcd.noBlink();
  lcd.setCursor(3, 1);
  char buff[15];

  sprintf(buff, "%02d-%02d-%02d %02d:%02d", adjustedTime.Month, adjustedTime.Day, adjustedTime.Year, adjustedTime.Hour, adjustedTime.Minute);
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

void updateSetIntervalLCD(uint8_t cursorPos, tmElements_t adjustedTime) {
  lcd.noBlink();
  lcd.setCursor(6, 1);
  char buff[9];

  sprintf(buff, "%02d %02d %02d", adjustedTime.Day, adjustedTime.Hour, adjustedTime.Minute);
  lcd.print(buff);

  switch (cursorPos) {
    case 0:
      lcd.setCursor(6, 1); //Day Tens Place
      break;
    
    case 1:
      lcd.setCursor(7, 1); //Day Ones Place
      break;
    
    case 2:
      lcd.setCursor(9, 1); //Hour Tens Place
      break;

    case 3:
      lcd.setCursor(10, 1); //Hour Ones Place
      break;

    case 4:
      lcd.setCursor(12, 1); //Min Tens Place
      break;

    case 5:
      lcd.setCursor(13, 1); //Min Ones Place
      break;
  }
  
  lcd.blink();
}

void updateSetSoakOrDryLCD(uint8_t cursorPos, tmElements_t adjustedTime) {
  lcd.noBlink();
  lcd.setCursor(7, 1);
  char buff[6];

  sprintf(buff, "%02d %02d", adjustedTime.Hour, adjustedTime.Minute);
  lcd.print(buff);

  switch (cursorPos) {
    case 0:
      lcd.setCursor(7, 1); //Hour Tens Place
      break;
    
    case 1:
      lcd.setCursor(8, 1); //Hour Ones Place
      break;
    
    case 2:
      lcd.setCursor(10, 1); //Min Tens Place
      break;

    case 3:
      lcd.setCursor(11, 1); //Min Ones Place
      break;
  }
  
  lcd.blink();
}

void updateBrightnessOrContrastLCD(bool increasingBar) {
  if (increasingBar) {
    screenBrightness++;
    lcd.setCursor(screenBrightness - 1, 1);
    lcd.print(char(255)); //prints black rect
  }
  else {
    lcd.setCursor(screenBrightness - 1, 1);
    screenBrightness--;
    lcd.print(char(254)); //prints blank rect
  }
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