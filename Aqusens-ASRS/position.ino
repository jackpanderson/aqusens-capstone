#define DROP_DISTANCE_IN    (6)     //! comment out if want to drop feet

#ifdef DROP_DISTANCE_IN
#define DROP_DISTANCE_FT    (DROP_DISTANCE_IN / 12.0f)
#else
#define DROP_DISTANCE_FT    (20)
#endif

#define DROP_DISTANCE_M     (DROP_DISTANCE_FT * 3.28084f)
#define DROP_DISTANCE_CM    (DROP_DISTANCE_M * 100)

#define REEL_RAD_CM         (5.0f)
#define PULSE_PER_REV       (1600)
#define PUL_TOLERANCE       (50)
#define GEARBOX_RATIO       (50) // Gearbox is 50:1, i.e. 50 stepper revoltuions = 1 gearbox drum rotation

#define MOTOR_STEPS         (3)
#define NUM_VEL_BINS        (2 * MOTOR_STEPS + 1)

typedef enum {
    RAMPING_UP,
    CONSTANT,
    SLOWING_DOWN
} SpeedState;

inline float vel_reel(unsigned int motor_freq);
void init_pos_tracking();
// void home_tube();
void init_timer(SpeedState state);
bool drop_motor(unsigned int& cur_position);
void TC4_Handler();

static volatile bool sample_flag = false;
static volatile bool dropping_flag = false;

inline float vel_reel(unsigned int motor_freq) {
    return (2 * PI * motor_freq * REEL_RAD_CM) / (PULSE_PER_REV * GEARBOX_RATIO);
}

void init_pos_tracking() {
    // TODO: 
    // setup timer values
    // add IRQ handler
    // bring back tube to home position
}

void home_tube() {
    /**
     *  drop reel some amount
     *  while (!mag_switch)
     *      reel up slowly
     */
  
  while (!drop_tube(10));
  
  // if (magSensorRead()) {
    // tube_position = 0;
  // }

  setMotorSpeed(2);     // TODO: what is the dropping speed
  while (!magSensorRead());
  turnMotorOff(true);
  tube_position = 0;
}


// ? maybe pass in pos_var so user knows exactly how much fallen?
// ? what happens if estop is pressed in the middle
// bool drop_motor(unsigned int& cur_position) {
//     static int bin_ind; 
//     static unsigned int start_cnt;
//     static unsigned int prev_cnt;

//     // start the drop
//     if (!dropping_flag) {
//         dropping_flag = true;
//         cur_position = 0;
//         start_cnt = 0; // FIXME: either this or get count from the timer
//         bin_ind = 0;
//         // set motor speed
//         init_timer(RAMPING_UP);
//     }

//     // not done getting into position
//     if (!sample_flag) return false; // early leave
//     sample_flag = false;    // clear flag

//     unsigned int cur_cnt = 0; // get count from timer
//     unsigned int motor_freq = 0; // FIXME: get_motor_freq
//     unsigned int pulses = cur_cnt - prev_cnt; // FIXME: get_timer_diff
//     // running integral
//     cur_position += vel_reel(motor_freq) * (pulses + PUL_TOLERANCE);
//     // set motor speed

//     // ? change to cur > drop_dis
//     // if here vel vs time graph should look like
//     /**
//      *      cm/s 
//      *      ^
//      *      |
//      *      |         ________________
//      *      |      __|                |__
//      *      |   __|                      |__
//      *      |__|                            |__
//      *      |                                  |
//      *      --------------------------------------> s
//      *      |--|
//      *       t is set by timer
//      */

//     if (bin_ind == NUM_VEL_BINS - 1) {
//         dropping_flag = false;
//         return true;
//     }

//     return false;
// }

#define DROP_SPEED_CM_SEC      (5.0f)
bool drop_tube(unsigned int distance_cm) {  
  // start the drop
  static bool dropping_flag = false;
  static unsigned long start_time;
  static unsigned int drop_distance_cm;
  static unsigned int drop_time_ms;
  
  if (!dropping_flag) {
      dropping_flag = true;
      tube_position = 0;                    // reset pos
      start_time = millis();                // FIXME: either this or get count from the timer
      setMotorSpeed(-DROP_SPEED_CM_SEC);     // TODO: what is the dropping speed
      drop_distance_cm = distance_cm;       // lock in the distance
      drop_time_ms = distance_cm / DROP_SPEED_CM_SEC * 1000;    // cnt to compare against
  }

  // elasped time would be coming from pwm timer?
  unsigned long elasped_time = millis() - start_time;
  tube_position = elasped_time * DROP_SPEED_CM_SEC / 1000.0f;
  // Serial.print("drop ");
  // Serial.println(tube_position); 
  if (elasped_time > drop_time_ms) {
    // Serial.println
    turnMotorOff();
    dropping_flag = false;
    return true;
  }

  return false;
}

// TODO: implement mag switch for homing;
#define RAISE_SPEED_CM_SEC      (5.0f)
bool raise_tube(unsigned int distance_cm) {  
  static bool raise_flag = false;
  static unsigned long start_time;
  static unsigned int drop_distance_cm;
  static unsigned int drop_time_ms;
  
  if (!raise_flag) {
      // TODO: if distance is greater than position so no buckling
      raise_flag = true;
      tube_position = 0;                    // reset pos
      start_time = millis();                // FIXME: either this or get count from the timer
      setMotorSpeed(RAISE_SPEED_CM_SEC);     // TODO: what is the dropping speed
      drop_distance_cm = distance_cm;       // lock in the distance
      drop_time_ms = distance_cm / RAISE_SPEED_CM_SEC * 1000;    // cnt to compare against
  }

  // elasped time would be coming from pwm timer?
  unsigned long elasped_time = millis() - start_time;
  tube_position += elasped_time / 1000 * RAISE_SPEED_CM_SEC; 
  if (elasped_time > drop_time_ms) {
  // if (tube_postition <) {
    turnMotorOff();
    raise_flag = false;
    return true;
  }

  return false;
}




