// Constants and Enums
typedef enum MotorDir {
  CCW,
  CW,
  OFF,
} MotorDir;

#define DEFAULT_MOTOR_FREQ_KHz  (500)
#define DEFAULT_MOTOR_DC        (0)
#define MOTOR_OFF(x)            (x == 0)
#define MAX_MOTOR_FREQ          (200000)

void setMotorDir(MotorDir dir);


// TODO: Init Function
void motorInit() {
  // stepper = new SAMD_PWM(STEP_POS_PIN, 500, 0);
  stepper = new SAMD_PWM(STEP_POS_PIN, DEFAULT_MOTOR_FREQ_KHz, 
                      DEFAULT_MOTOR_DC);

  setMotorSpeed(0);
}


// TODO: Function
void setMotorDir(MotorDir dir) {
  if (dir == CCW) {
    digitalWrite(DIR_POS_PIN, LOW);
  } else if (dir == CW) {
    digitalWrite(DIR_POS_PIN, HIGH);
  }
}

// TODO: what unit should speed be?
// right now speed is -100% - 100% of max freq
void setMotorSpeed(int speed) {
  stepper->setPWM(STEP_POS_PIN, DEFAULT_MOTOR_FREQ_KHz, DEFAULT_MOTOR_DC);
  
  if (speed > 0) {
    setMotorDir(CW);
    stepper->setPWM(STEP_POS_PIN, (float)(speed) / 100 * MAX_MOTOR_FREQ, 50);
  } else if (speed < 0) {
    setMotorDir(CCW);
    stepper->setPWM(STEP_POS_PIN, (float)(-speed) / 100 * MAX_MOTOR_FREQ, 50);
  }

}