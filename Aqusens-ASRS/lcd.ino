/* LCD Functions **************************************************************/

/**
 * @brief STANDBY screen
 * 
 * Displays current time and next sample time
 * 
 *  _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _
 * |C U R R : 0 0 - 0 0 - 0 0 _ 0 0 : 0 0 _|
 * |N E X T : 0 0 - 0 0 - 0 0 _ 0 0 : 0 0 _|
 * |* S E T T I N G S _ _ _ _ S T A N D B Y|
 * |_ R U N _ S A M P L E _ _ _ _ _ M O D E|
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
 *  _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _
 * |_ _ _ _ R U N _ S A M P L E _ _ _ _ _ _|
 * |_ _ _ A R E _ Y O U _ S U R E ? _ _ _ _|
 * |_ R U N _ S A M P L E _ _ _ _ _ _ _ _ _|
 * |* E X I T _ _ _ _ _ _ _ _ _ _ _ _ _ _ _|
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
      /*  _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _
       * |* I N T E R V A L _ _ _ _ _ _ _ _ _ _ _|
       * |_ S T A R T _ T I M E _ _ _ _ _ _ _ _ _|
       * |_ S E T _ C L O C K _ _ _ _ _ _ _ _ _ _|
       * |< E X I T _ _ _ _ _ _ _ _ _ _ M O R E >|
       */

      lcd.setCursor(1, 0);
      lcd.print("SET INTERVAL");
      lcd.setCursor(1, 1);
      lcd.print("SET START TIME");
      lcd.setCursor(1, 2);
      lcd.print("SET CLOCK");

      lcd.setCursor(0, 3);
      lcd.print("<EXIT"); // first page has "exit" option to return to standby
      lcd.setCursor(15, 3);
      lcd.print("MORE>");
      break;
    
    case 2:
      /*  _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _
       * |* S O A K _ T I M E _ _ _ _ _ _ _ _ _ _|
       * |_ T U B E _ F L U S H _ T I M E _ _ _ _|
       * |_ A Q U S E N S _ F L U S H _ T I M E _|
       * |< B A C K _ _ _ _ _ _ _ _ _ _ M O R E >|
       */

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
      /*  _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _
       * |* D R Y _ T I M E _ _ _ _ _ _ _ _ _ _ _|
       * |_ A D D _ E V E N T S _ _ _ _ _ _ _ _ _|
       * |_ V I E W _ E V E N T S _ _ _ _ _ _ _ _|
       * |< B A C K _ _ _ _ _ _ _ _ _ _ M O R E >|
       */

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
      /*  _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _
       * |* F I L T E R _ S T A T U S _ _ _ _ _ _|
       * |_ S E T _ C O N T R A S T _ _ _ _ _ _ _|
       * |_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _|
       * |< B A C K _ _ _ _ _ _ _ _ _ _ _ _ _ _ _|
       */

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
 *  _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _
 * |_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _|
 * |R E L E A S I N G _ D E V I C E . . . _|
 * |C U R R _ P O S I T I O N : 0 . 0 0 m _|
 * |_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _|
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
 *  _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _
 * |_ _ _ _ _ S O A K I N G . . . _ _ _ _ _|
 * |_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _|
 * |_ _ _ 0 0 _ M I N _ 0 0 _ S E C _ _ _ _|
 * |_ _ _ _ _ R E M A I N I N G _ _ _ _ _ _|
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
 *  _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _
 * |_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _|
 * |R E C O V E R I N G _ D E V I C E . . .|
 * |C U R R _ P O S I T I O N : 0 . 0 0 m _|
 * |_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _|
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
 * 
 *  _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _
 * |_ S A M P L I N G . . . _ _ _ _ _ _ _ _|
 * |_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _|
 * |_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _|
 * |_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _|
 */
void sampleLCD() {
  lcd.setCursor(0,1);
  lcd.print("SAMPLING...");
}

/**
 * @brief FLUSH_TUBE screen
 * 
 *  _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _
 * |_ F L U S H I N G _ S Y S T E M . . . _|
 * |_ _ _ _ T E M P : _ 0 0 . 0 C _ _ _ _ _|
 * |_ _ _ _ 0 0 _ M I N _ 0 0 _ S E C _ _ _|
 * |_ _ _ _ _ _ R E M A I N I N G _ _ _ _ _|
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
 *  _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _
 * |_ _ _ _ _ _ D R Y I N G . . . _ _ _ _ _|
 * |_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _|
 * |_ _ _ 0 0 _ M I N _ 0 0 _ S E C _ _ _ _|
 * |_ _ _ _ _ R E M A I N I N G _ _ _ _ _ _|
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
 * 
 *  _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _
 * |_ _ S O L E N O I D _ C O N T R O L _ _|
 * |* S O L E N O I D _ 1 : _ O P E N _ _ _|
 * |_ S O L E N O I D _ 2 : _ C L O S E D _|
 * |< E X I T _ _ _ _ _ _ _ _ _ _ _ _ _ _ _|
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
