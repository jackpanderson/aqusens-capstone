
/* State Machine Functions ******************************************************/

// CALIBRATE
// No selection options
void calibrateLoop() {
  resetLCD();
  lcd.setCursor(0, 0);
  lcd.print("Calibrating...");
  delay(5000);
  state = STANDBY;
}

// STANDBY
// Two selection options:
//    - Settings: proceeds to first page of settings page
//    - Run Sample: proceeds to "Are you sure?" screen for manually running sample
// Checks for E-stop press
void standbyLoop()
{ 
  char key;
  uint8_t keyPressed;
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

  // setMotorDir('D');
  // setMotorSpeed(50000);

  resetLCD();
  int position = 0;
  char pos[6];

  while (state == RELEASE) 
  {
    checkEstop();
    int meter = position / 100;
    int deci = position % 100;
    snprintf(pos, sizeof(pos), "%01d.%02dm", meter, deci);
    releaseLCD(pos);

    delay(33);
    position++;

    if (position == 900) {
      state = SOAK;
      // setMotorSpeed(0);
    }
  }
}

// SOAK
// No selection options
// Checks for E-stop press
void soakLoop() {
  // setMotorSpeed(0);
  resetLCD();
  tmElements_t endSoak;

  endSoak.Year = rtc.getYear() + 30;
  endSoak.Month = rtc.getMonth();
  endSoak.Day = rtc.getDay();
  endSoak.Hour = rtc.getHours() + soakTime.Hour;
  endSoak.Minute = rtc.getMinutes() + soakTime.Minute;

  time_t end = rtc.getEpoch() + 60 * soakTime.Minute;
  char time[6];
  int min;
  while (state == SOAK) 
  {
    checkEstop();
    Serial.println(end - rtc.getEpoch());
    min = (end - rtc.getEpoch()) / 60 + 1;
    
    if (min > soakTime.Minute) {
      min = soakTime.Minute;
    }

    snprintf(time, sizeof(time), "%01d MIN", min);
    soakLCD(time);
    if (end - rtc.getEpoch() <= 0) {
      state = RECOVER;
    }

  }
}


// RECOVER
// No selection options
// Checks for E-stop press
void recoverLoop() {
  // setMotorDir('U');
  // setMotorSpeed(50000);
  resetLCD();
  int position = 900;
  char pos[6];

  while (state == RECOVER) 
  {
    checkEstop();
    int meter = position / 100;
    int deci = position % 100;
    snprintf(pos, sizeof(pos), "%01d.%02dm", meter, deci);
    recoverLCD(pos);

    delay(33);
    position--;

    if (position < 0) {
      state = SAMPLE;
      // setMotorSpeed(0);
    }
  }
}

// SAMPLE
// No selection options
void sampleLoop() {
  resetLCD();

  while (state == SAMPLE) 
  {
    sampleLCD();

    delay(5000);

    state = FLUSH;

  }
}

// FLUSH
// No selection options
void flushLoop() {
  resetLCD();

  while (state == FLUSH) 
  {
    flushLCD();

    delay(5000);

    state = DRY;

  }
}

// DRY
// No selection options
void dryLoop() {
  resetLCD();

  while (state == DRY) 
  {
    dryLCD("00 min");

    delay(5000);

    state = STANDBY;

  }
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
  while (state == ESTOP_ALARM || state == MOTOR_ALARM) 
  {
    alarmLCD();
    
    keyPressed = cursorSelect(2, 3);
    
    if (keyPressed == 'S') {
      if (cursorY == 3 && !estopPressed) {
        state = CALIBRATE;
      }

      else if (cursorY == 3 && estopPressed) {
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
      } //else if (cursorY == 2) {

      //} 
      else if (cursorY == 3) {
        state = ESTOP_ALARM; // TODO: change to previous state
      }
    }
  }
}

void motorControlLoop() {
  char key;
  uint8_t keyPressed;
  lcd.clear();
  cursorY = 1;
  while (state == MOTOR_CONTROL) {
    motorControlLCD();
    keyPressed = cursorSelect(1, 1); // TODO: change to use different controls

    if (keyPressed == 'S') {
      state = MANUAL;
    }
  }
}
