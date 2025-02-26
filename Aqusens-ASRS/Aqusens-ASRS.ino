//  -Aqusens-ASRS
//  -Aqusens - Automated Sample Retrevial System V1.0
//  -Date of Last Revision: 2/26/25
//  -Califonia Polytechnic State University
//  -Bailey College of Science and Mathamatics Biology Department
//  -Primary Owner: Alexis Pasulka
//  -Design Engineers: Doug Brewster and Rob Brewster
//  -Contributors: Sarah Martin, Deeba Khosravi, Emma Lucke, Jack Anderson, Jorge Ramirez, Danny ("the one, the only")
//  -Microcontroller: P1AM-100 ProOpen 
//  -Arduino IDE version:2.3.4
//  -See User Manual For Project Description
//  -non-stock libraries needed: (add library reference here)

//* libraries

#include <P1AM.h>
#include <SD.h>
#include <LiquidCrystal.h> //TODO: remove?
#include <LiquidCrystal_I2C.h>
#include <arduino-timer.h> //TODO: remove?
#include <RTCZero.h>
#include <SimpleKeypad.h>
#include <TimeLib.h>
#include <math.h>
#include "SAMD_PWM.h"
#include <ArduinoJson.h>

/* Pin Mapping ********************************************************************/

// Slots
#define HV_GPIO_SLOT 1                  // High voltage GPIO (P1-15CDD1)
#define RELAY_SLOT   2                  // Relay module (P1-04TRS)
#define RTD_SLOT     3                  // RTD Temp Sensor Module (P1-04RTD)

// Keypad
#define KEY_S 0
#define KEY_D 1
#define KEY_U 2
#define KEY_L 3
#define KEY_R 4

// SD card
#define SD_CS 28

//Motor
#define STEP_POS_PIN  6
#define DIR_POS_PIN   13
#define ALARM_PLUS A2  // Used as interrupt, should be high normally, low in case of motor alarm
#define ALARM_MINUS A5 // Keep High

// HV GPIO
#define MAG_SENSOR_IO_SLOT 1

// Relays
#define RELAY_SLOT 2
#define MOTOR_POWER 1
#define SOLENOID_ONE 3
#define SOLENOID_TWO 4

// RTD Inputs
typedef enum TempSensor {
  TEMP_SENSOR_ONE = 1, // TODO: Eventually change to more descriptive names, depending on what each sensor actually is measuring
  TEMP_SENSOR_TWO = 2
} TempSensor;

/* Variable Declarations *********************************************************/

// Machine States
enum StateEnum {
  CALIBRATE,
  STANDBY,
  RELEASE,
  SOAK,
  RECOVER,
  SAMPLE,
  FLUSH_TUBE,
  DRY,
  ALARM,
  MANUAL,
  MOTOR_CONTROL,
  SOLENOID_CONTROL,
  SETTINGS,
  SET_INTERVAL,
  ENSURE_SAMPLE_START,
  SET_START_TIME,
  SET_TUBE_FLUSH_TIME,
  SET_AQUSENS_FLUSH_TIME,
  ADD_EVENT,
  VIEW_EVENTS,
  SET_CLOCK,
  SET_DRY_TIME,
  SET_SOAK_TIME,
  FILTER_STATUS,
  SET_BRIGHTNESS,
  SET_CONTRAST      
};

typedef enum AlarmFault {
  MOTOR,
  TUBE,
  ESTOP,
} AlarmFault;

typedef enum SolenoidState {
  OPEN,
  CLOSED
} SolenoidState;

typedef enum MotorStatus {
  RAISING,
  LOWERING,
  MOTOR_OFF
} MotorStatus; // For lowering and raising the motor manually

volatile StateEnum state = STANDBY; 
volatile AlarmFault fault = ESTOP;

volatile uint32_t motor_pulses = 0;

SolenoidState solenoid_one_state = CLOSED;
SolenoidState solenoid_two_state = CLOSED;

// Timing
tmElements_t next_sample_time, sample_interval, soak_time, dry_time, tube_flush_time, aqusens_flush_time;
// Timer<5, millis> drop_timer;
volatile bool estop_pressed = false; // Flag to keep track of E-stop pressed/released

int8_t cursor_y = 2; // keeps track of current cursor position
uint8_t last_setting_page = 4; // amount of settings pages
uint8_t settings_page = 1; // current settings page

// RTC
RTCZero rtc;

//PWM
// SAMD_PWM* stepper; //With 50:1 gearbox, max stable speed is around 47000-50000 //TODO: outdated comment - not 50:1

//I2C LCD Screen
LiquidCrystal_I2C lcd(0x27, 20, 4);

// Tube position
float drop_distance_cm;
float tube_position_f; // Stores the current position of the sampler tube relative to the top of the tube in the home position

void export_cfg_to_sd();


/* Setup and Loop **************************************************************/
void setup() {
  Serial.begin(115200);
  while (!P1.init()) {} // Initialize controller

  init_cfg();

  rtcInit(); //TODO: add screen to input actual time/date to init rtc with
  RTDInit();
  gpioInit();
  estopInit();
  motorInit();

  lcd.init(); // Initialize the LCD
  lcd.backlight(); // Turn on the backlight
  lcd.setCursor(0, 0); // Set cursor to column 0, row 0

  state = CALIBRATE;
  
  Serial.println("[SETUP] done with init");
}

void loop() {
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
    case FLUSH_TUBE: // Aqusens and Sample device are flushed with filtered freshwater/air
      tubeFlushLoop();
      break;
    case DRY: // Sample device is dried for predetermined amount of time
      dryLoop();
      break;
    case ALARM: // Alarm mode is tripped due to E-stop press
      alarmLoop();
      break;
    case MANUAL: // Manual control of motor/solenoids, only entered from alarm mode
      manualLoop();
      break;
    case MOTOR_CONTROL:
      motorControlLoop();
      break;
    case SOLENOID_CONTROL:
      solenoidControlLoop();
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
    case SET_TUBE_FLUSH_TIME: // Settings option to set dry time
      setTubeFlushTimeLoop();
      break;
    case SET_AQUSENS_FLUSH_TIME: // Settings option to set dry time
      setAqusensFlushTimeLoop();
      break;
    default:
      break;
  }
}
