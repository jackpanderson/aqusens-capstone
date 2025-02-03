// SETTINGS
// 4 page selection options
//    - INTERVAL/START TIME/CLOCK
//    - SOAK/FLUSH/DRY TIME
//    - ADD EVENT/VIEW EVENTS/FILTER STATUS
//    - BRIGHTNESS/CONTRAST
void settingsLoop() {
  uint8_t lastKeyPress = rtc.getMinutes();
  char keyPressed;
  resetLCD();
  
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
        resetLCD();
        state = STANDBY;
        cursorY = 2;
      }
      else { // else, returns to previous settings page
        resetLCD();
        settingsPage--;
      }
    }

    else if (keyPressed == 'R' && settingsPage != lastSettingPage) { //right
      resetLCD();
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
            state = SET_TUBE_FLUSH_TIME;
            break;
          
          case 2:
            state = SET_AQUSENS_FLUSH_TIME;
            break;
          
          default:
            break;
        }
      }

      else if (settingsPage == 3) {
        switch(cursorY) {
          case 0:
            state = SET_DRY_TIME;
            break;
          
          case 1:
            state = ADD_EVENT;
            break;
          
          case 2:
            state = VIEW_EVENTS;
            break;
          
          default:
            break;
        }
      }

      else if (settingsPage == 4) {
        switch(cursorY) {
          case 0:
            state = FILTER_STATUS;
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

  resetLCD();
  initSetClockLCD();
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

void setClockLoop() {

  //Serial.println(rtc.getYear());

  tmElements_t adjustedTime;
  adjustedTime.Month = rtc.getMonth();
  adjustedTime.Day = rtc.getDay();
  adjustedTime.Year = rtc.getYear(); 
  adjustedTime.Hour = rtc.getHours();
  adjustedTime.Minute = rtc.getMinutes();
  char key;
  uint8_t cursorPos = 0; // Holds time-setting position of cursor for "00-00-00 00:00"
                         // Does not include spacing/colons/hyphens, ranges 0 to 9.

  resetLCD();
  initSetClockLCD();
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

  resetLCD();
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
  newSoakTime.Minute = soakTime.Minute;
  newSoakTime.Second = soakTime.Second;
  char key;
  uint8_t cursorPos = 0; // Holds time-setting position of cursor for "00 00" (Hour Min)
                         // Does not include spacing/colons/hyphens, ranges 0 to 3.

  resetLCD();
  initSetSoakOrDryOrFlushLCD();
  updateSetSoakOrDryOrFlushLCD(cursorPos, newSoakTime);
  lcd.blink();

  while (state == SET_SOAK_TIME) {
    key = getKeyDebounce();
    
    if (key != NULL) {
      if (key == 'S') {
        //breakTime(makeTime(newInterval), newInterval);
        soakTime.Second = newSoakTime.Second;
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
        updateSetSoakOrDryOrFlushLCD(cursorPos, newSoakTime);
        }
    } 
  }
}

void setDryTimeLoop() {
  tmElements_t newDryTime;
  newDryTime.Minute = dryTime.Minute;
  newDryTime.Second = dryTime.Second;
  char key;
  uint8_t cursorPos = 0; // Holds time-setting position of cursor for "00 00" (Hour Min)
                         // Does not include spacing/colons/hyphens, ranges 0 to 3.

  resetLCD();
  initSetSoakOrDryOrFlushLCD();
  updateSetSoakOrDryOrFlushLCD(cursorPos, newDryTime);
  lcd.blink();

  while (state == SET_DRY_TIME) {
    key = getKeyDebounce();
    
    if (key != NULL) {

      if (key == 'S') {
        //breakTime(makeTime(newInterval), newInterval);
        dryTime.Second = newDryTime.Second;
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
      updateSetSoakOrDryOrFlushLCD(cursorPos, newDryTime);
      }
    } 
  }
}

void setTubeFlushTimeLoop() {
  tmElements_t newTubeFlushTime;
  newTubeFlushTime.Second = tubeFlushTime.Second;
  newTubeFlushTime.Minute = tubeFlushTime.Minute;
  char key;
  uint8_t cursorPos = 0; // Holds time-setting position of cursor for "00 00" (Hour Min)
                         // Does not include spacing/colons/hyphens, ranges 0 to 3.

  resetLCD();
  initSetSoakOrDryOrFlushLCD();
  updateSetSoakOrDryOrFlushLCD(cursorPos, newTubeFlushTime);
  lcd.blink();

  while (state == SET_TUBE_FLUSH_TIME) {
    key = getKeyDebounce();
    
    if (key != NULL) {

      if (key == 'S') {
        tubeFlushTime.Second = newTubeFlushTime.Second;
        tubeFlushTime.Minute = newTubeFlushTime.Minute;
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
        adjustSetSoakOrDryDigit(key, &newTubeFlushTime, &cursorPos);
      }

      if (state == SET_TUBE_FLUSH_TIME) {
      updateSetSoakOrDryOrFlushLCD(cursorPos, newTubeFlushTime);
      }
    } 
  }
}

void setAqusensFlushTimeLoop() {
  tmElements_t newAqusensFlushTime;
  newAqusensFlushTime.Second = aqusensFlushTime.Second;
  newAqusensFlushTime.Minute = aqusensFlushTime.Minute;
  char key;
  uint8_t cursorPos = 0; // Holds time-setting position of cursor for "00 00" (Hour Min)
                         // Does not include spacing/colons/hyphens, ranges 0 to 3.

  resetLCD();
  initSetSoakOrDryOrFlushLCD();
  updateSetSoakOrDryOrFlushLCD(cursorPos, newAqusensFlushTime);
  lcd.blink();

  while (state == SET_AQUSENS_FLUSH_TIME) {
    key = getKeyDebounce();
    
    if (key != NULL) {

      if (key == 'S') {
        aqusensFlushTime.Second = newAqusensFlushTime.Second;
        aqusensFlushTime.Minute = newAqusensFlushTime.Minute;
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
        adjustSetSoakOrDryDigit(key, &newAqusensFlushTime, &cursorPos);
      }

      if (state == SET_TUBE_FLUSH_TIME) {
      updateSetSoakOrDryOrFlushLCD(cursorPos, newAqusensFlushTime);
      }
    } 
  }
}

void setBrightnessLoop() {
  char key;

  resetLCD();
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