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
#define SYSTEM_CLOCK_FREQ 48000000
#define PRESCALER_VAL 16
#define REEL_RAD_CM         (2.7f)
#define PULSE_PER_REV       (8000)

volatile bool toggle = false;

void setMotorDir(MotorDir dir);


// TODO: Init Function
void motorInit() {
  //stepper = new SAMD_PWM(STEP_POS_PIN, DEFAULT_MOTOR_FREQ_Hz, 
  //                    DEFAULT_MOTOR_DC); // uses old lib
  // TODO: motor constructor
  pinMode(STEP_POS_PIN, OUTPUT);
}


// TODO: make just if else
void setMotorDir(MotorDir dir) {
  if (dir == CW) {
    digitalWrite(DIR_POS_PIN, HIGH);
  } else {
    digitalWrite(DIR_POS_PIN, LOW);
  }
}

// TODO: does this work for negative
inline uint32_t speed_to_freq(float cm_per_sec) {
  return (cm_per_sec * PULSE_PER_REV) / (2 * PI * REEL_RAD_CM);
}

void setMotorSpeed(float cm_per_sec) {
  setMotorFreq(speed_to_freq(cm_per_sec));
}


void setMotorFreq(uint32_t frequency) {

    //Disables timer clock, disabling output
    if (frequency == 0) {
      GCLK->CLKCTRL.reg &= (~GCLK_CLKCTRL_CLKEN); 
      return;
    }

    // Enable clock for TC5
    GCLK->CLKCTRL.reg = GCLK_CLKCTRL_ID_TC4_TC5 |   // Timer TC5
                        GCLK_CLKCTRL_GEN_GCLK0 |   // Use GCLK0 (48MHz)
                        GCLK_CLKCTRL_CLKEN;
    while (GCLK->STATUS.bit.SYNCBUSY);

    // Reset TC5 before configuration
    TC5->COUNT16.CTRLA.reg = TC_CTRLA_SWRST;
    while (TC5->COUNT16.STATUS.bit.SYNCBUSY);

    //setPrescaler(frequency, prescaler, compareValue);
    // Configure TC5 with Waveform Generation Mode (e.g., Match Frequency)
    TC5->COUNT16.CTRLA.reg = TC_CTRLA_MODE_COUNT16 |  
                              TC_CTRLA_PRESCALER_DIV16| TC_CTRLA_ENABLE |
                            TC_CTRLA_WAVEGEN_MFRQ; // Match Frequency PWM 
    
    while (TC5->COUNT16.STATUS.bit.SYNCBUSY); 

    // Set compare value
    uint16_t compareVal = countValFromFreq(frequency);

    TC5->COUNT16.CC[0].reg = compareVal;
    while (TC5->COUNT16.STATUS.bit.SYNCBUSY);

    // Enable interrupt on match
    enableTimerInterrupt();

    // Enable TC5
    TC5->COUNT16.CTRLA.bit.ENABLE = 1;
    while (TC5->COUNT16.STATUS.bit.SYNCBUSY);

    return;
}

uint16_t countValFromFreq(uint16_t frequency) 
{
  return round(((SYSTEM_CLOCK_FREQ/(PRESCALER_VAL*2)) / frequency) - 1);
}

// Enable TC5 interrupt
void enableTimerInterrupt() {
    NVIC_EnableIRQ(TC5_IRQn);
    TC5->COUNT16.INTENSET.reg = TC_INTENSET_MC(1);  // Enable Match/Compare 0 interrupt
}

// ISR for Timer/Counter 5
void TC5_Handler() {
    if (TC5->COUNT16.INTFLAG.bit.MC0) {  // Match/Compare 0 interrupt
        TC5->COUNT16.INTFLAG.reg = TC_INTFLAG_MC(1);  // Clear interrupt flag

        // Toggle the output pin
        toggle = !toggle;
        
        if (toggle == 1) {
          motorPulses++;
        }

        digitalWrite(STEP_POS_PIN, toggle);
    }
}

