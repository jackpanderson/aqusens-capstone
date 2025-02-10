/* LCD Functions **************************************************************/

/**
 * @brief STANDBY screen
 * 
 * Displays current time and next sample time
 */
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

/**
 * @brief "Are you sure?" screen
 * 
 * @param ensure the state to enter if confirmed
 */
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

/**
 * @brief SETTINGS screen
 * 
 * Each page has a maximum of three setting options
 * 
 * @param page the current settings page to be displayed
 */
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
      lcd.print("TUBE FLUSH TIME");
      lcd.setCursor(1, 2);
      lcd.print("AQUSENS FLUSH TIME");

      lcd.setCursor(0, 3);
      lcd.print("<BACK");
      lcd.setCursor(15, 3);
      lcd.print("MORE>");
      break;
    
    case 3:
      lcd.setCursor(1, 0);
      lcd.print("DRY TIME");
      lcd.setCursor(1, 1);
      lcd.print("ADD EVENTS");
      lcd.setCursor(1, 2);
      lcd.print("VIEW EVENTS");

      lcd.setCursor(0, 3);
      lcd.print("<BACK");
      lcd.setCursor(15, 3);
      lcd.print("MORE>");
      break;

    case 4:
      lcd.setCursor(1, 0);
      lcd.print("FILTER STATUS");
      lcd.setCursor(1, 1);
      lcd.print("SET CONTRAST");

      lcd.setCursor(0, 3);
      lcd.print("<BACK"); // final page has no "more" option
      break;
    
  }

  lcd.setCursor(0, cursorY);
  lcd.print("*");

}

/**
 * @brief RELEASE screen
 * 
 * Displays the current position of motor releasing sample
 * 
 * @param position String of current position of sample device in meters
 */
void releaseLCD(String position) {
  lcd.setCursor(0,1);
  lcd.print("RELEASING DEVICE...");
  lcd.setCursor(0,2);
  lcd.print("CURR POSITION:");
  lcd.setCursor(15,2);
  lcd.print(position);
}

/**
 * @brief SOAK screen
 * 
 * Displays the time remaining in soak procedure
 * 
 * @param minTime String of minutes remaining in soak time
 * @param secTime String of seconds remaining in soak time
 * @param numDots Number of dots to display in flashing ...
 */
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
      lcd.print("   ");
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

/**
 * @brief RECOVER screen
 * 
 * Displays the current position of motor recovering sample
 * 
 * @param position String of current position of sample device in meters
 */
void recoverLCD(String position) {
  lcd.setCursor(0,1);
  lcd.print("RECOVERING DEVICE...");
  lcd.setCursor(0,2);
  lcd.print("CURR POSITION:");
  lcd.setCursor(15,2);
  lcd.print(position);
}

/**
 * @brief SAMPLE screen
 * TODO: flash ...
 *       display state of sampling stage in Aqusens
 */
void sampleLCD() {
  lcd.setCursor(0,1);
  lcd.print("SAMPLING...");
}

/**
 * @brief FLUSH_TUBE screen
 * 
 * @param minTime String of minutes remaining in flush time
 * @param secTime String of seconds remaining in flush time
 * @param numDots Number of dots to display in flashing ...
 * @param tempFlag Flag that determines whether temperature data is displayed
 */
void flushLCD(String minTime, String secTime, int numDots, bool tempFlag) {
  lcd.setCursor(1,0);

  if (state == FLUSH_TUBE) {
    lcd.setCursor(1,0);
    lcd.print("FLUSHING SYSTEM");
    lcd.setCursor(16, 0);
  }

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
      lcd.print("   ");
      break;
  }
  lcd.setCursor(4, 1);
  lcd.print("TEMP: ");
  
  if (tempFlag) {
    lcd.print(readRTD(TEMP_SENSOR_ONE), 1);
    lcd.print("C");
  }

  lcd.setCursor(4,2);
  lcd.print(minTime);
  lcd.print(" MIN ");
  lcd.print(secTime);
  lcd.print(" SEC");
  lcd.setCursor(6,3);
  lcd.print("REMAINING");
}

/**
 * @brief DRY screen
 * 
 * Displays the time remaining in dry procedure
 * 
 * @param minTime String of minutes remaining in dry time
 * @param secTime String of seconds remaining in dry time
 * @param numDots Number of dots to display in flashing ...
 */
void dryLCD(String minTime, String secTime, int numDots) {
  lcd.setCursor(6,0);
  lcd.print("DRYING");
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
      lcd.print("   ");
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

/**
 * @brief SOLENOID_CONTROL screen
 * 
 * Displays state of both solenoids
 */
void solenoidControlLCD() {
  lcd.setCursor(2, 0);
  lcd.print("SOLENOID CONTROL");
  lcd.setCursor(1, 1);
  lcd.print("SOLENOID 1: ");
  
  if (solenoidOneState == OPEN)
    lcd.print("OPEN");
  else 
    lcd.print("CLOSED");

  lcd.setCursor(1, 2);
  lcd.print("SOLENOID 2: ");
  if (solenoidTwoState == OPEN)
    lcd.print("OPEN");
  else 
    lcd.print("CLOSED");

  lcd.setCursor(0, cursorY);
  lcd.print("*");

  lcd.setCursor(0, 3);
  lcd.print("<EXIT");

  
}

/**
 * @brief Initialize SET_CLOCK screen
 * 
 * Initialize settings page option to set clock
 */
void initSetClockLCD() {
  lcd.setCursor(6, 0);
  lcd.print("SET TIME");
  lcd.setCursor(3, 2);
  lcd.print("MO DA YR HR MN");
  lcd.setCursor(3, 3);
  lcd.print("SEL TO CONFIRM");
}

/**
 * @brief Initialize SET_INTERVAL screen
 * 
 * Initialize settings page option to set sampling interval
 */
void initSetIntervalLCD() {
  lcd.setCursor(0, 0);
  lcd.print("SET SAMPLE INTERVAL");
  lcd.setCursor(6, 2);
  lcd.print("DA HR MN");
  lcd.setCursor(3, 3);
  lcd.print("SEL TO CONFIRM");
}

/**
 * @brief Initialize SET_SOAK_TIME/SET_DRY_TIME/SET_TUBE_FLUSH_TIME/SET_AQUSENS_FLUSH_TIME screen
 * 
 * Initialize settings page option to set soak/dry/flush time
 */
void initSetSoakOrDryOrFlushLCD() {
  if (state == SET_SOAK_TIME) {
    lcd.setCursor(3, 0);
    lcd.print("SET SOAK TIME");
  }
  else if (state == SET_DRY_TIME) {
    lcd.setCursor(4, 0);
    lcd.print("SET DRY TIME");
  }
  else if (state == SET_TUBE_FLUSH_TIME) {
    lcd.setCursor(0, 0);
    lcd.print("TUBE FLUSH TIME");
  }

  else if (state == SET_AQUSENS_FLUSH_TIME) {
    lcd.setCursor(1, 0);
    lcd.print("AQUSENS FLUSH TIME");
  }

  lcd.setCursor(7, 2);
  lcd.print("MN SC");
  lcd.setCursor(3, 3);
  lcd.print("SEL TO CONFIRM");
}

/**
 * @brief Initialize SET_BRIGHTNESS/SET_CONTRAST screen
 * 
 * Initialize settings page option to set brightness/contrast
 */
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

/**
 * @brief Update SET_CLOCK screen
 * 
 * Update settings page option to set clock
 */
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

/**
 * @brief Update SET_INTERVAL screen
 * 
 * Update settings page option to set sampling interval
 */
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

/**
 * @brief Update SET_SOAK_TIME/SET_DRY_TIME/SET_TUBE_FLUSH_TIME/SET_AQUSENS_FLUSH_TIME screen
 * 
 * Update settings page option to set soak/dry/flush time
 */
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

/**
 * @brief Update SET_BRIGHTNESS/SET_CONTRAST screen
 * 
 * Update settings page option to set brightness/contrast
 */
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

/**
 * @brief ESTOP_ALARM/MOTOR_ALARM screen
 * 
 * Displays Alarm mode for E-stop/Motor alarm
 */
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

/**
 * @brief Release E-Stop warning screen in ESTOP_ALARM
 * 
 * Displays warning for trying to exit alarm mode without releasing E-stop
 */
void releaseEstopLCD() {
  lcd.setCursor(1, 1);
  lcd.print("RELEASE E-STOP TO");
  lcd.setCursor(2, 2);
  lcd.print("EXIT ALARM MODE");
}

/**
 * @brief MANUAL screen
 * 
 */
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

/**
 * @brief MOTOR_CONTROL screen
 * 
 */
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
  //lcd.setCursor(10, 3);
  //lcd.print("CURR:");
  
  lcd.setCursor(0, cursorY);
  lcd.print("*");
}

/**
 * @brief Reset Motor screen in MOTOR_CONTROL
 * 
 */
void resetMotorLCD() {
  lcd.setCursor(0,1);
  lcd.print("RESETTING...");
}

/**
 * @brief Updates display of the current position of the motor
 * 
 * @param currPos current position of the motor
 */
void updateMotorCurrPositionDisplay(motorStatus status) {
  //int32_t currPos = 40; //Temporary, assuming will have a global variable that tracks position
  if (status != RAISING)
    lcd.setCursor(12, 3);
  else 
    lcd.setCursor(13, 3);

  // int meters = tube_position_f/100; //Converts cm to nearest meter, assuming less than 10!
  // Serial.print(tube_position_f);
  // Serial.print("->");
  // Serial.print(meters);
  // int remainingCm = tube_position_f - meters*100; //Leftover centimeters
  // Serial.println(remainingCm);

  // char formattedDistance[6]; //Ex: 4.54, assuming less than 10 meters 

  // if (remainingCm >= 10) {
  //   snprintf(formattedDistance, 6, "%i.%im", meters, remainingCm); 
  // }

  // else {
  //   snprintf(formattedDistance, 6, "%i.0%im", meters, remainingCm); //Accounts for leading zero if remaining cm is less than 10
  // }

  // lcd.print(formattedDistance);
  if (status == RAISING)
    lcd.print("RAISING");
  else if (status == LOWERING)
    lcd.print("LOWERING");
  else 
    lcd.print("         ");
}

/**
 * @brief clears the LCD and resets cursorY
 * 
 */
void resetLCD() {
  lcd.clear();
  cursorY = 0;
}
