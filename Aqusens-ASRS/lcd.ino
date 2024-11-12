void standbyLCD() {
  lcd2.setCursor(0,0);
  lcd2.print("CURR: ");
  lcd2.print(getCurrentDateTime());
  lcd2.setCursor(0, 1);
  lcd2.print("NEXT: ");
  lcd2.print(getNextSampleTime());
  lcd2.setCursor(1, 2);
  lcd2.print("SETTINGS");
  lcd2.setCursor(1, 3);
  lcd2.print("RUN SAMPLE");
  lcd2.setCursor(13, 2);
  lcd2.print("STANDBY");
  lcd2.setCursor(16, 3);
  lcd2.print("MODE");
  lcd2.setCursor(0, cursorY);
  lcd2.print("*");
}

void ensureSampleStartLCD() {
  lcd2.setCursor(4, 0);
  lcd2.print("RUN SAMPLE");
  lcd2.setCursor(3, 1);
  lcd2.print("ARE YOU SURE?");
  lcd2.setCursor(1, 2);
  lcd2.print("RUN SAMPLE");
  lcd2.setCursor(1, 3);
  lcd2.print("EXIT");

  lcd2.setCursor(0, cursorY);
  lcd2.print("*");

}

void settingsLCD(uint8_t page) {

  switch (page) {
    case 1:
      lcd2.setCursor(1, 0);
      lcd2.print("INTERVAL");
      lcd2.setCursor(1, 1);
      lcd2.print("START TIME");
      lcd2.setCursor(1, 2);
      lcd2.print("SET CLOCK");
      lcd2.setCursor(0, 3);
      lcd2.print("<EXIT");
      lcd2.setCursor(15, 3);
      lcd2.print("MORE>");
      break;
    
    case 2:
      lcd2.setCursor(1, 0);
      lcd2.print("SOAK TIME");
      lcd2.setCursor(1, 1);
      lcd2.print("FLUSH TIME");
      lcd2.setCursor(1, 2);
      lcd2.print("DRY TIME");
      lcd2.setCursor(0, 3);
      lcd2.print("<BACK");
      lcd2.setCursor(15, 3);
      lcd2.print("MORE>");
      break;
    
    case 3:
      lcd2.setCursor(1, 0);
      lcd2.print("ADD EVENT");
      lcd2.setCursor(1, 1);
      lcd2.print("VIEW EVENTS");
      lcd2.setCursor(1, 2);
      lcd2.print("FILTER STATUS");
      lcd2.setCursor(0, 3);
      lcd2.print("<BACK");
      lcd2.setCursor(15, 3);
      lcd2.print("MORE>");
      break;

    case 4:
      lcd2.setCursor(1, 0);
      lcd2.print("SET BRIGHTNESS");
      lcd2.setCursor(1, 1);
      lcd2.print("SET CONTRAST");
      lcd2.setCursor(0, 3);
      lcd2.print("<BACK");
      break;
    
  }

  lcd2.setCursor(0, cursorY);
  lcd2.print("*");

}

void releaseLcd(String position) {
  lcd2.setCursor(0,1);
  lcd2.print("RELEASING DEVICE...");
  lcd2.setCursor(0,2);
  lcd2.print("CURR POSITION:");
  lcd2.setCursor(15,2);
  lcd2.print(position);
}

void soakLcd(String time) {
  lcd2.setCursor(0,1);
  lcd2.print("SOAKING...");
  lcd2.setCursor(0,2);
  lcd2.print(time);
  lcd2.setCursor(6,2);
  lcd2.print("REMAINING");
}

void recoverLcd(String position) {
  lcd2.setCursor(0,1);
  lcd2.print("RECOVERING DEVICE...");
  lcd2.setCursor(0,2);
  lcd2.print("CURR POSITION:");
  lcd2.setCursor(15,2);
  lcd2.print(position);
}

void sampleLcd() {
  lcd2.setCursor(0,1);
  lcd2.print("SAMPLING...");
}

void flushLcd() {
  lcd2.setCursor(0,1);
  lcd2.print("FLUSHING...");
}

void dryLcd(String time) {
  lcd2.setCursor(0,1);
  lcd2.print("DRYING...");
  lcd2.setCursor(0,2);
  lcd2.print(time);
  lcd2.setCursor(7,2);
  lcd2.print("REMAINING");
}

void alarmLcd(String reason) {

}

void manualLcd() {

}

void initSetClockLcd() {
  lcd2.setCursor(6, 0);
  lcd2.print("SET TIME");
  lcd2.setCursor(3, 2);
  lcd2.print("MO DA YR HR MN");
  lcd2.setCursor(3, 3);
  lcd2.print("SEL TO CONFIRM");
}

void initSetIntervalLCD() {
  lcd2.setCursor(0, 0);
  lcd2.print("SET SAMPLE INTERVAL");
  lcd2.setCursor(6, 2);
  lcd2.print("DA HR MN");
  lcd2.setCursor(3, 3);
  lcd2.print("SEL TO CONFIRM");
}

void initSetSoakOrDryLCD() {
  if (state == SET_SOAK_TIME) {
    lcd2.setCursor(3, 0);
    lcd2.print("SET SOAK TIME");
  }
  else if (state == SET_DRY_TIME) {
    lcd2.setCursor(4, 0);
    lcd2.print("SET DRY TIME");
  }
  lcd2.setCursor(7, 2);
  lcd2.print("HR MN");
  lcd2.setCursor(3, 3);
  lcd2.print("SEL TO CONFIRM");
}

void initSetBrightnessOrConstrastLCD() {
  resetLcd();

  if (state == SET_BRIGHTNESS) {
    lcd2.setCursor(3, 0);
    lcd2.print("SET BRIGHTNESS");

    for (int i = 0; i < screenBrightness; i++) {
      lcd2.setCursor(i, 1);
      lcd2.print(char(255));
    }
  }

  else if (state == SET_CONTRAST) {
    lcd2.setCursor(4, 0);
    lcd2.print("SET CONTRAST");
    
    for (int i = 0; i < screenBrightness; i++) {
      lcd2.setCursor(i, 1);
      lcd2.print(char(255));
    }
  }

  lcd2.setCursor(3, 3);
  lcd2.print("SEL TO CONFIRM"); 
}

void updateSetClockLCD(uint8_t cursorPos, tmElements_t adjustedTime) {
  lcd2.noBlink();
  lcd2.setCursor(3, 1);
  char buff[15];

  sprintf(buff, "%02d-%02d-%02d %02d:%02d", adjustedTime.Month, adjustedTime.Day, adjustedTime.Year, adjustedTime.Hour, adjustedTime.Minute);
  lcd2.print(buff);

  switch (cursorPos) {
    case 0:
      lcd2.setCursor(3, 1); //Month Tens Place
      break;
    
    case 1:
      lcd2.setCursor(4, 1); //Month Ones Place
      break;
    
    case 2:
      lcd2.setCursor(6, 1); //Day Tens Place
      break;

    case 3:
      lcd2.setCursor(7, 1); //Day Ones Place
      break;

    case 4:
      lcd2.setCursor(9, 1); //Year Tens Place
      break;

    case 5:
      lcd2.setCursor(10, 1); //Year Ones Place
      break;

    case 6:
      lcd2.setCursor(12, 1); //Hour Tens Place
      break;

    case 7:
      lcd2.setCursor(13, 1); //Hour Ones Place
      break;

    case 8:
      lcd2.setCursor(15, 1); //Min Tens Place
      break;
    
    case 9:
      lcd2.setCursor(16, 1); //Min Ones Place
      break;
    default:
      break;
  }
  
  lcd2.blink();
}

void updateSetIntervalLCD(uint8_t cursorPos, tmElements_t adjustedTime) {
  lcd2.noBlink();
  lcd2.setCursor(6, 1);
  char buff[9];

  sprintf(buff, "%02d %02d %02d", adjustedTime.Day, adjustedTime.Hour, adjustedTime.Minute);
  lcd2.print(buff);

  switch (cursorPos) {
    case 0:
      lcd2.setCursor(6, 1); //Day Tens Place
      break;
    
    case 1:
      lcd2.setCursor(7, 1); //Day Ones Place
      break;
    
    case 2:
      lcd2.setCursor(9, 1); //Hour Tens Place
      break;

    case 3:
      lcd2.setCursor(10, 1); //Hour Ones Place
      break;

    case 4:
      lcd2.setCursor(12, 1); //Min Tens Place
      break;

    case 5:
      lcd2.setCursor(13, 1); //Min Ones Place
      break;
  }
  
  lcd2.blink();
}

void updateSetSoakOrDryLCD(uint8_t cursorPos, tmElements_t adjustedTime) {
  lcd2.noBlink();
  lcd2.setCursor(7, 1);
  char buff[6];

  sprintf(buff, "%02d %02d", adjustedTime.Hour, adjustedTime.Minute);
  lcd2.print(buff);

  switch (cursorPos) {
    case 0:
      lcd2.setCursor(7, 1); //Hour Tens Place
      break;
    
    case 1:
      lcd2.setCursor(8, 1); //Hour Ones Place
      break;
    
    case 2:
      lcd2.setCursor(10, 1); //Min Tens Place
      break;

    case 3:
      lcd2.setCursor(11, 1); //Min Ones Place
      break;
  }
  
  lcd2.blink();
}

void updateBrightnessOrContrastLCD(bool increasingBar) {
  if (increasingBar) {
    screenBrightness++;
    lcd2.setCursor(screenBrightness - 1, 1);
    lcd2.print(char(255)); //prints black rect
  }
  else {
    lcd2.setCursor(screenBrightness - 1, 1);
    screenBrightness--;
    lcd2.print(char(254)); //prints blank rect
  }
}