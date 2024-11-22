
/* State Machine Functions ******************************************************/

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
  resetLcd();
  cursorY = 3;
  
  while (state == ENSURE_SAMPLE_START) {
    ensureSampleStartLCD();

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

// SETTINGS
// 
void settingsLoop() {
  uint8_t lastKeyPress = rtc.getMinutes();
  char keyPressed;
  resetLcd();
  
  while (state == SETTINGS) {
    checkEstop();
    settingsLCD(settingsPage); //Launch into first page of settings

    if (settingsPage != lastSettingPage)
      keyPressed = cursorSelect(0, 2);
    else
      keyPressed = cursorSelect(0, 1);

    if (keyPressed > 0) {
      lastKeyPress = rtc.getMinutes();
    }

    if (keyPressed == 'L') { // left
      if (settingsPage == 1) { // exits to STANDBY mode on first page left select
        resetLcd();
        state = STANDBY;
        cursorY = 2;
      }
      else { // else, returns to previous settings page
        resetLcd();
        settingsPage--;
      }
    }

    else if (keyPressed == 'R' && settingsPage != lastSettingPage) { //right
      resetLcd();
      settingsPage++;
    }

    else if (keyPressed == 'S') {
      if (settingsPage == 1) {
        switch(cursorY) {
          case 0:
            state = SET_INTERVAL;
            break;
          
          case 1:
            state = SET_START_TIME;
            break;
          
          case 2:
            state = SET_CLOCK;
            break;
          
          default:
            break;
        }
      }

      else if (settingsPage == 2) {
        switch(cursorY) { //add more states
          case 0:
            state = SET_SOAK_TIME;
            break;
          
          case 1:
            state = SET_FLUSH_TIME;
            break;
          
          case 2:
            state = SET_DRY_TIME;
            break;
          
          default:
            break;
        }
      }

      else if (settingsPage == 3) {
        switch(cursorY) {
          case 0:
            state = ADD_EVENT;
            break;
          
          case 1:
            state = VIEW_EVENTS;
            break;
          
          case 2:
            state = FILTER_STATUS;
            break;
          
          default:
            break;
        }
      }

      else if (settingsPage == 4) {
        switch(cursorY) {
          case 0:
            state = SET_BRIGHTNESS;
            break;
          case 1:
            state = SET_CONTRAST;
            break;
        }
      }
    }

    if (rtc.getMinutes() == ((lastKeyPress + 5) % 60)) {
      state = STANDBY;
    }
  }
}

void releaseLoop() {

  // setMotorDir('D');
  // setMotorSpeed(50000);

  resetLcd();
  int position = 0;
  char pos[6];

  while (state == RELEASE) 
  {
    checkEstop();
    int meter = position / 100;
    int deci = position % 100;
    snprintf(pos, sizeof(pos), "%01d.%02dm", meter, deci);
    releaseLcd(pos);

    delay(33);
    position++;

    if (position == 900) {
      state = SOAK;
      // setMotorSpeed(0);
    }
  }
}

void soakLoop() {
  // setMotorSpeed(0);
  resetLcd();
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
    soakLcd(time);
    if (end - rtc.getEpoch() <= 0) {
      state = RECOVER;
    }

  }
}

void setStartTimeLoop() {
  tmElements_t adjustedStartTime;
  adjustedStartTime.Month = rtc.getMonth();
  adjustedStartTime.Day = rtc.getDay();
  adjustedStartTime.Year = rtc.getYear(); 
  adjustedStartTime.Hour = rtc.getHours();
  adjustedStartTime.Minute = rtc.getMinutes();
  char key;
  uint8_t cursorPos = 0; // Holds time-setting position of cursor for "00-00-00 00:00"
                         // Does not include spacing/colons/hyphens, ranges 0 to 9.

  resetLcd();
  initSetClockLcd();
  updateSetClockLCD(cursorPos, adjustedStartTime);
  lcd.blink();

  while (state == SET_START_TIME) {
    key = getKeyDebounce();
    
    if (key != NULL) {
      if (key == 'S') {
        updateAlarm(adjustedStartTime);
        lcd.noBlink();
        state = SETTINGS;
      }

      else if (key == 'L' && cursorPos > 0) {
        cursorPos--;
      }

      else if (key == 'R' && cursorPos < 9) {
      cursorPos++;
      }

      else if (key == 'U' || key == 'D') {
        adjustSetTimeDigit(key, &adjustedStartTime, &cursorPos);
      }
      if (state == SET_START_TIME) {
        updateSetClockLCD(cursorPos, adjustedStartTime);
      }
    } 
  }

}

void recoverLoop() {
  // setMotorDir('U');
  // setMotorSpeed(50000);
  resetLcd();
  int position = 900;
  char pos[6];

  while (state == RECOVER) 
  {
    checkEstop();
    int meter = position / 100;
    int deci = position % 100;
    snprintf(pos, sizeof(pos), "%01d.%02dm", meter, deci);
    recoverLcd(pos);

    delay(33);
    position--;

    if (position < 0) {
      state = SAMPLE;
      // setMotorSpeed(0);
    }
  }
}

void sampleLoop() {
  resetLcd();

  while (state == SAMPLE) 
  {
    sampleLcd();

    delay(5000);

    state = FLUSH;

  }
}

void flushLoop() {
  resetLcd();

  while (state == FLUSH) 
  {
    flushLcd();

    delay(5000);

    state = DRY;

  }
}

void dryLoop() {
  resetLcd();

  while (state == DRY) 
  {
    dryLcd("00 min");

    delay(5000);

    state = STANDBY;

  }
}

void manualLoop(){}

void alarmLoop() {
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
    }
  }
}


void setClockLoop() {

  Serial.println(rtc.getYear());

  tmElements_t adjustedTime;
  adjustedTime.Month = rtc.getMonth();
  adjustedTime.Day = rtc.getDay();
  adjustedTime.Year = rtc.getYear(); 
  adjustedTime.Hour = rtc.getHours();
  adjustedTime.Minute = rtc.getMinutes();
  char key;
  uint8_t cursorPos = 0; // Holds time-setting position of cursor for "00-00-00 00:00"
                         // Does not include spacing/colons/hyphens, ranges 0 to 9.

  resetLcd();
  initSetClockLcd();
  updateSetClockLCD(cursorPos, adjustedTime);
  lcd.blink();

  while (state == SET_CLOCK) {
    key = getKeyDebounce();
    
    if (key != NULL) {
      if (key == 'S') {
        breakTime(makeTime(adjustedTime), adjustedTime);
        rtc.setMonth(adjustedTime.Month);
        rtc.setDay(adjustedTime.Day);
        rtc.setYear(adjustedTime.Year);
        rtc.setHours(adjustedTime.Hour);
        rtc.setMinutes(adjustedTime.Minute);
        updateAlarm();
        lcd.noBlink();
        state = SETTINGS;
      }

      else if (key == 'L' && cursorPos > 0) {
        cursorPos--;
      }

      else if (key == 'R' && cursorPos < 9) {
        cursorPos++;
      }

      else if (key == 'U' || key == 'D') {
        adjustSetTimeDigit(key, &adjustedTime, &cursorPos);
      }
      if (state == SET_CLOCK) {
        updateSetClockLCD(cursorPos, adjustedTime);
      }
    } 
  }
}

void setIntervalLoop() {
  tmElements_t newInterval;
  newInterval.Day = sampleInterval.Day;
  newInterval.Hour = sampleInterval.Hour;
  newInterval.Minute = sampleInterval.Minute;
  char key;
  uint8_t cursorPos = 0; // Holds time-setting position of cursor for "00 00 00" (Day Hour Min)
                         // Does not include spacing/colons/hyphens, ranges 0 to 5.

  resetLcd();
  initSetIntervalLCD();
  updateSetIntervalLCD(cursorPos, newInterval);
  lcd.blink();

  while (state == SET_INTERVAL) {
    key = getKeyDebounce();
    
    if (key != NULL) {
      if (key == 'S') {
        sampleInterval.Day = newInterval.Day;
        sampleInterval.Hour = newInterval.Hour;
        sampleInterval.Minute = newInterval.Minute;
        updateAlarm();
        lcd.noBlink();
        state = SETTINGS;
      }

      else if (key == 'L' && cursorPos > 0) {
        cursorPos--;
      }

      else if (key == 'R' && cursorPos < 5) {
      cursorPos++;
      }

      else if (key == 'U' || key == 'D') {
        adjustSetIntervalDigit(key, &newInterval, &cursorPos);
      }
      if (state == SET_INTERVAL) {
        updateSetIntervalLCD(cursorPos, newInterval);
      }
    } 
  }
}

void setSoakTimeLoop() {
  tmElements_t newSoakTime;
  newSoakTime.Hour = soakTime.Hour;
  newSoakTime.Minute = soakTime.Minute;
  char key;
  uint8_t cursorPos = 0; // Holds time-setting position of cursor for "00 00" (Hour Min)
                         // Does not include spacing/colons/hyphens, ranges 0 to 3.

  resetLcd();
  initSetSoakOrDryLCD();
  updateSetSoakOrDryLCD(cursorPos, newSoakTime);
  lcd.blink();

  while (state == SET_SOAK_TIME) {
    key = getKeyDebounce();
    
    if (key != NULL) {
      if (key == 'S') {
        //breakTime(makeTime(newInterval), newInterval);
        soakTime.Hour = newSoakTime.Hour;
        soakTime.Minute = newSoakTime.Minute;
        lcd.noBlink();
        state = SETTINGS;
      }

      else if (key == 'L' && cursorPos > 0) {
        cursorPos--;
      }

      else if (key == 'R' && cursorPos < 3) {
      cursorPos++;
      }

      else if (key == 'U' || key == 'D') {
        adjustSetSoakOrDryDigit(key, &newSoakTime, &cursorPos);
      }
      if (state == SET_SOAK_TIME) {
        updateSetSoakOrDryLCD(cursorPos, newSoakTime);
        }
    } 
  }
}

void setDryTimeLoop() {
  tmElements_t newDryTime;
  newDryTime.Hour = dryTime.Hour;
  newDryTime.Minute = dryTime.Minute;
  char key;
  uint8_t cursorPos = 0; // Holds time-setting position of cursor for "00 00" (Hour Min)
                         // Does not include spacing/colons/hyphens, ranges 0 to 3.

  resetLcd();
  initSetSoakOrDryLCD();
  updateSetSoakOrDryLCD(cursorPos, newDryTime);
  lcd.blink();

  while (state == SET_DRY_TIME) {
    key = getKeyDebounce();
    
    if (key != NULL) {

      if (key == 'S') {
        //breakTime(makeTime(newInterval), newInterval);
        dryTime.Hour = newDryTime.Hour;
        dryTime.Minute = newDryTime.Minute;
        lcd.noBlink();
        state = SETTINGS;
      }

      else if (key == 'L' && cursorPos > 0) {
        cursorPos--;
      }

      else if (key == 'R' && cursorPos < 3) {
      cursorPos++;
      }

      else if (key == 'U' || key == 'D') {
        adjustSetSoakOrDryDigit(key, &newDryTime, &cursorPos);
      }

      if (state == SET_DRY_TIME) {
      updateSetSoakOrDryLCD(cursorPos, newDryTime);
      }
    } 
  }
}

void setBrightnessLoop() {
  char key;

  resetLcd();
  initSetBrightnessOrConstrastLCD();

  while (state == SET_BRIGHTNESS) {
    key = getKeyDebounce();
    
    if (key != NULL) {

      if (key == 'S') {
        state = SETTINGS;
      }

      else if (key == 'L' && screenBrightness > 1) {
        updateBrightnessOrContrastLCD(false);
      }

      else if (key == 'R' && screenBrightness < 20) {
        updateBrightnessOrContrastLCD(true);
      }
    } 
  }
}

void calibrateLoop() {
  resetLcd();
  lcd.setCursor(0, 0);
  lcd.print("Calibrating...");
  delay(5000);
  state = STANDBY;
}

void resetLcd()
{
  lcd.clear();
  cursorY = 0;
}