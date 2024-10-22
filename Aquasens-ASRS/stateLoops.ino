
/* State Machine Functions ******************************************************/

// /*---------------------------------------------------------
// * Function: menuLoop()
// * Purpose: backend for main menu
// *   - Polls keypad for key, parses and switches to new
// *      state
// * Params: None                                            
// * Returns: None
// ---------------------------------------------------------*/
// void standbyLoop() {
//   char key;
//   uint8_t cursorX = 0, cursorY = 0;

//   lcd.clear();

//   while (state == STANDBY) {

//     standbyLCD(cursorX, cursorY);

//     if ((key = getKeyDebounce()) != NULL) {
//       switch (key) {

//         case 'U':  // UP
//           if (cursorY == 1) {
//             cursorY--;
//           }
//           break;

//         case 'D':  // DOWN
//           if (cursorY == 0) {
//             cursorY++;
//           }
//           break;

//         case 'L':  // LEFT
//           if (cursorX == 1) {
//             cursorX--;
//           }
//           break;

//         case 'R':  // RIGHT
//           if (cursorX == 0) {
//             cursorX++;
//           }
//           break;

//         case 'S':  // SELECT
//           if (cursorX == 0 && cursorY == 0) {
//             state = ACTIVE;
//           } else if (cursorX == 0 && cursorY == 1) {
//             state = SET_INTERVAL;
//           } else if (cursorX == 1 && cursorY == 0) {
//             state = MANUAL;
//           } else if (cursorX == 1 && cursorY == 1) {
//             state = SET_CLOCK;
//           }
//           break;
//         default:
//           break;
//       }
//     }
//   }
// }


void standbyLoop()
{
  char key;
  uint8_t cursorX, cursorY = 0;
  lcd.clear();

  while (state == STANDBY) 
  {
    standbyLCD(cursorX, cursorY);
    
    // if ((key = getKeyDebounce()) == 'U' && cursorX == 2) 
    // {  // Check if back button has been pressed: state = menu
    //   cursorX = 1;
    // } else if ((key = getKeyDebounce()) == 'D' && cursorX == 1) 
    // {
    //   cursorX = 2;
    // }

    int newCursor = cursorSelect(1, 2, cursorX);

    if (newCursor == cursorX) {
      if (cursorX == 1) 
      {
        lcd.clear();
        lcd.print("run");
      } else if (cursorX == 2)
      {
        lcd.clear();
        lcd.print("settings");
      }
    } else if (newCursor >= 0) {
      cursorX = newCursor;
    }

    // if ((key = getKeyDebounce()) == 'S') 
    // {
    //   if (cursorX == 1) 
    //   {
    //     lcd.clear();
    //     lcd.print("run");
    //   } else if (cursorX == 2)
    //   {
    //     lcd.clear();
    //     lcd.print("settings");
    //   }
    // }
  }
}

int cursorSelect(int begin, int end, int cursorX) 
{
  char key;
  
  if ((key = getKeyDebounce()) == 'U' && cursorX > begin) {  // Check if back button has been pressed: state = menu
    return cursorX - 1;
  } else if ((key = getKeyDebounce()) == 'D' && cursorX < end) {
    return cursorX + 1;
  } else if ((key = getKeyDebounce()) == 'S') {
    return cursorX;
  } else {
    return -1;
  }
}

/*---------------------------------------------------------
* Function: activeLoop()
* Purpose: Backend for when machine is in running mode
*           (Whole drop and pull process)
* Params: None                                            
* Returns: None
---------------------------------------------------------*/
void activeLoop() {
  String currTime;
  char nextDropTime[20];
  char key;
  double dropDistance;

  /* Beginning of full routine (calcs) */
  while (state == ACTIVE) {
    addTimes(getCurrentDateTime(), interval, nextDropTime);  // Calculate next drop time

    lcd.clear();
    activeLCD(String(nextDropTime).substring(5, 15));

    dropDistance = findClosestDate(nextDropTime) + BASE_DROP_DIS; // Calculate drop distance (1 ft = 1 sec)

    while (state == ACTIVE) {           // Waiting for next drop
      currTime = getCurrentDateTime();  // Grab current time
      if (strcmp(nextDropTime, getCurrentDateTime().c_str()) <= 0) {  // Check if interval has passed
        // Check if ready to drop (magnetic sensor = true, cleaning over)
        dropTimer.in(5000, fastDrop);   // Set timer to speed up drop after 5 seconds
        P1.writePWM(50, 1000, PWM_SLOT, STEPPER_PUL); // Set to slow drop

        while (dropFlag) {
          dropTimer.tick();
          // Check for back button or e stop
        }
      }
      // Check if E stop has been pressed
      if ((key = getKeyDebounce()) == 'L') {  // Check if back button has been pressed: state = menu
        state = STANDBY;
      }
    }
  }
}

void maintenanceLoop() {
}

void setClockLoop() {
}

void manualLoop() {
}

void setIntervalLoop() {
  tmElements_t intComps = parseTime(interval.c_str());  // intComps = interval components
  int cursorX = 0;
  char key;

  lcd.clear();

  while (state == SET_INTERVAL) {

    while ((key = getKeyDebounce()) != 'S') {
      intervalMenuLCD();
      if (key == 'L') {     // Back button was pressed
        state = STANDBY;
        break;
      }
    }

    if (state == STANDBY) {    // Check if back button was pressed
      break;
    }
    lcd.clear();

    while ((key = getKeyDebounce()) != 'S') {
      intervalLCD("Year: ", intComps.Year);
      adjustComponent(&intComps.Year, key);
    }

    while ((key = getKeyDebounce()) != 'S') {
      intervalLCD("Month: ", intComps.Month);
      adjustComponent(&intComps.Month, key);
    }

    clearTopLine();

    while ((key = getKeyDebounce()) != 'S') {
      intervalLCD("Day: ", intComps.Day);
      adjustComponent(&intComps.Day, key);
    }

    while ((key = getKeyDebounce()) != 'S') {
      intervalLCD("Hour: ", intComps.Hour);
      adjustComponent(&intComps.Hour, key);
    }

    while ((key = getKeyDebounce()) != 'S') {
      intervalLCD("Minute: ", intComps.Minute);
      adjustComponent(&intComps.Minute, key);
    }

    char buffer[20];
    sprintf(buffer, "%04d-%02d-%02d %02d:%02d:%02d", intComps.Year, intComps.Month, intComps.Day, intComps.Hour, intComps.Minute, intComps.Second);
    interval = String(buffer);
    lcd.clear();
  }
}

/* Helper Functions **************************************************************/

/*---------------------------------------------------------
* Function: fastDrop()
* Purpose: function called by the timer to start speed up
*           the drop
* Params: None                                            
* Returns: Flag to indicate if function was successful
---------------------------------------------------------*/
bool fastDrop(void *) {
  P1.writePWM(50, MICROSTEP, PWM_SLOT, STEPPER_PUL);
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
  P1.writePWM(50, 0, PWM_SLOT, STEPPER_PUL);
  return true;
}

/*---------------------------------------------------------
* Function: getKeyDebounce()
* Purpose: gets key press from user (debounced)
* Params: None                                            
* Returns: key pressed by the user
---------------------------------------------------------*/
char getKeyDebounce() {
  char key = getKeyPress();

  if (key != NULL) {
    while (getKeyPress() == key) { ; }
  }

  return key;
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

void adjustComponent(uint8_t* timeComp, char key) {
  if (key == 'U') {
    clearTopLine();
    (*timeComp)++;
  } else if (key == 'D') {
    clearTopLine();
    (*timeComp)--;
  }
}

String getCurrentDateTime() {
  char dateTimeString[16];
  //uint32_t currentEpoch = rtc.getEpoch();

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
  //uint32_t currentEpoch = rtc.getEpoch();

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
  sprintf(buffer, "%04d-%02d-%02d %02d:%02d:%02d", tm.Year, tm.Month, tm.Day, tm.Hour, tm.Minute, tm.Second);
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