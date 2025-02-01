#define PRESS_AND_HOLD_INTERVAL_MS 50
/* State Machine Functions ******************************************************/

// CALIBRATE
// No selection options
void calibrateLoop() {
  resetLCD();
  lcd.setCursor(0, 0);
  lcd.print("Calibrating...");
  home_tube();
  state = STANDBY;
}

// STANDBY
// Two selection options:
//    - Settings: proceeds to first page of settings page
//    - Run Sample: proceeds to "Are you sure?" screen for manually running sample
// Checks for E-stop press
// NOTE: I dont really like blocking blocks but this isnt really a critical section
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

// ENSURE_SAMPLE_START
// Two selection options:
//    - Exit: returns to standby mode
//    - Run Sample: proceeds to release mode
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


// RELEASE
// No selection options
// Checks for E-stop press
void releaseLoop() {

  resetLCD();
  static char pos[6];

  while (state == RELEASE) 
  {
    checkEstop();

    snprintf(pos, sizeof(pos), "%.2fm", tube_position_f / 100.0f);
    releaseLCD(pos);

    if (drop_tube(drop_distance_cm)) {
      while (1);
      state = SOAK;
    }
  }
}

// SOAK
// No selection options
// Checks for E-stop press
void soakLoop() {
  char secTime[3]; // "00"
  char minTime[3]; // "00"

  resetLCD();

  uint32_t currTime = millis();
  uint32_t endTime = currTime + (60 * soakTime.Minute * 1000) + (soakTime.Second * 1000);
  // Serial.println(endTime - currTime);

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


// RECOVER
// No selection options
// Checks for E-stop press
void recoverLoop() {
  static char pos[6];

  resetLCD();

  while (state == RECOVER) 
  {
    checkEstop();
    if (raise_tube(tube_position)) {
      state = SAMPLE;
    }
    int meter = tube_position / 100;
    int deci = tube_position % 100;
    snprintf(pos, sizeof(pos), "%01d.%02dm", meter, deci);
    recoverLCD(pos);
  }
}

// SAMPLE
// No selection options
// This would be where the aquasens thing takes place
void sampleLoop() {
  resetLCD();

  while (state == SAMPLE) 
  {
    sampleLCD();

    delay(5 * 1000);

    state = FLUSH;

  }
}

// FLUSH
// No selection options
void flushLoop() {
  resetLCD();
  bool tempFlag = true;
  char secTime[3]; // "00"
  char minTime[3]; // "00"

  uint32_t currTime = millis();
  uint32_t endTime = currTime + (60 * flushTime.Minute * 1000) + (flushTime.Second * 1000);
  Serial.println(endTime - currTime);

  int secondsRemaining, minutesRemaining;
  uint32_t lastToggleTime = currTime; // Track the last time tempFlag was toggled

  while (state == FLUSH && millis() < endTime) {
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

// DRY
// No selection options
void dryLoop() {
  // setMotorSpeed(0);
  
  char secTime[3]; // "00"
  char minTime[3]; // "00"

  resetLCD();

  uint32_t currTime = millis();
  uint32_t endTime = currTime + (60 * dryTime.Minute * 1000) + (dryTime.Second * 1000);
  Serial.println(endTime - currTime);

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
}

// ALARM
// Two selection options:
//    - Exit: returns to standby mode if alarm is resolved
//            otherwise, flashes warning
//    - Manual Mode: proceeds to manual mode
void alarmLoop() {
  setMotorSpeed(0);
  char key;
  uint8_t keyPressed;
  lcd.clear();
  cursorY = 2;
  //Serial.println(analogRead(A1));
  while (state == ESTOP_ALARM || state == MOTOR_ALARM) 
  {
    alarmLCD();
    
    keyPressed = cursorSelect(2, 3);
    
    if (keyPressed == 'S') {
      if (cursorY == 3 && checkEstop()) {
        // Serial.println(digitalRead(A1));
        state = CALIBRATE;
      }
      
      else if (cursorY == 3 && ~checkEstop()) {
        // Serial.println(digitalRead(A1));
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

// MANUAL
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
        state = ESTOP_ALARM; // TODO: change to previous state
      }
    }
  }
}

void motorControlLoop() {
  int fakePos = 50; // Jack: REMOVE EVENTUALLY, USE GLOBAL MOTOR POSITION VARIABLE
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


char pressAndHold(uint8_t lastKeyPressed) {
  //uint8_t currKey = getKeyPress();
  unsigned long startTime = millis();
  unsigned long currTime = millis();

  while(currTime - startTime < PRESS_AND_HOLD_INTERVAL_MS) {
    if (getKeyPress() != lastKeyPressed) {
      return 0; //Target key has been let go
    }
    currTime = millis();
  }

  return lastKeyPressed;
}
