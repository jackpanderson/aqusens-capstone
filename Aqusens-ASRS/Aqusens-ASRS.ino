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
#include "SAMD_PWM.h"

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
// #define ESTOP_IN 4 //Change to real value

//Motor
#define STEP_POS_PIN  6
#define STEP_NEG_PIN 7

#define DIR_POS_PIN   13
#define DIR_NEG_PIN  14

// Outputs
// #define STEPPER_PUL 1    // Stepper pulse output
// #define STEPPER_DIR 2   // Stepper direction output
// #define LCD_POWER {HV_GPIO_SLOT, 10}
// #define LCD_RS 0
// #define LCD_E  1
// #define LCD_D4 2
// #define LCD_D5 3
// #define LCD_D6 4
// #define LCD_D7 6


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
  CALIBRATE,
  STANDBY,
  RELEASE,
  SOAK,
  RECOVER,
  SAMPLE,
  FLUSH,
  DRY,
  MOTOR_ALARM,
  ESTOP_ALARM,
  MANUAL,
  MOTOR_CONTROL,
  SETTINGS,
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

volatile stateEnum state = STANDBY;   // Start up will show standby state
// volatile bool isDelayingStartTime = false; 

// Timing
tmElements_t nextSampleTime, sampleInterval, soakTime, dryTime;
Timer<5, millis> dropTimer;
bool dropFlag = true; // Set by timers in activeLoop
bool estopPressed = false; // Flag to keep track of E-stop pressed/released

int8_t cursorY = 2; // keeps track of current cursor position
uint8_t screenBrightness = 10; 
uint8_t screenContrast = 10; // change to a sensible init val, is half of num of steps
uint8_t lastSettingPage = 4; // amount of settings pages
uint8_t settingsPage = 1; // current settings page

// RTC
RTCZero rtc;

//PWM
SAMD_PWM* stepper; //With 50:1 gearbox, max stable speed is around 47000-50000

//I2C LCD Screen
LiquidCrystal_I2C lcd(0x27, 20, 4);

/* Setup and Loop **************************************************************/

int speed = 800;
void setup() {

  Serial.begin(115200);
  while (!P1.init()) {;} // Initialize controller

  rtcInit();
  gpioInit();
  estopInit();
  motorInit();

  lcd.init(); // Initialize the LCD
  lcd.backlight(); // Turn on the backlight
  lcd.setCursor(0, 0); // Set cursor to column 0, row 0
  
  setMotorSpeed(40000);
  // motorInit();
  // Serial.println("Setting DOWN!");
  // setMotorDir('D');
  // setMotorSpeed(20000);
  // delay(10000);
  //findHomePos();          // Bring probe back to home position
}

void loop() {
  // setMotorDir('D');
  // while(1) {
  //   digitalWrite(STEP_POS_PIN, HIGH);
  //   delay(1000);
  //   digitalWrite(STEP_POS_PIN, LOW);
  // }
  switch (state) {
    case CALIBRATE: // Entered after Alarm mode to recalibrate sample device and flush as needed
      calibrateLoop();
      break;
    case STANDBY: // Always starts in STANDBY
      standbyLoop();
      break;
    case ENSURE_SAMPLE_START: // "Are you sure?" screen for manually starting sample run
      ensureSampleStartLoop();
      break;
    case RELEASE: // Releasing the sample device to the ocean
      releaseLoop();
      break;
    case SOAK: // Device on the surface of the ocean, collecting sample
      soakLoop();
      break;
    case RECOVER: // Recovering the sample device from the ocean surface to the home position
      recoverLoop();
      break;
    case SAMPLE: // Sample is sent through the Aqusens device
      sampleLoop();
      break;
    case FLUSH: // Aqusens and Sample device are flushed with filtered freshwater/air
      flushLoop();
      break;
    case DRY: // Sample device is dried for predetermined amount of time
      dryLoop();
      break;
    case MOTOR_ALARM: // Alarm mode is tripped due to motor alarm
      alarmLoop();
      break;
    case ESTOP_ALARM: // Alarm mode is tripped due to E-stop press
      alarmLoop();
      break;
    case MANUAL: // Manual control of motor/solenoids, only entered from alarm mode
      manualLoop();
      break;
    case MOTOR_CONTROL:
      motorControlLoop();
      break;
    case SETTINGS: // Pages of parameters that can be modified or checked
      settingsLoop();
      break;
    case SET_CLOCK: // Settings option to set current time
      setClockLoop();
      break;
    case SET_INTERVAL: // Settings option to set sampling interval
      setIntervalLoop();
      break;
    case SET_START_TIME: // Settings option to set start time
      setStartTimeLoop();
      break;
    case SET_SOAK_TIME: // Settings option to set soak time
      setSoakTimeLoop();
      break;
    case SET_DRY_TIME: // Settings option to set dry time
      setDryTimeLoop();
      break;
    case SET_BRIGHTNESS: // Settings option to set brightness of lcd
      setBrightnessLoop();
      break;
    defa