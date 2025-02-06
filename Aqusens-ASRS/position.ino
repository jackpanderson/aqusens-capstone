#define NUM_PHASES              (4UL)
#define NARROW_TUBE_CM          (15.0f) // TODO: replace with actual val
#define TUBE_CM                 (85.0f) // TODO: replace with actual val
#define WATER_LEVEL_CM          (15.0f) // TODO: replace with actual val
#define CONST_DIST_CM           (NARROW_TUBE_CM + TUBE_CM + WATER_LEVEL_CM)
#define FREE_FALL_IND           (2)
#define MIN_RAMP_DROP_DIST_CM   (CONST_DIST_CM)
#define DROP_SPEED_CM_SEC       (15.0f)

#define SAFE_RISE_SPEED_CM_SEC  (3.0f)
#define SAFE_DROP_DIST_CM       (10.0f)

void home_tube() {
  tube_position_f = 0;
  if (magSensorRead()) return;

  while (!drop_tube(SAFE_DROP_DIST_CM));

  setMotorSpeed(SAFE_RISE_SPEED_CM_SEC);
  while (!magSensorRead());
  turnMotorOff(true);
}

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

bool drop_tube(unsigned int distance_cm) {
  static bool dropping_flag = false;
  static bool small_drop = false;
  static unsigned long prev_time;
  static unsigned int drop_distance_cm;
  static size_t phase_ind;
  
  static float speeds_cm_p_s[NUM_PHASES] = {15.0f, 30.0f, 75.0f, 30.0f};
  static float dists_cm[NUM_PHASES] = {NARROW_TUBE_CM, TUBE_CM + NARROW_TUBE_CM, 0.0f, 0.0f};


  if (!dropping_flag) {
    dropping_flag = true;
    tube_position_f = 0.0f;
    drop_distance_cm = distance_cm;

    if (distance_cm <= MIN_RAMP_DROP_DIST_CM)  {
      small_drop = true;
      setMotorSpeed(-DROP_SPEED_CM_SEC);
    } else {
      phase_ind = 0;
      dists_cm[FREE_FALL_IND] = distance_cm - WATER_LEVEL_CM; 
      dists_cm[FREE_FALL_IND + 1] = distance_cm;
      setMotorSpeed(-speeds_cm_p_s[phase_ind]);
    }

    prev_time = millis();
  }

  // running integral
  unsigned long cur_time = millis();
  unsigned long delta_time = cur_time - prev_time;
  prev_time = cur_time;

  if (small_drop) {
    tube_position_f += (delta_time * DROP_SPEED_CM_SEC) / 1000.0f;
  } 
  else {
    tube_position_f += (delta_time * speeds_cm_p_s[phase_ind]) / 1000.0f;
    if (tube_position_f >= dists_cm[phase_ind]) {
      phase_ind++;
      setMotorSpeed(-speeds_cm_p_s[phase_ind]);
    }
  }

  if (tube_position_f >= drop_distance_cm) {
    turnMotorOff();
    dropping_flag = false;
    small_drop = false;
    return true;
  }

  return false;
}

bool retrieve_tube(unsigned int distance_cm) {
  static bool raise_flag = false;
  static unsigned long prev_time;
  static unsigned int raise_distance_cm;
  static unsigned int drop_time_ms;
  static size_t phase_ind;
  
  static float speeds_cm_p_s[NUM_PHASES] = {25.0f, 50.0f, 10.0f, 1.5f};
  static float dists_cm[NUM_PHASES] = {0.0f, TUBE_CM + NARROW_TUBE_CM, NARROW_TUBE_CM, 0.0f};


  if (!raise_flag) {
    raise_flag = true;
    phase_ind = 0;
    distance_cm += 2; // add some padding
    tube_position_f = distance_cm;
    
    raise_distance_cm = distance_cm;

    dists_cm[0] = distance_cm - WATER_LEVEL_CM;

    prev_time = millis();
    setMotorSpeed(speeds_cm_p_s[phase_ind]);
  }

  // running integral
  unsigned long cur_time = millis();
  unsigned long delta_time = cur_time - prev_time;
  prev_time = cur_time;

  tube_position_f -= (delta_time * speeds_cm_p_s[phase_ind]) / 1000.0f;

  if (magSensorRead()) {
    turnMotorOff();
    tube_position_f = 0;
    raise_flag = false;
    
    Serial.println("Hit mag sens");
    return true;
  } else if (tube_position_f <= 0) {
    raise_flag = false;
    turnMotorOff();
    
    Serial.println("Went too far ALARM");
    return true;
  }

  if (tube_position_f <= dists_cm[phase_ind]) {
    phase_ind++;
    setMotorSpeed(speeds_cm_p_s[phase_ind]);
  }

  return false;
}




