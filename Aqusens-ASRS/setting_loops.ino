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
  uint8_t last_key_press = rtc.getMinutes();
  char key_pressed;
  resetLCD();
  
  while (state == SETTINGS) {
    settingsLCD(settings_page); //Launch into first page of settings

    if (settings_page != last_setting_page)
      key_pressed = cursorSelect(0, 2);
    else
      key_pressed = cursorSelect(0, 0);

    if (key_pressed > 0) {
      last_key_press = rtc.getMinutes();
    }

    if (key_pressed == 'L') { // left
      if (settings_page == 1) { // exits to STANDBY mode on first page left select
        resetLCD();
        state = STANDBY;
        export_cfg_to_sd();
        // TODO: save config before leaving
        cursor_y = 2;
      }
      else { // else, returns to previous settings page
        resetLCD();
        settings_page--;
      }
    }

    else if (key_pressed == 'R' && settings_page != last_setting_page) { //right
      resetLCD();
      settings_page++;
    }

    else if (key_pressed == 'S') {
      if (settings_page == 1) {
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

      else if (settings_page == 2) {
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

      else if (settings_page == 3) {
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

      else if (settings_page == 4) {
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

    if (rtc.getMinutes() == ((last_key_press + 5) % 60)) {
      // TODO: save cfg before leaving
      export_cfg_to_sd();
      state = STANDBY;
    }
  }
}

/**
 * @brief SET_START_TIME
 * 
 */
void setStartTimeLoop() {
  tmElements_t adjusted_start_time;
  adjusted_start_time.Month = rtc.getMonth();
  adjusted_start_time.Day = rtc.getDay();
  adjusted_start_time.Year = rtc.getYear(); 
  adjusted_start_time.Hour = rtc.getHours();
  adjusted_start_time.Minute = rtc.getMinutes();
  char key;
  uint8_t cursor_pos = 0; // Holds time-setting position of cursor for "00-00-00 00:00"
                         // Does not include spacing/colons/hyphens, ranges 0 to 9.

  resetLCD();
  initSetClockLCD();
  updateSetClockLCD(cursor_pos, adjusted_start_time);
  lcd.blink();

  while (state == SET_START_TIME) {
    key = getKeyDebounce();
    
    if (key != NULL) {
      if (key == 'S') {
        updateAlarm(adjusted_start_time);
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
        adjustSetTimeDigit(key, &adjusted_start_time, &cursor_pos);
      }
      if (state == SET_START_TIME) {
        updateSetClockLCD(cursor_pos, adjusted_start_time);
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
  tmElements_t new_soak_time;
  new_soak_time.Minute = soak_time.Minute;
  new_soak_time.Second = soak_time.Second;
  char key;
  uint8_t cursor_pos = 0; // Holds time-setting position of cursor for "00 00" (Hour Min)
                         // Does not include spacing/colons/hyphens, ranges 0 to 3.

  resetLCD();
  initSetSoakOrDryOrFlushLCD();
  updateSetSoakOrDryOrFlushLCD(cursor_pos, new_soak_time);
  lcd.blink();

  TimeUnit_t& soak_times_cfg = getGlobalCfg().times_cfg.soak_time;

  while (state == SET_SOAK_TIME) {
    key = getKeyDebounce();
    
    if (key != NULL) {
      if (key == 'S') {
        //breakTime(makeTime(new_interval), new_interval);
        soak_time.Second = new_soak_time.Second;
        soak_time.Minute = new_soak_time.Minute;
        
        soak_times_cfg.min = new_soak_time.Minute;
        soak_times_cfg.sec = new_soak_time.Second;

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
        adjustSetSoakOrDryDigit(key, &new_soak_time, &cursor_pos);
      }
      if (state == SET_SOAK_TIME) {
        updateSetSoakOrDryOrFlushLCD(cursor_pos, new_soak_time);
        }
    } 
  }
}

/**
 * @brief SET_DRY_TIME
 * 
 */
void setDryTimeLoop() {
  tmElements_t new_dry_time;
  new_dry_time.Minute = dry_time.Minute;
  new_dry_time.Second = dry_time.Second;
  char key;
  uint8_t cursor_pos = 0; // Holds time-setting position of cursor for "00 00" (Hour Min)
                         // Does not include spacing/colons/hyphens, ranges 0 to 3.

  resetLCD();
  initSetSoakOrDryOrFlushLCD();
  updateSetSoakOrDryOrFlushLCD(cursor_pos, new_dry_time);
  lcd.blink();

  TimeUnit_t& dry_times_cfg = getGlobalCfg().times_cfg.dry_time;


  while (state == SET_DRY_TIME) {
    key = getKeyDebounce();
    
    if (key != NULL) {

      if (key == 'S') {
        //breakTime(makeTime(new_interval), new_interval);
        dry_time.Second = new_dry_time.Second;
        dry_time.Minute = new_dry_time.Minute;

        dry_times_cfg.min = new_dry_time.Minute;
        dry_times_cfg.sec = new_dry_time.Second;


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
        adjustSetSoakOrDryDigit(key, &new_dry_time, &cursor_pos);
      }

      if (state == SET_DRY_TIME) {
      updateSetSoakOrDryOrFlushLCD(cursor_pos, new_dry_time);
      }
    } 
  }
}

/**
 * @brief SET_TUBE_FLUSH_TIME
 * 
 */
void setTubeFlushTimeLoop() {
  tmElements_t new_tube_flush_time;
  new_tube_flush_time.Second = tube_flush_time.Second;
  new_tube_flush_time.Minute = tube_flush_time.Minute;
  char key;
  uint8_t cursor_pos = 0; // Holds time-setting position of cursor for "00 00" (Hour Min)
                         // Does not include spacing/colons/hyphens, ranges 0 to 3.

  resetLCD();
  initSetSoakOrDryOrFlushLCD();
  updateSetSoakOrDryOrFlushLCD(cursor_pos, new_tube_flush_time);
  lcd.blink();

  while (state == SET_TUBE_FLUSH_TIME) {
    key = getKeyDebounce();
    
    if (key != NULL) {

      if (key == 'S') {
        tube_flush_time.Second = new_tube_flush_time.Second;
        tube_flush_time.Minute = new_tube_flush_time.Minute;
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
        adjustSetSoakOrDryDigit(key, &new_tube_flush_time, &cursor_pos);
      }

      if (state == SET_TUBE_FLUSH_TIME) {
      updateSetSoakOrDryOrFlushLCD(cursor_pos, new_tube_flush_time);
      }
    } 
  }
}

/**
 * @brief SET_AQUSENS_FLUSH_TIME
 * 
 */
void setAqusensFlushTimeLoop() {
  tmElements_t new_aqusens_flush_time;
  new_aqusens_flush_time.Second = aqusens_flush_time.Second;
  new_aqusens_flush_time.Minute = aqusens_flush_time.Minute;
  char key;
  uint8_t cursor_pos = 0; // Holds time-setting position of cursor for "00 00" (Hour Min)
                         // Does not include spacing/colons/hyphens, ranges 0 to 3.

  resetLCD();
  initSetSoakOrDryOrFlushLCD();
  updateSetSoakOrDryOrFlushLCD(cursor_pos, new_aqusens_flush_time);
  lcd.blink();

  while (state == SET_AQUSENS_FLUSH_TIME) {
    key = getKeyDebounce();
    
    if (key != NULL) {

      if (key == 'S') {
        aqusens_flush_time.Second = new_aqusens_flush_time.Second;
        aqusens_flush_time.Minute = new_aqusens_flush_time.Minute;
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
        adjustSetSoakOrDryDigit(key, &new_aqusens_flush_time, &cursor_pos);
      }

      if (state == SET_AQUSENS_FLUSH_TIME) {
      updateSetSoakOrDryOrFlushLCD(cursor_pos, new_aqusens_flush_time);
      }

      Serial.print(key);
      Serial.println(cursor_pos);
    
    } 
  }
}