// Displays current time and next sample time
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

// "Are you sure?" screen
void ensureLCD(String ensure) {
  lcd.setCursor(4, 0);
  lcd.print(ensure);
  lcd.setCursor(3, 1);
  lcd.print("ARE YOU SURE?");
  lcd.setCursor(1, 2);
  lcd.print(ensure);
  lcd.setCursor(1, 3);
  lcd.print("EXIT");

  lcd.setCursor(0, cursorY);
  lcd.print("*");
}

// Settings screen pages
//    page: the current settings page to be displayed
//    Every page has a maximum of three setting options
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
      lcd.print("<EXIT"); // first page has "exit" option to return to standby
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
      lcd.print("<BACK"); // final page has no "more" option
      break;
    
  }

  lcd.setCursor(0, cursorY);
  lcd.print("*");

}

// Displays the current position of motor releasing sample
//    position: String of current position of sample device in meters
void releaseLCD(String position) {
  lcd.setCursor(0,1);
  lcd.print("RELEASING DEVICE...");
  lcd.setCursor(0,2);
  lcd.print("CURR POSITION:");
  lcd.setCursor(15,2);
  lcd.print(position);
}

// Displays the time remaining in soak procedure
//    time: String of time remaining in soak time in minutes
void soakLCD(String minTime, String secTime, int numDots) {
  lcd.setCursor(5,0);
  lcd.print("SOAKING");
  lcd.setCursor(12, 0);


  switch (numDots) {
    case 0: 
      lcd.print("...");
      break;
    case 1:
      lcd.print(".. ");
      break;
    case 2:
      lcd.print(".  ");
      break;
    case 3:
      // lcd.print("...");
      break;
  }


  lcd.setCursor(3,2);
  lcd.print(minTime);
  lcd.print(" MIN ");
  lcd.print(secTime);
  lcd.print(" SEC");
  lcd.setCursor(5,3);
  lcd.print("REMAINING");
}

// Displays the current position of motor recovering sample
//    position: String of current position of sample device in meters
void recoverLCD(String position) {
  lcd.setCursor(0,1);
  lcd.print("RECOVERING DEVICE...");
  lcd.setCursor(0,2);
  lcd.print("CURR POSITION:");
  lcd.setCursor(15,2);
  lcd.print(position);
}

// Displays the Sampling screen
void sampleLCD() {
  lcd.setCursor(0,1);
  lcd.print("SAMPLING...");
}

// Displays the Flsuhing screen
void flushLCD() {
  lcd.setCursor(0,1);
  lcd.print("FLUSHING...");
  lcd.print("TEMP:");
  lcd.print(readRTD());
  lcd.print("F");
}

// Displays the time remaining in dry procedure
//    time: String of time remaining in dry time in minutes
void dryLCD(String time) {
  lcd.setCursor(0,1);
  lcd.print("DRYING...");
  lcd.setCursor(0,2);
  lcd.print(time);
  lcd.setCursor(7,2);
  lcd.print("REMAINING");
}

// Settings page option to set clock
void initSetClockLCD() {
  lcd.setCursor(6, 0);
  lcd.print("SET TIME");
  lcd.setCursor(3, 2);
  lcd.print("MO DA YR HR MN");
  lcd.setCursor(3, 3);
  lcd.print("SEL TO CONFIRM");
}

// Settings page option to set sampling interval
void initSetIntervalLCD() {
  lcd.setCursor(0, 0);
  lcd.print("SET SAMPLE INTERVAL");
  lcd.setCursor(6, 2);
  lcd.print("DA HR MN");
  lcd.setCursor(3, 3);
  lcd.print("SEL TO CONFIRM");
}

// Settings page option to set soak or dry time
void initSetSoakOrDryOrFlushLCD() {
  if (state == SET_SOAK_TIME) {
    lcd.setCursor(3, 0);
    lcd.print("SET SOAK TIME");
  }
  else if (state == SET_DRY_TIME) {
    lcd.setCursor(4, 0);
    lcd.print("SET DRY TIME");
  }
  else if (state == SET_FLUSH_TIME) {
    lcd.setCursor(3, 0);
    lcd.print("SET FLUSH TIME");
  }
  lcd.setCursor(7, 2);
  lcd.print("MN SC");
  lcd.setCursor(3, 3);
  lcd.print("SEL TO CONFIRM");
}

// Settings page option to set brightness/contrast
void initSetBrightnessOrConstrastLCD() {
  resetLCD();

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

void updateSetSoakOrDryOrFlushLCD(uint8_t cursorPos, tmElements_t adjustedTime) {
  lcd.noBlink();
  lcd.setCursor(7, 1);
  char buff[6];

  sprintf(buff, "%02d %02d", adjustedTime.Minute, adjustedTime.Second);
  lcd.print(buff);

  switch (cursorPos) {
    case 0:
      lcd.setCursor(7, 1); //Min Tens Place
      break;
    
    case 1:
      lcd.setCursor(8, 1); //Min Ones Place
      break;
    
    case 2:
      lcd.setCursor(10, 1); //Sec Tens Place
      break;

    case 3:
      lcd.setCursor(11, 1); //Sec Ones Place
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

// Displays Alarm mode for E-stop/Motor alarm
void alarmLCD() {
  lcd.setCursor(7, 0);
  lcd.print("ALARM");
  lcd.setCursor(1, 2);
  lcd.print("MANUAL MODE");
  lcd.setCursor(1, 3);
  lcd.print("EXIT");
  lcd.setCursor(0, 1);
  lcd.print("REASON:");
  lcd.setCursor(8, 1);

  if (state == ESTOP_ALARM) {
    lcd.print("E-STOP");
  } else {
    lcd.print("MOTOR");
  }

  lcd.setCursor(0, cursorY);
  lcd.print("*");
}

// Displays warning for trying to exit alarm mode without releasing E-stop
void releaseEstopLCD() {
  lcd.setCursor(1, 1);
  lcd.print("RELEASE E-STOP TO");
  lcd.setCursor(2, 2);
  lcd.print("EXIT ALARM MODE");
}

void manualLCD() {
  lcd.setCursor(5, 0);
  lcd.print("MANUAL MODE");
  lcd.setCursor(1, 1);
  lcd.print("MOTOR");
  lcd.setCursor(1, 2);
  lcd.print("SOLENOIDS");
  lcd.setCursor(1, 3);
  lcd.print("EXIT");
  
  lcd.setCursor(0, cursorY);
  lcd.print("*");
}

void motorControlLCD() {
  lcd.setCursor(3, 0);
  lcd.print("MOTOR CONTROLS");
  lcd.setCursor(1, 1);
  lcd.print("SEL TO RESET MOTOR");
  lcd.setCursor(0, 2);
  lcd.print("^RAISE");
  lcd.setCursor(14, 2);
  lcd.print("vLOWER");
  lcd.setCursor(0, 3);
  lcd.print("<BACK");
  lcd.setCursor(10, 3);
  lcd.print("CURR:");
  
  lcd.setCursor(0, cursorY);
  lcd.print("*");
}

void resetMotorLCD() {
  lcd.setCursor(0,1);
  lcd.print("RESETTING...");
}

void resetLCD()
{
  lcd.clear();
  cursorY = 0;
}
