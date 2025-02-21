/* State Machine Functions ******************************************************/

/**
 * @brief CALIBRATE
 * 
 * No selection options
 */
void calibrateLoop() {
  resetMotor();
  resetLCD();

  updateSolenoid(CLOSED, SOLENOID_ONE);
  updateSolenoid(CLOSED, SOLENOID_TWO);

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
  drop_distance_cm = 30;

  // get the distance to drop from online or sd card
  // while ((state == RELEASE) && (drop_distance_cm == 0))
  // {
  //   if (Serial.available()) {
  //       String data = Serial.readStringUntil('\n'); // Read full line
  //       drop_distance_cm = data.toFloat();  // Convert to float

  //       // if drop distance is -1 then get SD card info
  //       if (drop_distance_cm == -1) {
  //         drop_distance_cm = getTideData();
  //       }
  //       // TODO: calibrate drop distance

  //       // Serial.print("Received: ");
  //       // Serial.println(drop_distance_cm);

  //       // Flush any remaining characters
  //       while (Serial.available()) {
  //           Serial.read();  // Discard extra data
  //       }
  //   }
  //   checkEstop();
  // }

  // drop_distance_cm = getTideData();
  Serial.print("drop distance is ");
  Serial.println(drop_distance_cm);

  // actually drop the tube
  while (state == RELEASE){
    snprintf(pos, sizeof(pos), "%.2fm", tube_position_f / 100.0f);
    releaseLCD(pos);

    if (drop_tube(drop_distance_cm)) {
      state = SOAK;
    }
  }

  // while(1);
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

  // begin Aqusens sampling 
  Serial.println("S");

  while (state == SAMPLE) 
  {
    sampleLCD();

    // if (Serial.available()) {
    //   String data = Serial.readStringUntil('\n'); // Read full line
        
    //   // only transition to flushing after Aqusens sample done
    //   if (data == "D") {  
    //     state = FLUSH_TUBE;
    //   }

    //   // Flush any remaining characters
    //   while (Serial.available()) {
    //       Serial.read();  // Discard extra data
    //   }
    // }
    if (Serial.available()) {
      state = FLUSH_TUBE;
    }
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

    // if done flushing, exit loop
    if (flush_tube()) {
      state = DRY;
      break;
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

  // turn off Aqusens pump before transitioning to dry state
  // Serial.println("F");

  // while (state == FLUSH_TUBE) {
  //   checkEstop();

  //   if (Serial.available()) {
  //     String data = Serial.readStringUntil('\n'); // Read full line

  //     // only transition to drying after Aqusens pump turned off
  //     if (data == "D") {  
  //       state = DRY;
  //     }

  //     // Flush any remaining characters
  //     while (Serial.available()) {
  //         Serial.read();  // Discard extra data
  //     }
  //   }
  // }

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
    Serial.println(checkEstop());
    
    keyPressed = cursorSelect(2, 3);
    
    if (keyPressed == 'S') {
      if (cursorY == 3 && !checkEstop()) {
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
  motorControlLCD();
  updateMotorCurrPositionDisplay(MOTOR_OFF);
  char key;
  uint8_t keyPressed;
  uint8_t lastKeyPressed;

  lcd.clear();
  cursorY = 1;
  
  while (state == MOTOR_CONTROL) {
    motorControlLCD();
    
    
    keyPressed = getKeyTimeBasedDebounce();

    if (keyPressed == 'L') {
      state = MANUAL;
    }

    else if (keyPressed == 'S') {
      resetMotor();
    }

    else if (keyPressed == 'D') {
      while (pressAndHold('D') == 'D') {
        setMotorSpeed(-10);
        updateMotorCurrPositionDisplay(LOWERING);
        // Serial.println("D");
      }
      turnMotorOff();
      updateMotorCurrPositionDisplay(MOTOR_OFF);
    }

    else if (keyPressed == 'U') {
      while (!magSensorRead() && pressAndHold('U') == 'U') {
        if (magSensorRead()) 
          turnMotorOff();
        else
          setMotorSpeed(10);
        //Move motor up 1cm
        updateMotorCurrPositionDisplay(RAISING);
        // while(!retrieve_tube(1)); // Raises tube 
        // Serial.println("U");
      }
      turnMotorOff();
      updateMotorCurrPositionDisplay(MOTOR_OFF);
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