/* State Machine Functions ******************************************************/

/**
 * @brief CALIBRATE
 * 
 * No selection options
 */
void calibrateLoop() {
  resetMotor();
  resetLCD();

  lcd.setCursor(0, 0);
  lcd.print("Calibrating...");


  home_tube();
  state = STANDBY;
}

/**
 * @brief STANDBY
 * 
 * Two selection options:
 *    - Settings: proceeds to first page of settings page
 *    - Run Sample: proceeds to "Are you sure?" screen for manually running sample
 * Checks for E-Stop press
 */
void standbyLoop()
{ 
  static char key;
  static uint8_t keyPressed;
  lcd.clear();
  cursorY = 2;

  while (state == STANDBY) 
  {
    checkEstop();

    standbyLCD();
    
    keyPressed = cursorSelect(2, 3);

    if (keyPressed == 'S') {
      if (cursorY == 2) {
        settingsPage = 1;
        state = SETTINGS;
      }

      else if (cursorY == 3) {
        state = ENSURE_SAMPLE_START;
      }
    }
  }
}

/**
 * @brief ENSURE_SAMPLE_START
 * 
 * Two selection options:
 *    - Exit: returns to standby mode
 *    - Run Sample: proceeds to release mode
 */
void ensureSampleStartLoop() {
  char keyPressed;
  resetLCD();
  cursorY = 3;
  
  while (state == ENSURE_SAMPLE_START) {
    ensureLCD("RUN SAMPLE");

    keyPressed = cursorSelect(2, 3);
    
    if (keyPressed == 'S') {
      if (cursorY == 2) {
        state = RELEASE;
      }
      else if (cursorY == 3) {
        state = STANDBY;
      }
    }
  }
}

/**
 * @brief RELEASE
 * 
 * No selection options
 * Checks for E-Stop press
 */
void releaseLoop() {

  resetLCD();
  static char pos[6];

  // TODO: get the tide postion
  // drop the tube that much

  while (state == RELEASE) 
  {
    checkEstop();

    snprintf(pos, sizeof(pos), "%.2fm", tube_position_f / 100.0f);
    releaseLCD(pos);

    if (drop_tube(drop_distance_cm)) {
      state = SOAK;
    }
  }
}

/**
 * @brief SOAK
 * 
 * No selection options
 * Checks for E-Stop press
 */
void soakLoop() {
  char secTime[3]; // "00"
  char minTime[3]; // "00"

  resetLCD();

  uint32_t currTime = millis();
  uint32_t endTime = currTime + (60 * soakTime.Minute * 1000) + (soakTime.Second * 1000);

  int secondsRemaining, minutesRemaining;

  while (state == SOAK && millis() < endTime) {
    checkEstop();

    // Calculate remaining time, accounting for millis() overflow
    uint32_t millisRemaining;
    if (endTime > millis()) {
      millisRemaining = endTime - millis();
    } else {
      // Handle overflow case
      millisRemaining = (UINT32_MAX - millis()) + endTime;
    }

    secondsRemaining = millisRemaining / 1000;
    minutesRemaining = secondsRemaining / 60;

    // Format seconds with leading zero if necessary
    if (secondsRemaining % 60 > 9) {
      snprintf(secTime, sizeof(secTime), "%i", secondsRemaining % 60);
    } else {
      snprintf(secTime, sizeof(secTime), "0%i", secondsRemaining % 60);
    }

    // Format minutes with leading zero if necessary
    if (minutesRemaining > 9) {
      snprintf(minTime, sizeof(minTime), "%i", minutesRemaining);
    } else {
      snprintf(minTime, sizeof(minTime), "0%i", minutesRemaining);
    }

    // Update LCD with remaining time
    soakLCD(minTime, secTime, secondsRemaining % 4);
  }

  state = RECOVER;
}

/**
 * @brief RECOVER
 * 
 * No selection options
 * Checks for E-Stop press
 */
void recoverLoop() {
  static char pos[6];

  resetLCD();

  while (state == RECOVER) 
  {
    checkEstop();

    if (retrieve_tube(tube_position_f + .1)) {
      state = SAMPLE;
    }
    snprintf(pos, sizeof(pos), "%.2fm", tube_position_f / 100.0f);
    recoverLCD(pos);
  }

}

/**
 * @brief SAMPLE
 * 
 * No selection options
 */
void sampleLoop() {
  resetLCD();

  while (state == SAMPLE) 
  {
    sampleLCD();

    delay(5 * 1000);

    state = FLUSH_TUBE;

  }
}

/**
 * @brief FLUSH
 * 
 * No selection options
 */
void tubeFlushLoop() {
  resetLCD();
  bool tempFlag = true;
  char secTime[3]; // "00"
  char minTime[3]; // "00"

  uint32_t currTime = millis();
  uint32_t endTime = currTime + (60 * tubeFlushTime.Minute * 1000) + (tubeFlushTime.Second * 1000);

  int secondsRemaining, minutesRemaining;
  uint32_t lastToggleTime = currTime; // Track the last time tempFlag was toggled

  // while (state == FLUSH_TUBE && millis() < endTime) {
  while (state == FLUSH_TUBE) {
    checkEstop();

    if (flush_tube()) {
      state = DRY;
    }

    // Calculate remaining time, accounting for millis() overflow
    uint32_t millisRemaining;
    if (endTime > millis()) {
      millisRemaining = endTime - millis();
    } else {
      // Handle overflow case
      millisRemaining = (UINT32_MAX - millis()) + endTime;
    }

    secondsRemaining = millisRemaining / 1000;
    minutesRemaining = secondsRemaining / 60;

    // Format seconds with leading zero if necessary
    if (secondsRemaining % 60 > 9) {
      snprintf(secTime, sizeof(secTime), "%i", secondsRemaining % 60);
    } else {
      snprintf(secTime, sizeof(secTime), "0%i", secondsRemaining % 60);
    }

    // Format minutes with leading zero if necessary
    if (minutesRemaining > 9) {
      snprintf(minTime, sizeof(minTime), "%i", minutesRemaining);
    } else {
      snprintf(minTime, sizeof(minTime), "0%i", minutesRemaining);
    }

    // Toggle tempFlag every second
    if (millis() - lastToggleTime >= 1000) {
      tempFlag = true; // Toggle tempFlag
      lastToggleTime = millis(); // Update the last toggle time
    }

    // Update LCD with remaining time
    flushLCD(minTime, secTime, secondsRemaining % 4, tempFlag);
    
    if (tempFlag)
      tempFlag = false;
  }

  state = DRY;
}

/**
 * @brief DRY
 * 
 * No selection options
 */
void dryLoop() {
  // setMotorSpeed(0);
  
  char secTime[3]; // "00"
  char minTime[3]; // "00"

  resetLCD();

  uint32_t currTime = millis();
  uint32_t endTime = currTime + (60 * dryTime.Minute * 1000) + (dryTime.Second * 1000);

  int secondsRemaining, minutesRemaining;

  while (state == DRY && millis() < endTime) {
    checkEstop();

    // Calculate remaining time, accounting for millis() overflow
    uint32_t millisRemaining;
    if (endTime > millis()) {
      millisRemaining = endTime - millis();
    } else {
      // Handle overflow case
      millisRemaining = (UINT32_MAX - millis()) + endTime;
    }

    secondsRemaining = millisRemaining / 1000;
    minutesRemaining = secondsRemaining / 60;

    // Format seconds with leading zero if necessary
    if (secondsRemaining % 60 > 9) {
      snprintf(secTime, sizeof(secTime), "%i", secondsRemaining % 60);
    } else {
      snprintf(secTime, sizeof(secTime), "0%i", secondsRemaining % 60);
    }

    // Format minutes with leading zero if necessary
    if (minutesRemaining > 9) {
      snprintf(minTime, sizeof(minTime), "%i", minutesRemaining);
    } else {
      snprintf(minTime, sizeof(minTime), "0%i", minutesRemaining);
    }

    // Update LCD with remaining time
    dryLCD(minTime, secTime, secondsRemaining % 4);
  }

  state = STANDBY;
  tube_position_f = 0;
  Serial.print("Done with full seq. Final tube_pos = ");
  Serial.println(tube_position_f);
}

/**
 * @brief ALARM
 * 
 * Two selection options:
 *    - Exit: returns to standby mode if alarm is resolved
 *            otherwise, flashes warning
 *    - Manual Mode: proceeds to manual mode
 */
void alarmLoop() {
  setMotorSpeed(0);
  char key;
  uint8_t keyPressed;
  lcd.clear();
  cursorY = 2;
  while (state == ESTOP_ALARM || state == MOTOR_ALARM) 
  {
    alarmLCD();
    
    keyPressed = cursorSelect(2, 3);
    
    if (keyPressed == 'S') {
      if (cursorY == 3 && ~checkEstop()) {
        state = CALIBRATE;
      }
      
      else if (cursorY == 3 && checkEstop()) {
        lcd.clear();
        releaseEstopLCD();
        delay(1500);
        lcd.clear();
      }

      else if (cursorY == 2) {
        state = MANUAL;
      }
    }
  }
}

/**
 * @brief MANUAL
 * 
 * Three selection options:
 *    - Motor: proceeds to MOTOR_CONTROL state
 *    - Solenoids: proceeds to SOLENOID_CONTROL state
 *    - Exit: returns to previous alarm state
 */
void manualLoop() {
  char key;
  uint8_t keyPressed;
  lcd.clear();
  cursorY = 1;
  while (state == MANUAL) {
    manualLCD();
    keyPressed = cursorSelect(1, 3);

    if (keyPressed == 'S') {
      if (cursorY == 1) {
        state = MOTOR_CONTROL;
      } 

      else if (cursorY == 2) {
        state = SOLENOID_CONTROL;
      }

      else if (cursorY == 3) {
        state = ESTOP_ALARM; // TODO: change to previous state (ESTOP vs. ALARM)
      }
    }
  }
}

/**
 * @brief MOTOR_CONTROL
 * 
 * Four selection options:
 *    - Reset: cycles motor power to reset any alarm
 *    - Raise: slowly raises motor in increments
 *    - Lower: slowly lowers motor in increments
 *    - Back: returns to manual mode
 */
void motorControlLoop() {
  int fakePos = 50; // TODO: REMOVE EVENTUALLY, USE GLOBAL MOTOR POSITION VARIABLE
  motorControlLCD();
  updateMotorCurrPositionDisplay(fakePos);
  char key;
  uint8_t keyPressed;
  uint8_t lastKeyPressed;

  lcd.clear();
  cursorY = 1;
  
  while (state == MOTOR_CONTROL) {
    motorControlLCD();
    updateMotorCurrPositionDisplay(fakePos); // Eventually should not need an argument
    
    keyPressed = getKeyTimeBasedDebounce();

    if (keyPressed == 'L') {
      state = MANUAL;
    }

    else if (keyPressed == 'S') {
      resetMotor();
    }

    else if (keyPressed == 'D') {
      while (pressAndHold('D') == 'D') {
        // move motor down 1cm
        updateMotorCurrPositionDisplay(fakePos++);
      }
    }

    else if (keyPressed == 'U') {
      while (pressAndHold('U') == 'U') {
        //Move motor up 1cm
        updateMotorCurrPositionDisplay(fakePos--);
      }
    }
  }
}

/**
 * @brief SOLENOID_CONTROL
 * 
 * Three selection options:
 *    - Solenoid 1: switches Solenoid 1 relay
 *    - Solenoid 2: switches Solenoid 2 relay
 *    - Exit: returns to manual mode
 */
void solenoidControlLoop() {
  lcd.clear();
  char keyPressed;
  cursorY = 1;

  while (state == SOLENOID_CONTROL) {
    solenoidControlLCD();
    keyPressed = cursorSelect(1, 2);

    if (keyPressed == 'S') {
      if (cursorY == 1) {

        if (solenoidOneState == OPEN) {
          solenoidOneState = CLOSED;
        }

        else {
          solenoidOneState = OPEN;
        }

        updateSolenoid(solenoidOneState, SOLENOID_ONE);
      } 

      else if (cursorY == 2) {
        if (solenoidTwoState == OPEN) {
          solenoidTwoState = CLOSED;
        }

        else {
          solenoidTwoState = OPEN;
        }

        updateSolenoid(solenoidTwoState, SOLENOID_TWO);
      } 
    }
    else if (keyPressed == 'L') {
      state = MANUAL;
    }
  }
}
