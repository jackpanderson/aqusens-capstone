#define TIME_BASED_DEBOUNCE_WAIT_TIME_MS 35

/* Init Functions **************************************************************/

/*---------------------------------------------------------
* Function: gpioInit()
* Purpose: initializes all pins on gpio module
* Params: None                                            
* Returns: None
---------------------------------------------------------*/
void gpioInit() {
  /* Initialize Stepper PWM */
  pinMode(KEY_U, INPUT_PULLDOWN);
  pinMode(KEY_D, INPUT_PULLDOWN);
  pinMode(KEY_L, INPUT_PULLDOWN);
  pinMode(KEY_R, INPUT_PULLDOWN);
  pinMode(KEY_S, INPUT_PULLDOWN);
  pinMode(DIR_POS_PIN, OUTPUT);
  pinMode(A1, INPUT_PULLDOWN);
  //pinMode(DIR_NEG_PIN, OUTPUT);
  // pinMode(STEP_NEG_PIN, OUTPUT);
  // digitalWrite(STEP_NEG_PIN, 0);

  // pinMode(STEP_POS_PIN, OUTPUT);
  // pinMode(STEP_NEG_PIN, OUTPUT);
  // pinMode(DIR_POS_PIN, OUTPUT);
  // pinMode(DIR_NEG_PIN, OUTPUT);

  /* Initialize LCD */
  //P1.writeDiscrete(LOW, 1, 2);    // Outputs 5V power source for LCD
  //lcd.begin(20, 4);               // cols, rows

  /* SD */
  SD.begin(SD_CS);
}

void estopInit() {
  pinMode(A1, INPUT_PULLDOWN);

  attachInterrupt(digitalPinToInterrupt(A1), onLowTrigger, FALLING);
}

void updateSolenoid(solenoidState state, int solenoidNumber) {
  if (solenoidNumber == SOLENOID_ONE || solenoidNumber == SOLENOID_TWO) {
      if (state == OPEN)
        P1.writeDiscrete(false, RELAY_SLOT, solenoidNumber);
      else
        P1.writeDiscrete(true, RELAY_SLOT, solenoidNumber);
  }
}

void onLowTrigger() {
  //setMotorSpeed(0);
  state = ESTOP_ALARM;
  estopPressed = 1;
}


/*---------------------------------------------------------
* Function: rtcInit()
* Purpose: initialize the RTC
* Params: None                                            
* Returns: None
---------------------------------------------------------*/
void rtcInit() {
  rtc.begin();
  rtc.setEpoch(1738123553);
  sampleInterval.Year = 0;
  sampleInterval.Month = 0;
  sampleInterval.Day = 0;
  sampleInterval.Hour = 0;
  sampleInterval.Minute = 15;  
  sampleInterval.Second = 0;

  nextSampleTime.Year = rtc.getYear() + sampleInterval.Year;
  nextSampleTime.Month = rtc.getMonth() + sampleInterval.Month;
  nextSampleTime.Day = rtc.getDay() + sampleInterval.Day;
  nextSampleTime.Hour = rtc.getHours() + sampleInterval.Hour;
  nextSampleTime.Minute = rtc.getMinutes() + sampleInterval.Minute;

  soakTime.Minute = 0;
  soakTime.Second = 5;

  // dryTime.Minute = 20;
  dryTime.Minute = 0;
  dryTime.Second = 30;

  tubeFlushTime.Minute = 0;
  tubeFlushTime.Second = 15;

  aqusensFlushTime.Minute = 0;
  aqusensFlushTime.Second = 15;
  
  updateAlarm();
}


// void updateStartTime() {
//   rtc.setAlarmTime(nextSampleTime.Hour, nextSampleTime.Minute, 0); // Set alarm for the specified time
//   rtc.setAlarmDate(nextSampleTime.Day, nextSampleTime.Month, nextSampleTime.Year);
//   rtc.enableAlarm(rtc.MATCH_YYMMDDHHMMSS); // Match hours and minutes
//   rtc.attachInterrupt(alarmTriggered); // Attach the ISR for the alarm interrupt
// }

// void startDelayAlarm(tmElements_t newStartTime) {

//   breakTime(makeTime(newStartTime), newStartTime);
  
//   rtc.setAlarmTime(nextSampleTime.Hour, nextSampleTime.Minute, 0); // Set alarm for the specified time
//   rtc.setAlarmDate(nextSampleTime.Day, nextSampleTime.Month, nextSampleTime.Year);
//   rtc.enableAlarm(rtc.MATCH_YYMMDDHHMMSS); // Match hours and minutes
//   rtc.attachInterrupt(delayAlarmTriggered); // Attach the ISR for the alarm interrupt
// }


void updateAlarm(tmElements_t delayTime) {

  breakTime(makeTime(nextSampleTime), nextSampleTime);

  nextSampleTime.Year = delayTime.Year;
  nextSampleTime.Month = delayTime.Month;
  nextSampleTime.Day = delayTime.Day;
  nextSampleTime.Hour = delayTime.Hour;
  nextSampleTime.Minute = delayTime.Minute;


  
  rtc.setAlarmTime(nextSampleTime.Hour, nextSampleTime.Minute, 0); // Set alarm for the specified time
  rtc.setAlarmDate(nextSampleTime.Day, nextSampleTime.Month, nextSampleTime.Year);
  rtc.enableAlarm(rtc.MATCH_YYMMDDHHMMSS); // Match hours and minutes
  rtc.attachInterrupt(alarmTriggered); // Attach the ISR for the alarm interrupt
}

void updateAlarm() {
  nextSampleTime.Year = rtc.getYear() + sampleInterval.Year;
  nextSampleTime.Month = rtc.getMonth() + sampleInterval.Month;
  nextSampleTime.Day = rtc.getDay() + sampleInterval.Day;
  nextSampleTime.Hour = rtc.getHours() + sampleInterval.Hour;
  nextSampleTime.Minute = rtc.getMinutes() + sampleInterval.Minute;


  breakTime(makeTime(nextSampleTime), nextSampleTime);
  
  rtc.setAlarmTime(nextSampleTime.Hour, nextSampleTime.Minute, 0); // Set alarm for the specified time
  rtc.setAlarmDate(nextSampleTime.Day, nextSampleTime.Month, nextSampleTime.Year);
  rtc.enableAlarm(rtc.MATCH_YYMMDDHHMMSS); // Match hours and minutes
  rtc.attachInterrupt(alarmTriggered); // Attach the ISR for the alarm interrupt
}

void alarmTriggered() {
  // This will be called when the alarm is triggered
  // You can handle additional tasks here if needed
  nextSampleTime.Year = rtc.getYear() + sampleInterval.Year;
  nextSampleTime.Month = rtc.getMonth() + sampleInterval.Month;
  nextSampleTime.Day = rtc.getDay() + sampleInterval.Day;
  nextSampleTime.Hour = rtc.getHours() + sampleInterval.Hour;
  nextSampleTime.Minute = rtc.getMinutes() + sampleInterval.Minute;
  
  breakTime(makeTime(nextSampleTime), nextSampleTime);

  rtc.setAlarmTime(nextSampleTime.Hour, nextSampleTime.Minute, 0); // Set alarm for the specified time
  rtc.setAlarmDate(nextSampleTime.Day, nextSampleTime.Month, nextSampleTime.Year);

  if (state == STANDBY) {
    state = RELEASE;
  }
}

/*---------------------------------------------------------
* Function: findHomePos()
* Purpose: Returns the probe to home position
* Params: None                                            
* Returns: None
---------------------------------------------------------*/
void findHomePos() {
  // Checks if probe is connected to magnetic sensor
  // If not:
  //    Slowly brings the probe up until it is connected
  //    to magnetic sensor
  // Need a way to make sure it is not above the sensor
}

char cursorSelect(int begin, int end) 
{
  char key = getKeyDebounce();
  
  if (key == 'U' && cursorY > begin) {  // Check if back button has been pressed: state = menu
    lcd.setCursor(0, cursorY);
    lcd.print(" ");
    cursorY--;
    return 'U';
  } 
  else if (key == 'D' && cursorY < end) {
    lcd.setCursor(0, cursorY);
    lcd.print(" ");
    cursorY++;
    return 'D';
  }

  else {
    return key;
  }
}

/*---------------------------------------------------------
* Function: getKeyDebounce()
* Purpose: gets key press from user (debounced)
* Params: None                                            
* Returns: key pressed by the user
---------------------------------------------------------*/
char getKeyDebounce() {

  char key = getKeyPress();
  //Serial.println(key);

  if (key != NULL) {
    while (getKeyPress() == key) {;}
  }


  return key;
}



//Time based debounce function for press-and-hold
char getKeyTimeBasedDebounce() {
  char key = getKeyPress();

  unsigned long startTime = millis();
  unsigned long currTime = millis();

  if (key == 'U' || key == 'D') {
    while (currTime - startTime < TIME_BASED_DEBOUNCE_WAIT_TIME_MS) {
      currTime = millis();
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

String getCurrentDateTime() {
  char dateTimeString[16];

  snprintf(dateTimeString, sizeof(dateTimeString), "%02d-%02d-%02d %02d:%02d",
           rtc.getMonth(),
           rtc.getDay(),
           rtc.getYear(),
           rtc.getHours(),
           rtc.getMinutes());

  return String(dateTimeString);
}

String getNextSampleTime() {
  char dateTimeString[16];

  snprintf(dateTimeString, sizeof(dateTimeString), "%02d-%02d-%02d %02d:%02d",
           nextSampleTime.Month,
           nextSampleTime.Day,
           nextSampleTime.Year,
           nextSampleTime.Hour,
           nextSampleTime.Minute);

  return String(dateTimeString);
}

tmElements_t parseTime(const char* timeStr) {
  int year, month, day, hour, minute, second;
  sscanf(timeStr, "%4d-%2d-%2d %2d:%2d:%2d", &year, &month, &day, &hour, &minute, &second);

  tmElements_t tm;
  tm.Year = year;  // Arduino time library uses years since 1970
  tm.Month = month;
  tm.Day = day;
  tm.Hour = hour;
  tm.Minute = minute;
  tm.Second = second;

  return tm;
}

void formatTime(char* buffer, time_t time) {
  tmElements_t tm;
  breakTime(time, tm);
  sprintf(buffer, "%0d-%02d-%02d %02d:%02d:%02d", tm.Year, tm.Month, tm.Day, tm.Hour, tm.Minute, tm.Second);
}


void addTimes(String time1Str, String time2Str, char* result) {
  tmElements_t resultTime;
  tmElements_t time1 = parseTime(time1Str.c_str());
  tmElements_t time2 = parseTime(time2Str.c_str());

  resultTime.Year = time1.Year + time2.Year; 
  resultTime.Month = time1.Month + time2.Month;
  resultTime.Day = time1.Day + time2.Day;
  resultTime.Hour = time1.Hour + time2.Hour;
  resultTime.Minute = time1.Minute + time2.Minute;
  resultTime.Second = 0;

  formatTime(result, makeTime(resultTime));
}

/*---------------------------------------------------------
* Function: fastDrop()
* Purpose: function called by the timer to start speed up
*           the drop
* Params: None                                            
* Returns: Flag to indicate if function was successful
---------------------------------------------------------*/
bool fastDrop(void *) {
  //P1.writePWM(50, MICROSTEP, PWM_SLOT, STEPPER_PUL);
  //dropTimer(dropDistance*1000, stopMotor);  // Need to create new timer that takes dropDistance as param or make
                                              // dropDistance a global variable. First way cleaner, second way faster
  return true;
}

/*---------------------------------------------------------
* Function: stopMotor()
* Purpose: function called by the timer to stop the motor
* Params: None                                            
* Returns: Flag to indicate if function was successful
---------------------------------------------------------*/
bool stopMotor(void *) {
  //P1.writePWM(50, 0, PWM_SLOT, STEPPER_PUL);
  return true;
}

/*---------------------------------------------------------
* Function: getKeyPress()
* Purpose: gets any key currently pressed by user
* Params: None                                            
* Returns: key pressed by user (NULL if no current key press)
---------------------------------------------------------*/
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

void adjustSetTimeDigit(char key, tmElements_t* adjustedTime, uint8_t* cursorPos) {
  switch (*cursorPos) {
    case 0: //Month Tens Place
      if (key == 'U' && (adjustedTime -> Month) < 10) {
        (adjustedTime -> Month) += 10;
      }
      else if (key == 'D' && (adjustedTime -> Month) > 10) {
        (adjustedTime -> Month) -= 10;
      }
      break;
    
    case 1: //Month Ones Place
      if (key == 'U' && (adjustedTime -> Month) < 12) {
        (adjustedTime -> Month) += 1;
      }
      else if (key == 'D' && (adjustedTime -> Month) > 1) {
        (adjustedTime -> Month) -= 1;
      }
      break;
    
    case 2: //Day Tens Place
      if (key == 'U' && (adjustedTime -> Day) <= 21) { //Max 31, only true if we can validly add 10 to the day
        (adjustedTime -> Day) += 10;
      }
      else if (key == 'D' && (adjustedTime -> Day) >= 11) { //Min 1, only can deduct 10 if greater than 10
        (adjustedTime -> Day) -= 10;
      }
      break;
    
    case 3: //Day Ones Place
      if (key == 'U' && (adjustedTime -> Day) < 31) {
        (adjustedTime -> Day) += 1;
      }
      else if (key == 'D' && (adjustedTime -> Day) > 1) {
        (adjustedTime -> Day) -= 1;
      }
      break;
    
    case 4: //Year Tens Place
      if (key == 'U' && (adjustedTime -> Year) <= 119) { //Max is 2099, 2099-1970 = 129
        (adjustedTime -> Year) += 10;
      }
      else if (key == 'D' && (adjustedTime -> Year) >= 10) {
        (adjustedTime -> Year) -= 10;
      }
      break;
    
    case 5: //Year Ones Place
      if (key == 'U' && (adjustedTime -> Year) < 129) { 
        (adjustedTime -> Year) += 1;
      }
      else if (key == 'D' && (adjustedTime -> Year) > 0) {
        (adjustedTime -> Year) -= 1;
      }
      break;
    
    case 6: //Hours Tens Place, using military/24 hour time
      if (key == 'U' && (adjustedTime -> Hour) <= 14) { //Max 24 
        (adjustedTime -> Hour) += 10;
      }
      else if (key == 'D' && (adjustedTime -> Hour) >= 10) { //Min 0
        (adjustedTime -> Hour) -= 10;
      }
      break;
    
    case 7: //Hours Ones Place
      if (key == 'U' && (adjustedTime -> Hour) < 24) { 
        (adjustedTime -> Hour) += 1;
      }
      else if (key == 'D' && (adjustedTime -> Hour) > 0) {
        (adjustedTime -> Hour) -= 1;
      }
      break;
    
    case 8: //Minutes Tens Place
      if (key == 'U' && (adjustedTime -> Minute) < 49) { //Max 59
        (adjustedTime -> Minute) += 10;
      }
      else if (key == 'D' && (adjustedTime -> Minute) >= 10) { //Min 0
        (adjustedTime -> Minute) -= 10;
      }
      break;
    
    case 9:  //Minutes Ones Place
      if (key == 'U' && (adjustedTime -> Minute) < 59) { 
        (adjustedTime -> Minute) += 1;
      }
      else if (key == 'D' && (adjustedTime -> Minute) > 0) {
        (adjustedTime -> Minute) -= 1;
      }
      break;
    
    default:
      break;
  }
}

void adjustSetIntervalDigit(char key, tmElements_t* newInterval, uint8_t* cursorPos) {
  switch (*cursorPos) {
    case 0: //Day Tens Place
      if (key == 'U' && (newInterval -> Day) <= 21) { //Max 31, only true if we can validly add 10 to the day
        (newInterval -> Day) += 10;
      }
      else if (key == 'D' && (newInterval -> Day) >= 11) { //Min 1, only can deduct 10 if greater than 10
        (newInterval -> Day) -= 10;
      }
      break;
    
    case 1: //Day Ones Place
      if (key == 'U' && (newInterval -> Day) < 31) {
        (newInterval -> Day) += 1;
      }
      else if (key == 'D' && (newInterval -> Day) > 1) {
        (newInterval -> Day) -= 1;
      }
      break;
    
    case 2: //Hours Tens Place, using military/24 hour time
      if (key == 'U' && (newInterval -> Hour) <= 14) { //Max 24 
        (newInterval -> Hour) += 10;
      }
      else if (key == 'D' && (newInterval -> Hour) >= 10) { //Min 0
        (newInterval -> Hour) -= 10;
      }
      break;
    
    case 3: //Hours Ones Place
      if (key == 'U' && (newInterval -> Hour) < 24) { 
        (newInterval -> Hour) += 1;
      }
      else if (key == 'D' && (newInterval -> Hour) > 0) {
        (newInterval -> Hour) -= 1;
      }
      break;
    
    case 4: //Minutes Tens Place
      if (key == 'U' && (newInterval -> Minute) < 49) { //Max 59
        (newInterval -> Minute) += 10;
      }
      else if (key == 'D' && (newInterval -> Minute) >= 10) { //Min 0
        (newInterval -> Minute) -= 10;
      }
      break;
    
    case 5:  //Minutes Ones Place
      if (key == 'U' && (newInterval -> Minute) < 59) { 
        (newInterval -> Minute) += 1;
      }
      else if (key == 'D' && (newInterval -> Minute) > 0) {
        (newInterval -> Minute) -= 1;
      }
      break;
    
    default:
      break;
  }

  if ((newInterval -> Day) > 31) {
    (newInterval -> Day) = 31;
  }

  if ((newInterval -> Hour) > 23) {
    (newInterval -> Hour) = 23;
  }

  if ((newInterval -> Minute) > 59) {
    (newInterval -> Minute) = 59;
  }
}

void adjustSetSoakOrDryDigit(char key, tmElements_t* newTime, uint8_t* cursorPos) {
  switch (*cursorPos) {
    case 0: //Minutes Tens Place
      if (key == 'U' && (newTime -> Minute) < 49) { //Max 59
        (newTime -> Minute) += 10;
      }
      else if (key == 'D' && (newTime -> Minute) >= 10) { //Min 0
        (newTime -> Minute) -= 10;
      }
      break;
    
    case 1:  //Minutes Ones Place
      if (key == 'U' && (newTime -> Minute) < 59) { 
        (newTime -> Minute) += 1;
      }
      else if (key == 'D' && (newTime -> Minute) > 0) {
        (newTime -> Minute) -= 1;
      }
      break;

      case 2:  //Sec tens Place
        if (key == 'U' && (newTime -> Second) < 49) { 
          (newTime -> Second) += 10;
        }
        else if (key == 'D' && (newTime -> Second) > 0) {
          (newTime -> Second) -= 10;
        }
      break;

      case 3:  //Sec Ones Place
        if (key == 'U' && (newTime -> Second) < 59) { 
          (newTime -> Second) += 1;
        }
        else if (key == 'D' && (newTime -> Second) > 0) {
          (newTime -> Second) -= 1;
        }
      break;
    
    default:
      break;
  }

  // if ((newTime -> Hour) > 23) {
  //   (newTime -> Hour) = 23;
  // }

  if ((newTime -> Minute) > 59) {
    (newTime -> Minute) = 59;
  }

  if ((newTime -> Second) > 59) {
    (newTime -> Second) = 59;
  }
}

bool checkEstop() {
  if (digitalRead(A1) == 0) {
    estopPressed = 1;
    state = ESTOP_ALARM;
    return false; //estop is pressed
  }

  else {
    estopPressed = 0; //Estop is not pressed
    return true;
  }
}

bool magSensorRead() {
  // Reads the magnetic sensor input
  // Returns 1 if high, 0 if low
  return P1.readDiscrete(HV_GPIO_SLOT, MAG_SENSOR_IO_SLOT);
}

void RTDInit() {
  const char P1_04RTD_CONFIG[] = { 0x40, 0x03, 0x60, 0x01, 0x20, 0x02, 0x80, 0x00 };
  Serial.println(P1.configureModule(P1_04RTD_CONFIG, RTD_SLOT));  //sends the config data to the module in slot 1
}

float readRTD() {
  return roundf(P1.readTemperature(RTD_SLOT, TEMP_SENSOR_ONE) * 10) / 10.0;
}


void updateMotorCurrPositionDisplay(int currPos) {
  //int32_t currPos = 40; //Temporary, assuming will have a global variable that tracks position
  lcd.setCursor(15, 3);

  int meters = currPos/100; //Converts cm to nearest meter, assuming less than 10!
  int remainingCm = currPos - meters*100; //Leftover centimeters

  char formattedDistance[6]; //Ex: 4.54, assuming less than 10 meters 

  if (remainingCm >= 10) {
    snprintf(formattedDistance, 6, "%i.%im", meters, remainingCm); 
  }

  else {
    snprintf(formattedDistance, 6, "%i.0%im", meters, remainingCm); //Accounts for leading zero if remaining cm is less than 10
  }

  lcd.print(formattedDistance);
}
