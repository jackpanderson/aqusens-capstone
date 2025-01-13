// Constants and Enums
typedef enum MotorDir {
  CCW,
  CW,
  OFF,
} MotorDir;

#define DEFAULT_MOTOR_FREQ_Hz   (500)
#define DEFAULT_MOTOR_DC        (0)
#define MOTOR_OFF(x)            (x == 0)
#define MAX_MOTOR_FREQ          (300000)

void setMotorDir(MotorDir dir);


// TODO: Init Function
void motorInit() {
  stepper = new SAMD_PWM(STEP_POS_PIN, DEFAULT_MOTOR_FREQ_Hz, 
                      DEFAULT_MOTOR_DC);
  // stepper->setPWM(STEP_POS_PIN, 3000, 50); // this doesnt work
  // stepper->setPWM(STEP_POS_PIN, 500, 50); // this works
}


// TODO: make just if else
void setMotorDir(MotorDir dir) {
  // digitalWrite(DIR_POS_PIN, dir == CW);
  
  if (dir == CW) {
    digitalWrite(DIR_POS_PIN, HIGH);
  } else {
    digitalWrite(DIR_POS_PIN, LOW);
  }
}

// TODO: what unit should speed be?
// right now speed is -100% - 100% of max freq
void setMotorSpeed(int rpm) {
  stepper->setPWM(STEP_POS_PIN, DEFAULT_MOTOR_FREQ_Hz, DEFAULT_MOTOR_DC);
  
  if (rpm > 0) {
    setMotorDir(CW);
    stepper->setPWM(STEP_POS_PIN, rpm * 2, 50); 
    Serial.println(rpm);
  } else if (rpm < 0) {
    setMotorDir(CCW);
    stepper->setPWM(STEP_POS_PIN, -rpm * 2, 50);
  }


}