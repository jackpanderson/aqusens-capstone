
/* State Machine Functions ******************************************************/

void standbyLoop()
{
  char key;
  uint8_t keyPressed;
  lcd.clear();
  cursorY = 2;

  while (state == STANDBY) 
  {
    standbyLCD();

    keyPressed = cursorSelect(2, 3);

    if (keyPressed == 1) {
      if (cursorY == 3) {
        state = RELEASE;
      }

      else if (cursorY = 2) {
        state = SETTINGS;
      }
    }
  }
}

void settingsLoop() {
  uint8_t settingsPage = 1;
  uint8_t lastKeyPress = rtc.getMinutes();
  uint8_t keyPressed;
  resetLcd();
  
  while (state == SETTINGS) {
    settingsLCD(settingsPage); //Launch into first page of settings

    if (settingsPage != 3)
      keyPressed = cursorSelect(0, 2);
    else
      keyPressed = cursorSelect(0, 1);

    if (keyPressed > 0) {
      lastKeyPress = rtc.getMinutes();
    }

    if (keyPressed == 2) {//left
      if (settingsPage == 1) {
        lcd.clear();
        state = STANDBY;
        cursorY = 2;
      }
      else {
        resetLcd();
        settingsPage--;
      }
    }

    else if (keyPressed == 3 && settingsPage != 3) { //right
      resetLcd();
      settingsPage++;
    }

    else if (keyPressed == 1) {
      if (settingsPage == 2) {
        switch(cursorY) { //add more states
          case 0:
            //state = SET_DRY_TIME;
            break;
          case 1:
            //state = SET_SOAK_TIME;
            break;
          case 2:
            state = SET_CLOCK;
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
  resetLcd();
  int position = 0;
  char pos[6];

  while (state == RELEASE) 
  {
    snprintf(pos, sizeof(pos), "%03dcm", position);
    releaseLcd(pos);

    delay(33);
    position++;

    if (position == 900) {
      state = SOAK;
    }
  }
}

void soakLoop() {
  resetLcd();

  while (state == SOAK) 
  {
    soakLcd("0 min");

    delay(10000);

    state = RECOVER;

  }
}

void recoverLoop() {
  resetLcd();
  int position = 900;
  char pos[6];

  while (state == RECOVER) 
  {
    snprintf(pos, sizeof(pos), "%03dcm", position);
    recoverLcd(pos);

    delay(33);
    position--;

    if (position < 0) {
      state = SAMPLE;
    }
  }
}

void sampleLoop() {
  resetLcd();

  while (state == SAMPLE) 
  {
    sampleLcd();

    delay(10000);

    state = FLUSH;

  }
}

void flushLoop() {
  resetLcd();

  while (state == FLUSH) 
  {
    flushLcd();

    delay(10000);

    state = DRY;

  }
}

void dryLoop() {
  resetLcd();

  while (state == DRY) 
  {
    dryLcd("00 min");

    delay(10000);

    state = STANDBY;

  }
}

void manualLoop(){}
void alarmLoop(){}


void setClockLoop() {
  tmElements_t adjustedTime;
  adjustedTime.Month = 1;
  adjustedTime.Day = 1;
  adjustedTime.Year = 2024 - 1970; //Years since from 1970
  adjustedTime.Hour = 0;
  adjustedTime.Minute = 0;
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
        adjustTimeDigit(key, &adjustedTime, &cursorPos);
      }

      updateSetClockLCD(cursorPos, adjustedTime);
    } 
  }
}

void resetLcd()
{
  lcd.clear();
  cursorY = 0;
}


/*---------------------------------------------------------
* Function: activeLoop()
* Purpose: Backend for when machine is in running mode
*           (Whole drop and pull process)
* Params: None                                            
* Returns: None
---------------------------------------------------------*/
// void activeLoop() {
//   String currTime;
//   char nextDropTime[20];
//   char key;
//   double dropDistance;

//   /* Beginning of full routine (calcs) */
//   while (state == ACTIVE) {
//     addTimes(getCurrentDateTime(), interval, nextDropTime);  // Calculate next drop time

//     lcd.clear();
//     activeLCD(String(nextDropTime).substring(5, 15));

//     dropDistance = findClosestDate(nextDropTime) + BASE_DROP_DIS; // Calculate drop distance (1 ft = 1 sec)

//     while (state == ACTIVE) {           // Waiting for next drop
//       currTime = getCurrentDateTime();  // Grab current time
//       if (strcmp(nextDropTime, getCurrentDateTime().c_str()) <= 0) {  // Check if interval has passed
//         // Check if ready to drop (magnetic sensor = true, cleaning over)
//         dropTimer.in(5000, fastDrop);   // Set timer to speed up drop after 5 seconds
//         P1.writePWM(50, 1000, PWM_SLOT, STEPPER_PUL); // Set to slow drop

//         while (dropFlag) {
//           dropTimer.tick();
//           // Check for back button or e stop
//         }
//       }
//       // Check if E stop has been pressed
//       if ((key = getKeyDebounce()) == 'L') {  // Check if back button has been pressed: state = menu
//         state = STANDBY;
//       }
//     }
//   }
// }

// void maintenanceLoop() {
// }



// void manualLoop() {
// }

// void setIntervalLoop() {
//   tmElements_t intComps = parseTime(interval.c_str());  // intComps = interval components
//   char key;

//   lcd.clear();

//   while (state == SET_INTERVAL) {

//     while ((key = getKeyDebounce()) != 'S') {
//       intervalMenuLCD();
//       if (key == 'L') {     // Back button was pressed
//         state = STANDBY;
//         break;
//       }
//     }

//     if (state == STANDBY) {    // Check if back button was pressed
//       break;
//     }
//     lcd.clear();

//     while ((key = getKeyDebounce()) != 'S') {
//       intervalLCD("Year: ", intComps.Year);
//       adjustComponent(&intComps.Year, key);
//     }

//     while ((key = getKeyDebounce()) != 'S') {
//       intervalLCD("Month: ", intComps.Month);
//       adjustComponent(&intComps.Month, key);
//     }

//     clearTopLine();

//     while ((key = getKeyDebounce()) != 'S') {
//       intervalLCD("Day: ", intComps.Day);
//       adjustComponent(&intComps.Day, key);
//     }

//     while ((key = getKeyDebounce()) != 'S') {
//       intervalLCD("Hour: ", intComps.Hour);
//       adjustComponent(&intComps.Hour, key);
//     }

//     while ((key = getKeyDebounce()) != 'S') {
//       intervalLCD("Minute: ", intComps.Minute);
//       adjustComponent(&intComps.Minute, key);
//     }

//     char buffer[20];
//     sprintf(buffer, "%04d-%02d-%02d %02d:%02d:%02d", intComps.Year, intComps.Month, intComps.Day, intComps.Hour, intComps.Minute, intComps.Second);
//     interval = String(buffer);
//     lcd.clear();
//   }
// }

/* Helper Functions **************************************************************/



// void adjustComponent(uint8_t* timeComp, char key) {
//   if (key == 'U') {
//     clearTopLine();
//     (*timeComp)++;
//   } else if (key == 'D') {
//     clearTopLine();
//     (*timeComp)--;
//   }
// }

