//  -Aquasens-ASRS
//  -Aquasens - Automated Sample Retrevial System V1.0
//  -Date of Last Revision: 8/1/23
//  -Califonia Polytechnic State University
//  -Bailey College of Science and Mathamatics Biology Department
//  -Primary Owner: Alexis Pasulka
//  -Design Engineers: Doug Brewster and Rob Brewster
//  -Contributors: Sarah Martin (Add contributirs here)
//  -Microcontroller: P1AM-100 ProOpen 
//  -Arduino IDE version:2.3.3
//  -See User Manual For Project Description
//  -non-stock libraries needed: (add library reference here)

//* libraries

#include <P1AM.h>
#include <SD.h>
#include <LiquidCrystal.h>
#include <Time.h>
#include <arduino-timer.h>
#include <RTCZero.h>
#include <SimpleKeypad.h>
#include <TimeLib.h>

/* Pin Mapping ********************************************************************/

// Slots
#define HV_GPIO_SLOT 1                  // High voltage GPIO (P1-15CDD1)
#define PWM_SLOT 2
#define RELAY_SLOT 3

// Inputs
#define MAG_SENS_IN {HV_GPIO_SLOT, 1}   // Magnetic sensor input
#define SWITCH_IN {HV_GPIO_SLOT, 2}     // Switch input (for manual operation)
#define E_STOP_IN {HV_GPIO_SLOT, 3}     // E Stop input
#define KEY_S 7
#define KEY_D 11
#define KEY_U 12
#define KEY_L 13
#define KEY_R 14
#define SD_CS 28

// Outputs
#define STEPPER_PUL 1    // Stepper pulse output
#define STEPPER_DIR 2   // Stepper direction output
#define LCD_POWER {HV_GPIO_SLOT, 10}
#define LCD_RS 0
#define LCD_E  1
#define LCD_D4 2
#define LCD_D5 3
#define LCD_D6 4
#define LCD_D7 6


/* Hardcoded Macros **************************************************************/

#define BASE_DROP_DIS 20      // Distance of drop until water (without tide data) 
#define FLUSH_TIME 100        // Time in sec to flush probe
#define MICROSTEP 3200        // Number of pulses for one revolution of motor
#define TIDE_FILE "tides.txt"


/* Variable Declarations *********************************************************/

// Machine States
enum stateEnum {
  INIT,
  STANDBY,
  ACTIVE,           
  MANUAL,
  SETTING,                 
  SET_CLOCK,              
  SET_INTERVAL            
};
stateEnum state = STANDBY;   // Start up will show menu

// Timing
String interval = "0000-00-00 00:03:00";
tmElements_t nextSampleTime, sampleInterval;
Timer<5, millis> dropTimer;
bool dropFlag = true;                // Set by timers in activeLoop

// RTC
RTCZero rtc;

// LCD
LiquidCrystal lcd(LCD_RS, LCD_E, LCD_D4, LCD_D5, LCD_D6, LCD_D7);

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

  /* Initialize LCD */
  //P1.writeDiscrete(LOW, 1, 2);    // Outputs 5V power source for LCD
  lcd.begin(20, 4);               // cols, rows

  /* SD */
  SD.begin(SD_CS);

}

/*---------------------------------------------------------
* Function: rtcInit()
* Purpose: initialize the RTC
* Params: None                                            
* Returns: None
---------------------------------------------------------*/
void rtcInit() {
  rtc.begin();
  rtc.setEpoch(1729525000);
  sampleInterval.Year = 0;
  sampleInterval.Month = 0;
  sampleInterval.Day = 0;
  sampleInterval.Hour = 1;
  sampleInterval.Minute = 0;  
  sampleInterval.Second = 0;
  
  nextSampleTime.Year = rtc.getYear() + sampleInterval.Year;
  nextSampleTime.Month = rtc.getMonth() + sampleInterval.Month;
  nextSampleTime.Day = rtc.getDay() + sampleInterval.Day;
  nextSampleTime.Hour = rtc.getHours() + sampleInterval.Hour;
  nextSampleTime.Minute = rtc.getMinutes() + sampleInterval.Minute;
  
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

/* Setup and Loop **************************************************************/

void setup() {

  Serial.begin(115200);
  while (!P1.init()) {;}    // Initialize controller

  rtcInit();
  gpioInit();
  //findHomePos();          // Bring probe back to home position
}

void loop() {
  switch (state) {
    case STANDBY:              // Always starts in STANDBY
      standbyLoop();
      break;
    case ACTIVE:
      activeLoop();
      break;
    case MANUAL:
      manualLoop();
      break;
    case SET_CLOCK:
      setClockLoop();
      break;
    case SET_INTERVAL:
      setIntervalLoop();
      break;
    default:
      break;
  }
}
