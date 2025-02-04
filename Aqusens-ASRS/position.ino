#define NUM_PHASES          (4UL)
#define NARROW_TUBE_CM      (20.0f) // TODO: replace with actual val
#define TUBE_CM             (20.0f) // TODO: replace with actual val
#define WATER_LEVEL_CM      (5.0f) // TODO: replace with actual val
#define CONST_DIST_CM       (NARROW_TUBE_CM + TUBE_CM + WATER_LEVEL_CM)
#define FREE_FALL_IND       (2)


void home_tube() {
  tube_position_f = 0;
  if (magSensorRead()) return;

  while (!drop_tube_small(5));

  setMotorSpeed(2);     // TODO: what is the dropping speed
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


// FIXME: clean up
bool drop_tube_small(unsigned int distance_cm) {
  static bool dropping_flag = false;
  static unsigned long start_time;
  static unsigned int drop_distance_cm;
  static unsigned int drop_time_ms;
  #define DROP_SPEED_CM_SEC (3.0f)
  
  if (!dropping_flag) {
      dropping_flag = true;
      tube_position_f = 0;                    
      start_time = millis();                
      setMotorSpeed(-DROP_SPEED_CM_SEC);    
      drop_distance_cm = distance_cm;       
      drop_time_ms = distance_cm / DROP_SPEED_CM_SEC * 1000;
  }

  unsigned long elasped_time = millis() - start_time;
  tube_position_f = elasped_time * DROP_SPEED_CM_SEC / 1000.0f + 1;

  if (elasped_time > drop_time_ms) {
    turnMotorOff();
    dropping_flag = false;
    return true;
  }

  return false;
}


bool drop_tube(unsigned int distance_cm) {
  static bool dropping_flag = false;
  static unsigned long prev_time;
  static unsigned int drop_distance_cm;
  static size_t phase_ind;
  
  static float speeds_cm_p_s[NUM_PHASES] = {10.0f, 20.0f, 40.0f, 20.0f};
  // static float speeds_cm_p_s[NUM_PHASES] = {5.0f, 10.0f, 20.0f, 15.0f};
  static float dists_cm[NUM_PHASES] = {NARROW_TUBE_CM, TUBE_CM + NARROW_TUBE_CM, 0.0f, 0.0f};


  if (!dropping_flag) {
    dropping_flag = true;
    tube_position_f = 0.0f;
    phase_ind = 0;
    drop_distance_cm = distance_cm;

    dists_cm[FREE_FALL_IND] = distance_cm - WATER_LEVEL_CM; 
    dists_cm[FREE_FALL_IND + 1] = distance_cm;

    prev_time = millis();
    setMotorSpeed(-speeds_cm_p_s[phase_ind]);
  }

  // running integral
  unsigned long cur_time = millis();
  unsigned long delta_time = cur_time - prev_time;
  prev_time = cur_time;

  tube_position_f += (delta_time * speeds_cm_p_s[phase_ind]) / 1000.0f;

  if (tube_position_f >= drop_distance_cm) {
    turnMotorOff();
    return true;
  }

  if (tube_position_f >= dists_cm[phase_ind]) {
    phase_ind++;
    setMotorSpeed(-speeds_cm_p_s[phase_ind]);
  }

  return false;
}

bool retrieve_tube(unsigned int distance_cm) {
  static bool raise_flag = false;
  static unsigned long prev_time;
  static unsigned int raise_distance_cm;
  static unsigned int drop_time_ms;
  static size_t phase_ind;
  
  static float speeds_cm_p_s[NUM_PHASES] = {20.0f, 30.0f, 8.0f, 2.0f};
  // static float speeds_cm_p_s[NUM_PHASES] = {5.0f, 15.0f, 8.0f, 2.0f};
  static float dists_cm[NUM_PHASES] = {0.0f, TUBE_CM + NARROW_TUBE_CM, NARROW_TUBE_CM, 0.0f};


  if (!raise_flag) {
    raise_flag = true;
    tube_position_f = distance_cm;
    phase_ind = 0;
    
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

  if (magSensorRead() || tube_position_f <= 0) {
    turnMotorOff();
    return true;
  }

  if (tube_position_f <= dists_cm[phase_ind]) {
    phase_ind++;
    setMotorSpeed(speeds_cm_p_s[phase_ind]);
  }

  return false;
}




