#define TIME_BASED_DEBOUNCE_WAIT_TIME_MS 35
#define PRESS_AND_HOLD_INTERVAL_MS 25

/* Init Functions **************************************************************/

/**
 * @brief initializes all pins on gpio module
 * 
 */
void gpioInit() {
  /* Initialize Stepper PWM */
  pinMode(KEY_U, INPUT_PULLDOWN);
  pinMode(KEY_D, INPUT_PULLDOWN);
  pinMode(KEY_L, INPUT_PULLDOWN);
  pinMode(KEY_R, INPUT_PULLDOWN);
  pinMode(KEY_S, INPUT_PULLDOWN);
  pinMode(DIR_POS_PIN, OUTPUT);
  pinMode(A1, INPUT_PULLDOWN);
  pinMode(A2, INPUT_PULLDOWN);
  pinMode(ALARM_MINUS, OUTPUT);
  pinMode(ALARM_PLUS, INPUT_PULLDOWN);

  /* SD */
  SD.begin(SD_CS);
  digitalWrite(ALARM_MINUS, HIGH);
}

/**
 * @brief initializes E-stop pin as pull-down and interrupt
 * 
 */
void estopInit() {
  pinMode(A1, INPUT_PULLDOWN);

  attachInterrupt(digitalPinToInterrupt(A1), onLowTrigger, FALLING);
}

/**
 * @brief switches relay for given solenoid
 * 
 * @param state state of current solenoid (OPEN/CLOSED)
 * @param solenoid_number solenoid to control (SOLENOID_ONE/SOLENOID_TWO)
 */
void updateSolenoid(solenoidState state, int solenoid_number) {
  if (solenoid_number == SOLENOID_ONE) {
      if (state == OPEN) {
        solenoidOneState = OPEN;
        P1.writeDiscrete(true, RELAY_SLOT, solenoid_number);
      } else {
        solenoidOneState = CLOSED;
        P1.writeDiscrete(false, RELAY_SLOT, solenoid_number);
      }
  } else if (solenoid_number == SOLENOID_TWO) {
      if (state == OPEN) {
        solenoidTwoState = OPEN;
        P1.writeDiscrete(true, RELAY_SLOT, solenoid_number);
      } else {
        solenoidTwoState = CLOSED;
        P1.writeDiscrete(false, RELAY_SLOT, solenoid_number);
      }
  }
}

/**
 * @brief interrupt function for E-Stop
 * 
 */
void onLowTrigger() {
  state = ESTOP_ALARM;
}

/**
 * @brief initializes the RTC
 * 
 */
void rtcInit() {
  rtc.begin();
  rtc.setEpoch(1738123553);
  sample_interval.Year = 0;
  sample_interval.Month = 0;
  sample_interval.Day = 0;
  sample_interval.Hour = 0;
  sample_interval.Minute = 15;  
  sample_interval.Second = 0;

  next_sample_time.Year = rtc.getYear() + sample_interval.Year;
  next_sample_time.Month = rtc.getMonth() + sample_interval.Month;
  next_sample_time.Day = rtc.getDay() + sample_interval.Day;
  next_sample_time.Hour = rtc.getHours() + sample_interval.Hour;
  next_sample_time.Minute = rtc.getMinutes() + sample_interval.Minute;

  soak_time.Minute = 0;
  soak_time.Second = 5;

  // dryTime.Minute = 20;
  dryTime.Minute = 0;
  dryTime.Second = 5;

  tube_flush_time.Minute = 2;
  tube_flush_time.Second = 45;

  aqusens_flush_time.Minute = 0;
  aqusens_flush_time.Second = 15;
  
  updateAlarm();
}

/**
 * @brief update timer alarm for given time
 * 
 * @param delayTime next sample time
 */
void updateAlarm(tmElements_t delayTime) {

  breakTime(makeTime(next_sample_time), next_sample_time);

  next_sample_time.Year = delayTime.Year;
  next_sample_time.Month = delayTime.Month;
  next_sample_time.Day = delayTime.Day;
  next_sample_time.Hour = delayTime.Hour;
  next_sample_time.Minute = delayTime.Minute;


  
  rtc.setAlarmTime(next_sample_time.Hour, next_sample_time.Minute, 0); // Set alarm for the specified time
  rtc.setAlarmDate(next_sample_time.Day, next_sample_time.Month, next_sample_time.Year);
  rtc.enableAlarm(rtc.MATCH_YYMMDDHHMMSS); // Match hours and minutes
  rtc.attachInterrupt(alarmTriggered); // Attach the ISR for the alarm interrupt
}

/**
 * @brief update timer alarm for next sample interval
 * 
 */
void updateAlarm() {
  next_sample_time.Year = rtc.getYear() + sample_interval.Year;
  next_sample_time.Month = rtc.getMonth() + sample_interval.Month;
  next_sample_time.Day = rtc.getDay() + sample_interval.Day;
  next_sample_time.Hour = rtc.getHours() + sample_interval.Hour;
  next_sample_time.Minute = rtc.getMinutes() + sample_interval.Minute;


  breakTime(makeTime(next_sample_time), next_sample_time);
  
  rtc.setAlarmTime(next_sample_time.Hour, next_sample_time.Minute, 0); // Set alarm for the specified time
  rtc.setAlarmDate(next_sample_time.Day, next_sample_time.Month, next_sample_time.Year);
  rtc.enableAlarm(rtc.MATCH_YYMMDDHHMMSS); // Match hours and minutes
  rtc.attachInterrupt(alarmTriggered); // Attach the ISR for the alarm interrupt
}

/**
 * @brief interrupt function for alarm interrupt
 * 
 */
void alarmTriggered() {
  // This will be called when the alarm is triggered
  // You can handle additional tasks here if needed
  next_sample_time.Year = rtc.getYear() + sample_interval.Year;
  next_sample_time.Month = rtc.getMonth() + sample_interval.Month;
  next_sample_time.Day = rtc.getDay() + sample_interval.Day;
  next_sample_time.Hour = rtc.getHours() + sample_interval.Hour;
  next_sample_time.Minute = rtc.getMinutes() + sample_interval.Minute;
  
  breakTime(makeTime(next_sample_time), next_sample_time);

  rtc.setAlarmTime(next_sample_time.Hour, next_sample_time.Minute, 0); // Set alarm for the specified time
  rtc.setAlarmDate(next_sample_time.Day, next_sample_time.Month, next_sample_time.Year);

  if (state == STANDBY) {
    state = RELEASE;
  }
}

/**
 * @brief controls row moving with cursor
 * 
 * @param begin first row available to traverse
 * @param end last row available to traverse
 * @return char key that was pressed
 */
char cursorSelect(int begin, int end) 
{
  char key = getKeyDebounce();
  
  if (key == 'U' && cursor_y > begin) {  // Check if back button has been pressed: state = menu
    lcd.setCursor(0, cursor_y);
    lcd.print(" ");
    cursor_y--;
    return 'U';
  } 
  else if (key == 'D' && cursor_y < end) {
    lcd.setCursor(0, cursor_y);
    lcd.print(" ");
    cursor_y++;
    return 'D';
  }

  else {
    return key;
  }
}

/**
 * @brief gets key press from user (debounced)
 * 
 * @return char key pressed by the user
 */
char getKeyDebounce() {

  char key = getKeyPress();
  //Serial.println(key);

  if (key != NULL) {
    while (getKeyPress() == key) {;}
  }


  return key;
}

/**
 * @brief Time based debounce function for press-and-hold
 * 
 * @return char key pressed by the user
 */
char getKeyTimeBasedDebounce() {
  char key = getKeyPress();

  unsigned long start_time = millis();
  unsigned long curr_time = millis();

  if (key == 'U' || key == 'D') {
    while (curr_time - start_time < TIME_BASED_DEBOUNCE_WAIT_TIME_MS) {
      curr_time = millis();
    }

    if (getKeyPress() == key) {
      return key;
    }

    else {
      return NULL;
    }
  }

  return getKeyDebounce();
}

/**
 * @brief formats the current date/time
 * 
 * @return String formatted string of current date/time
 */
String getCurrentDateTime() {
  char date_time_string[16];

  snprintf(date_time_string, sizeof(date_time_string), "%02d-%02d-%02d %02d:%02d",
           rtc.getMonth(),
           rtc.getDay(),
           rtc.getYear(),
           rtc.getHours(),
           rtc.getMinutes());

  return String(date_time_string);
}

/**
 * @brief formats the next sample date/time
 * 
 * @return String formatted string of next sample date/time
 */
String getNextSampleTime() {
  char date_time_string[16];

  snprintf(date_time_string, sizeof(date_time_string), "%02d-%02d-%02d %02d:%02d",
           next_sample_time.Month,
           next_sample_time.Day,
           next_sample_time.Year,
           next_sample_time.Hour,
           next_sample_time.Minute);

  return String(date_time_string);
}

/**
 * @brief parses time string and formats to tmElements_t
 * 
 * @param time_str formatted time string
 * @return tmElements_t object containing time value from time_str
 */
tmElements_t parseTime(const char* time_str) {
  int year, month, day, hour, minute, second;
  sscanf(time_str, "%4d-%2d-%2d %2d:%2d:%2d", &year, &month, &day, &hour, &minute, &second);

  tmElements_t tm;
  tm.Year = year;  // Arduino time library uses years since 1970
  tm.Month = month;
  tm.Day = day;
  tm.Hour = hour;
  tm.Minute = minute;
  tm.Second = second;

  return tm;
}

/**
 * @brief gets any key currently pressed by user
 * 
 * @return char key pressed by user (NULL if no current key press)
 */
char getKeyPress() {
  if (digitalRead(KEY_U)) {
    return 'U';
  } else if (digitalRead(KEY_D)) {
    return 'D';
  } else if (digitalRead(KEY_L)) {
    return 'L';
  } else if (digitalRead(KEY_R)) {
    return 'R';
  } else if (digitalRead(KEY_S)) {
    return 'S';
  } else {
    return NULL;
  }
}

/**
 * @brief controls incrementing and decrementing digits on time setting pages
 * 
 * @param key key pressed by user
 * @param adjusted_time pointer to tmElements_t object that is being modified
 * @param cursor_pos current position of cursor
 */
void adjustSetTimeDigit(char key, tmElements_t* adjusted_time, uint8_t* cursor_pos) {
  switch (*cursor_pos) {
    case 0: //Month Tens Place
      if (key == 'U' && (adjusted_time -> Month) < 10) {
        (adjusted_time -> Month) += 10;
      }
      else if (key == 'D' && (adjusted_time -> Month) > 10) {
        (adjusted_time -> Month) -= 10;
      }
      break;
    
    case 1: //Month Ones Place
      if (key == 'U' && (adjusted_time -> Month) < 12) {
        (adjusted_time -> Month) += 1;
      }
      else if (key == 'D' && (adjusted_time -> Month) > 1) {
        (adjusted_time -> Month) -= 1;
      }
      break;
    
    case 2: //Day Tens Place
      if (key == 'U' && (adjusted_time -> Day) <= 21) { //Max 31, only true if we can validly add 10 to the day
        (adjusted_time -> Day) += 10;
      }
      else if (key == 'D' && (adjusted_time -> Day) >= 11) { //Min 1, only can deduct 10 if greater than 10
        (adjusted_time -> Day) -= 10;
      }
      break;
    
    case 3: //Day Ones Place
      if (key == 'U' && (adjusted_time -> Day) < 31) {
        (adjusted_time -> Day) += 1;
      }
      else if (key == 'D' && (adjusted_time -> Day) > 1) {
        (adjusted_time -> Day) -= 1;
      }
      break;
    
    case 4: //Year Tens Place
      if (key == 'U' && (adjusted_time -> Year) <= 119) { //Max is 2099, 2099-1970 = 129
        (adjusted_time -> Year) += 10;
      }
      else if (key == 'D' && (adjusted_time -> Year) >= 10) {
        (adjusted_time -> Year) -= 10;
      }
      break;
    
    case 5: //Year Ones Place
      if (key == 'U' && (adjusted_time -> Year) < 129) { 
        (adjusted_time -> Year) += 1;
      }
      else if (key == 'D' && (adjusted_time -> Year) > 0) {
        (adjusted_time -> Year) -= 1;
      }
      break;
    
    case 6: //Hours Tens Place, using military/24 hour time
      if (key == 'U' && (adjusted_time -> Hour) <= 14) { //Max 24 
        (adjusted_time -> Hour) += 10;
      }
      else if (key == 'D' && (adjusted_time -> Hour) >= 10) { //Min 0
        (adjusted_time -> Hour) -= 10;
      }
      break;
    
    case 7: //Hours Ones Place
      if (key == 'U' && (adjusted_time -> Hour) < 24) { 
        (adjusted_time -> Hour) += 1;
      }
      else if (key == 'D' && (adjusted_time -> Hour) > 0) {
        (adjusted_time -> Hour) -= 1;
      }
      break;
    
    case 8: //Minutes Tens Place
      if (key == 'U' && (adjusted_time -> Minute) < 49) { //Max 59
        (adjusted_time -> Minute) += 10;
      }
      else if (key == 'D' && (adjusted_time -> Minute) >= 10) { //Min 0
        (adjusted_time -> Minute) -= 10;
      }
      break;
    
    case 9:  //Minutes Ones Place
      if (key == 'U' && (adjusted_time -> Minute) < 59) { 
        (adjusted_time -> Minute) += 1;
      }
      else if (key == 'D' && (adjusted_time -> Minute) > 0) {
        (adjusted_time -> Minute) -= 1;
      }
      break;
    
    default:
      break;
  }
}

/**
 * @brief controls incrementing and decrementing digits on interval setting page
 * 
 * @param key key pressed by user
 * @param new_interval pointer to tmElements_t object that is being modified
 * @param cursor_pos current position of cursor
 */
void adjustSetIntervalDigit(char key, tmElements_t* new_interval, uint8_t* cursor_pos) {
  switch (*cursor_pos) {
    case 0: //Day Tens Place
      if (key == 'U' && (new_interval -> Day) <= 21) { //Max 31, only true if we can validly add 10 to the day
        (new_interval -> Day) += 10;
      }
      else if (key == 'D' && (new_interval -> Day) >= 11) { //Min 1, only can deduct 10 if greater than 10
        (new_interval -> Day) -= 10;
      }
      break;
    
    case 1: //Day Ones Place
      if (key == 'U' && (new_interval -> Day) < 31) {
        (new_interval -> Day) += 1;
      }
      else if (key == 'D' && (new_interval -> Day) > 1) {
        (new_interval -> Day) -= 1;
      }
      break;
    
    case 2: //Hours Tens Place, using military/24 hour time
      if (key == 'U' && (new_interval -> Hour) <= 14) { //Max 24 
        (new_interval -> Hour) += 10;
      }
      else if (key == 'D' && (new_interval -> Hour) >= 10) { //Min 0
        (new_interval -> Hour) -= 10;
      }
      break;
    
    case 3: //Hours Ones Place
      if (key == 'U' && (new_interval -> Hour) < 24) { 
        (new_interval -> Hour) += 1;
      }
      else if (key == 'D' && (new_interval -> Hour) > 0) {
        (new_interval -> Hour) -= 1;
      }
      break;
    
    case 4: //Minutes Tens Place
      if (key == 'U' && (new_interval -> Minute) < 49) { //Max 59
        (new_interval -> Minute) += 10;
      }
      else if (key == 'D' && (new_interval -> Minute) >= 10) { //Min 0
        (new_interval -> Minute) -= 10;
      }
      break;
    
    case 5:  //Minutes Ones Place
      if (key == 'U' && (new_interval -> Minute) < 59) { 
        (new_interval -> Minute) += 1;
      }
      else if (key == 'D' && (new_interval -> Minute) > 0) {
        (new_interval -> Minute) -= 1;
      }
      break;
    
    default:
      break;
  }

  if ((new_interval -> Day) > 31) {
    (new_interval -> Day) = 31;
  }

  if ((new_interval -> Hour) > 23) {
    (new_interval -> Hour) = 23;
  }

  if ((new_interval -> Minute) > 59) {
    (new_interval -> Minute) = 59;
  }
}

/**
 * @brief controls incrementing and decrementing digits on dry/soak time setting pages
 * 
 * @param key key pressed by user
 * @param new_time pointer to tmElements_t object that is being modified
 * @param cursor_pos current position of cursor
 */
void adjustSetSoakOrDryDigit(char key, tmElements_t* new_time, uint8_t* cursor_pos) {
  switch (*cursor_pos) {
    case 0: //Minutes Tens Place
      if (key == 'U' && (new_time -> Minute) < 49) { //Max 59
        (new_time -> Minute) += 10;
      }
      else if (key == 'D' && (new_time -> Minute) >= 10) { //Min 0
        (new_time -> Minute) -= 10;
      }
      break;
    
    case 1:  //Minutes Ones Place
      if (key == 'U' && (new_time -> Minute) < 59) { 
        (new_time -> Minute) += 1;
      }
      else if (key == 'D' && (new_time -> Minute) > 0) {
        (new_time -> Minute) -= 1;
      }
      break;

      case 2:  //Sec tens Place
        if (key == 'U' && (new_time -> Second) < 49) { 
          (new_time -> Second) += 10;
        }
        else if (key == 'D' && (new_time -> Second) > 0) {
          (new_time -> Second) -= 10;
        }
      break;

      case 3:  //Sec Ones Place
        if (key == 'U' && (new_time -> Second) < 59) { 
          (new_time -> Second) += 1;
        }
        else if (key == 'D' && (new_time -> Second) > 0) {
          (new_time -> Second) -= 1;
        }
      break;
    
    default:
      break;
  }

  // if ((new_time -> Hour) > 23) {
  //   (new_time -> Hour) = 23;
  // }

  if ((new_time -> Minute) > 59) {
    (new_time -> Minute) = 59;
  }

  if ((new_time -> Second) > 59) {
    (new_time -> Second) = 59;
  }
}

/**
 * @brief checks if E-Stop is pressed
 * 
 * @return true if E-Stop is pressed
 * @return false if E-Stop is not pressed
 */
bool checkEstop() {
  if (digitalRead(A1) == 0) {
    estop_pressed = 1;
    state = ESTOP_ALARM;
    return true; //estop is pressed
  }

  else {
    estop_pressed = 0; //Estop is not pressed
    return false;
  }
}

/**
 * @brief reads the magnetic sensor value
 * 
 * @return true if high
 * @return false if low
 */
bool magSensorRead() {
  // Reads the magnetic sensor input
  // Returns 1 if high, 0 if low
  return P1.readDiscrete(HV_GPIO_SLOT, MAG_SENSOR_IO_SLOT);
}

/**
 * @brief initializes RTD module
 * 
 */
void RTDInit() {
  const char P1_04RTD_CONFIG[] = { 0x40, 0x03, 0x60, 0x01, 0x20, 0x02, 0x80, 0x00 };
  // Config data for RTD module, configures Pt1000 type sensor and Celcius units returned when read
  Serial.println(P1.configureModule(P1_04RTD_CONFIG, RTD_SLOT));  //sends the config data to the module in slot 1
}

/**
 * @brief reads RTD value from given temperature sensor
 * 
 * @param sensor_num temperature sensor to read from
 * @return float temperature (in C)
 */
float readRTD(TempSensor sensor_num) {
  return roundf(P1.readTemperature(RTD_SLOT, sensor_num) * 10) / 10.0;
}

/**
 * @brief provides a delay for press-and-hold functionality, prevents adjusted values from 
 *        being incremented/decremented too quickly
 * 
 * @param last_key_pressed 
 * @return char 
 */
char pressAndHold(uint8_t last_key_pressed) {
  //uint8_t currKey = getKeyPress();
  unsigned long start_time = millis();
  unsigned long curr_time = millis();

  while(curr_time - start_time < PRESS_AND_HOLD_INTERVAL_MS) {
    if (getKeyPress() != last_key_pressed) {
      return 0; //Target key has been let go
    }
    curr_time = millis();
  }

  return last_key_pressed;
}
