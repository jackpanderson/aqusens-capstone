/**
 * @brief SETTINGS
 * 
 * Four page options:
 *    - INTERVAL/START TIME/CLOCK
 *    - SOAK/FLUSH/DRY TIME
 *    - ADD EVENT/VIEW EVENTS/FILTER STATUS
 *    - BRIGHTNESS/CONTRAST
 */
void settingsLoop() {
  uint8_t lastKeyPress = rtc.getMinutes();
  char keyPressed;
  resetLCD();
  
  while (state == SETTINGS) {
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
        cursor_y = 2;
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
        switch(cursor_y) {
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
        switch(cursor_y) { //add more states
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
        switch(cursor_y) {
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
        switch(cursor_y) {
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

/**
 * @brief SET_START_TIME
 * 
 */
void setStartTimeLoop() {
  tmElements_t adjustedStartTime;
  adjustedStartTime.Month = rtc.getMonth();
  adjustedStartTime.Day = rtc.getDay();
  adjustedStartTime.Year = rtc.getYear(); 
  adjustedStartTime.Hour = rtc.getHours();
  adjustedStartTime.Minute = rtc.getMinutes();
  char key;
  uint8_t cursor_pos = 0; // Holds time-setting position of cursor for "00-00-00 00:00"
                         // Does not include spacing/colons/hyphens, ranges 0 to 9.

  resetLCD();
  initSetClockLCD();
  updateSetClockLCD(cursor_pos, adjustedStartTime);
  lcd.blink();

  while (state == SET_START_TIME) {
    key = getKeyDebounce();
    
    if (key != NULL) {
      if (key == 'S') {
        updateAlarm(adjustedStartTime);
        lcd.noBlink();
        state = SETTINGS;
      }

      else if (key == 'L' && cursor_pos > 0) {
        cursor_pos--;
      }

      else if (key == 'R' && cursor_pos < 9) {
      cursor_pos++;
      }

      else if (key == 'U' || key == 'D') {
        adjustSetTimeDigit(key, &adjustedStartTime, &cursor_pos);
      }
      if (state == SET_START_TIME) {
        updateSetClockLCD(cursor_pos, adjustedStartTime);
      }
    } 
  }

}

/**
 * @brief SET_CLOCK
 * 
 */
void setClockLoop() {

  tmElements_t adjusted_time;
  adjusted_time.Month = rtc.getMonth();
  adjusted_time.Day = rtc.getDay();
  adjusted_time.Year = rtc.getYear(); 
  adjusted_time.Hour = rtc.getHours();
  adjusted_time.Minute = rtc.getMinutes();
  char key;
  uint8_t cursor_pos = 0; // Holds time-setting position of cursor for "00-00-00 00:00"
                         // Does not include spacing/colons/hyphens, ranges 0 to 9.

  resetLCD();
  initSetClockLCD();
  updateSetClockLCD(cursor_pos, adjusted_time);
  lcd.blink();

  while (state == SET_CLOCK) {
    key = getKeyDebounce();
    
    if (key != NULL) {
      if (key == 'S') {
        breakTime(makeTime(adjusted_time), adjusted_time);
        rtc.setMonth(adjusted_time.Month);
        rtc.setDay(adjusted_time.Day);
        rtc.setYear(adjusted_time.Year);
        rtc.setHours(adjusted_time.Hour);
        rtc.setMinutes(adjusted_time.Minute);
        updateAlarm();
        lcd.noBlink();
        state = SETTINGS;
      }

      else if (key == 'L' && cursor_pos > 0) {
        cursor_pos--;
      }

      else if (key == 'R' && cursor_pos < 9) {
        cursor_pos++;
      }

      else if (key == 'U' || key == 'D') {
        adjustSetTimeDigit(key, &adjusted_time, &cursor_pos);
      }
      if (state == SET_CLOCK) {
        updateSetClockLCD(cursor_pos, adjusted_time);
      }
    } 
  }
}

/**
 * @brief SET_INTERVAL
 * 
 */
void setIntervalLoop() {
  tmElements_t new_interval;
  new_interval.Day = sample_interval.Day;
  new_interval.Hour = sample_interval.Hour;
  new_interval.Minute = sample_interval.Minute;
  char key;
  uint8_t cursor_pos = 0; // Holds time-setting position of cursor for "00 00 00" (Day Hour Min)
                         // Does not include spacing/colons/hyphens, ranges 0 to 5.

  resetLCD();
  initSetIntervalLCD();
  updateSetIntervalLCD(cursor_pos, new_interval);
  lcd.blink();

  while (state == SET_INTERVAL) {
    key = getKeyDebounce();
    
    if (key != NULL) {
      if (key == 'S') {
        sample_interval.Day = new_interval.Day;
        sample_interval.Hour = new_interval.Hour;
        sample_interval.Minute = new_interval.Minute;
        updateAlarm();
        lcd.noBlink();
        state = SETTINGS;
      }

      else if (key == 'L' && cursor_pos > 0) {
        cursor_pos--;
      }

      else if (key == 'R' && cursor_pos < 5) {
      cursor_pos++;
      }

      else if (key == 'U' || key == 'D') {
        adjustSetIntervalDigit(key, &new_interval, &cursor_pos);
      }
      if (state == SET_INTERVAL) {
        updateSetIntervalLCD(cursor_pos, new_interval);
      }
    } 
  }
}

/**
 * @brief SET_SOAK_TIME
 * 
 */
void setSoakTimeLoop() {
  tmElements_t newSoakTime;
  newSoakTime.Minute = soak_time.Minute;
  newSoakTime.Second = soak_time.Second;
  char key;
  uint8_t cursor_pos = 0; // Holds time-setting position of cursor for "00 00" (Hour Min)
                         // Does not include spacing/colons/hyphens, ranges 0 to 3.

  resetLCD();
  initSetSoakOrDryOrFlushLCD();
  updateSetSoakOrDryOrFlushLCD(cursor_pos, newSoakTime);
  lcd.blink();

  while (state == SET_SOAK_TIME) {
    key = getKeyDebounce();
    
    if (key != NULL) {
      if (key == 'S') {
        //breakTime(makeTime(new_interval), new_interval);
        soak_time.Second = newSoakTime.Second;
        soak_time.Minute = newSoakTime.Minute;
        lcd.noBlink();
        state = SETTINGS;
      }

      else if (key == 'L' && cursor_pos > 0) {
        cursor_pos--;
      }

      else if (key == 'R' && cursor_pos < 3) {
      cursor_pos++;
      }

      else if (key == 'U' || key == 'D') {
        adjustSetSoakOrDryDigit(key, &newSoakTime, &cursor_pos);
      }
      if (state == SET_SOAK_TIME) {
        updateSetSoakOrDryOrFlushLCD(cursor_pos, newSoakTime);
        }
    } 
  }
}

/**
 * @brief SET_DRY_TIME
 * 
 */
void setDryTimeLoop() {
  tmElements_t newDryTime;
  newDryTime.Minute = dryTime.Minute;
  newDryTime.Second = dryTime.Second;
  char key;
  uint8_t cursor_pos = 0; // Holds time-setting position of cursor for "00 00" (Hour Min)
                         // Does not include spacing/colons/hyphens, ranges 0 to 3.

  resetLCD();
  initSetSoakOrDryOrFlushLCD();
  updateSetSoakOrDryOrFlushLCD(cursor_pos, newDryTime);
  lcd.blink();

  while (state == SET_DRY_TIME) {
    key = getKeyDebounce();
    
    if (key != NULL) {

      if (key == 'S') {
        //breakTime(makeTime(new_interval), new_interval);
        dryTime.Second = newDryTime.Second;
        dryTime.Minute = newDryTime.Minute;
        lcd.noBlink();
        state = SETTINGS;
      }

      else if (key == 'L' && cursor_pos > 0) {
        cursor_pos--;
      }

      else if (key == 'R' && cursor_pos < 3) {
      cursor_pos++;
      }

      else if (key == 'U' || key == 'D') {
        adjustSetSoakOrDryDigit(key, &newDryTime, &cursor_pos);
      }

      if (state == SET_DRY_TIME) {
      updateSetSoakOrDryOrFlushLCD(cursor_pos, newDryTime);
      }
    } 
  }
}

/**
 * @brief SET_TUBE_FLUSH_TIME
 * 
 */
void setTubeFlushTimeLoop() {
  tmElements_t newTubeFlushTime;
  newTubeFlushTime.Second = tube_flush_time.Second;
  newTubeFlushTime.Minute = tube_flush_time.Minute;
  char key;
  uint8_t cursor_pos = 0; // Holds time-setting position of cursor for "00 00" (Hour Min)
                         // Does not include spacing/colons/hyphens, ranges 0 to 3.

  resetLCD();
  initSetSoakOrDryOrFlushLCD();
  updateSetSoakOrDryOrFlushLCD(cursor_pos, newTubeFlushTime);
  lcd.blink();

  while (state == SET_TUBE_FLUSH_TIME) {
    key = getKeyDebounce();
    
    if (key != NULL) {

      if (key == 'S') {
        tube_flush_time.Second = newTubeFlushTime.Second;
        tube_flush_time.Minute = newTubeFlushTime.Minute;
        lcd.noBlink();
        state = SETTINGS;
      }

      else if (key == 'L' && cursor_pos > 0) {
        cursor_pos--;
      }

      else if (key == 'R' && cursor_pos < 3) {
        cursor_pos++;
      }

      else if (key == 'U' || key == 'D') {
        adjustSetSoakOrDryDigit(key, &newTubeFlushTime, &cursor_pos);
      }

      if (state == SET_TUBE_FLUSH_TIME) {
      updateSetSoakOrDryOrFlushLCD(cursor_pos, newTubeFlushTime);
      }
    } 
  }
}

/**
 * @brief SET_AQUSENS_FLUSH_TIME
 * 
 */
void setAqusensFlushTimeLoop() {
  tmElements_t newAqusensFlushTime;
  newAqusensFlushTime.Second = aqusens_flush_time.Second;
  newAqusensFlushTime.Minute = aqusens_flush_time.Minute;
  char key;
  uint8_t cursor_pos = 0; // Holds time-setting position of cursor for "00 00" (Hour Min)
                         // Does not include spacing/colons/hyphens, ranges 0 to 3.

  resetLCD();
  initSetSoakOrDryOrFlushLCD();
  updateSetSoakOrDryOrFlushLCD(cursor_pos, newAqusensFlushTime);
  lcd.blink();

  while (state == SET_AQUSENS_FLUSH_TIME) {
    key = getKeyDebounce();
    
    if (key != NULL) {

      if (key == 'S') {
        aqusens_flush_time.Second = newAqusensFlushTime.Second;
        aqusens_flush_time.Minute = newAqusensFlushTime.Minute;
        lcd.noBlink();
        state = SETTINGS;
      }

      else if (key == 'L' && cursor_pos > 0) {
        cursor_pos--;
      }

      else if (key == 'R' && cursor_pos < 3) {
        cursor_pos++;
      }

      else if (key == 'U' || key == 'D') {
        adjustSetSoakOrDryDigit(key, &newAqusensFlushTime, &cursor_pos);
      }

      if (state == SET_AQUSENS_FLUSH_TIME) {
      updateSetSoakOrDryOrFlushLCD(cursor_pos, newAqusensFlushTime);
      }

      Serial.print(key);
      Serial.println(cursor_pos);
    
    } 
  }
}

/**
 * @brief SET_BRIGHTNESS
 * 
 */
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