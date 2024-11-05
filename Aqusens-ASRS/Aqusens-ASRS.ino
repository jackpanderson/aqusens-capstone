//  -Aqusens-ASRS
//  -Aqusens - Automated Sample Retrevial System V1.0
//  -Date of Last Revision: 8/1/23
//  -Califonia Polytechnic State University
//  -Bailey College of Science and Mathamatics Biology Department
//  -Primary Owner: Alexis Pasulka
//  -Design Engineers: Doug Brewster and Rob Brewster
//  -Contributors: Sarah Martin, Deeba Khosravi, Emma Lucke, Jack Anderson, Jorge Ramirez
//  -Microcontroller: P1AM-100 ProOpen 
//  -Arduino IDE version:2.3.3
//  -See User Manual For Project Description
//  -non-stock libraries needed: (add library reference here)

//* libraries

#include <P1AM.h>
#include <SD.h>
#include <LiquidCrystal.h>
#include <LiquidCrystal_I2C.h>
#include <arduino-timer.h>
#include <RTCZero.h>
#include <SimpleKeypad.h>
#include <TimeLib.h>
#include <math.h>

/* Pin Mapping ********************************************************************/

// Slots
#define HV_GPIO_SLOT 1                  // High voltage GPIO (P1-15CDD1)
#define PWM_SLOT 2
#define RELAY_SLOT 3

// Inputs
#define MAG_SENS_IN {HV_GPIO_SLOT, 1}   // Magnetic sensor input
#define SWITCH_IN {HV_GPIO_SLOT, 2}     // Switch input (for manual operation)
#define E_STOP_IN {HV_GPIO_SLOT, 3}     // E Stop input
#define KEY_S 0
#define KEY_D 1
#define KEY_U 2
#define KEY_L 3
#define KEY_R 4
#define SD_CS 28
//#define ESTOP_IN 4 //Change to real value

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
#define NUM_CONTRAST_STEPS 20
#define NUM_BRIGHTNESS_STEPS 20


/* Variable Declarations *********************************************************/

// Machine States
enum stateEnum {
  INIT,
  STANDBY,
  SETTINGS,
  RELEASE,
  SOAK,
  RECOVER,
  SAMPLE,
  FLUSH,
  DRY,
  MOTOR_ALARM,
  ESTOP_ALARM,
  MANUAL,
  SET_INTERVAL,
  ENSURE_SAMPLE_START,
  SET_START_TIME,
  SET_FLUSH_TIME,
  ADD_EVENT,
  VIEW_EVENTS,
  SET_CLOCK,
  SET_DRY_TIME,
  SET_SOAK_TIME,
  FILTER_STATUS,
  SET_BRIGHTNESS,
  SET_CONTRAST      
};
volatile stateEnum state = STANDBY;   // Start up will show menu
volatile bool isDelayingStartTime = false;

// Timing
String interval = "0000-00-00 00:30:00";
tmElements_t nextSampleTime, sampleInterval,soakTime, dryTime;
Timer<5, millis> dropTimer;
bool dropFlag = true;                // Set by timers in activeLoop

int8_t cursorY = 2;
uint8_t screenBrightness = 10;
uint8_t screenContrast = 10; //change to a sensible init val, is half of num of steps
uint8_t lastSettingPage = 4;
uint8_t settingsPage = 1;
// RTC
RTCZero rtc;

// LCD
LiquidCrystal lcd(LCD_RS, LCD_E, LCD_D4, LCD_D5, LCD_D6, LCD_D7);

LiquidCrystal_I2C lcd2(0x27, 20, 4);

/* Setup and Loop **************************************************************/

void setup() {

  Serial.begin(115200);
  while (!P1.init()) {;}    // Initialize controller

  rtcInit();
  gpioInit();

  lcd2.init();          // Initialize the LCD
  lcd2.backlight();      // Turn on the backlight
  lcd2.setCursor(0, 0);  // Set cursor to column 0, row 0
  lcd2.print("Hello, P1AM-100!"); // Print a message
  //findHomePos();          // Bring probe back to home position
}

void loop() {
  lcd2.print("poop");
  delay(500);
  switch (state) {
    case STANDBY:              // Always starts in STANDBY
      standbyLoop();
      break;
    case SETTINGS:
      settingsLoop();
      break;
    case RELEASE:
      releaseLoop();
      break;
    case SOAK:
      soakLoop();
      break;
    case RECOVER:
      recoverLoop();
      break;
    case SAMPLE:
      sampleLoop();
      break;
    case FLUSH:
      flushLoop();
      break;
    case DRY:
      dryLoop();
      break;
    case MOTOR_ALARM:
      alarmLoop();
      break;
    case ESTOP_ALARM:
      alarmLoop();
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
    case SET_START_TIME:
      setStartTimeLoop();
      break;
    case SET_SOAK_TIME:
      setSoakTimeLoop();
      break;
    case SET_DRY_TIME:
      setDryTimeLoop();
      break;
    case SET_BRIGHTNESS:
      setBrightnessLoop();
      break;
    case ENSURE_SAMPLE_START:
      ensureSampleStartLoop();
      break;
    default:
      break;
  }
}
