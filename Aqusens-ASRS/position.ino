#define REEL_RAD_CM         (5.0f)
#define PULSE_PER_REV       (1600)
#define PUL_TOLERANCE       (50)
#define GEARBOX_RATIO       (50) // Gearbox is 50:1, i.e. 50 stepper revoltuions = 1 gearbox drum rotation

void home_tube() {
  tube_position = 0;
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

#define NUM_PHASES          (4UL)
// #define NARROW_TUBE_CM      (243UL)
// #define TUBE_CM             (183UL)
// #define WATER_LEVEL_CM      (121UL)
#define NARROW_TUBE_CM      (9.0f) // TODO: replace with actual val
#define TUBE_CM             (16.0f) // TODO: replace with actual val
#define WATER_LEVEL_CM      (5.0f) // TODO: replace with actual val
#define CONST_DIST_CM       (NARROW_TUBE_CM + TUBE_CM + WATER_LEVEL_CM)
// #define NARROW_TUBE_SPD     (5.0f)
// #define TUBE_SPD            (15.0f)
// #define FREE_FALL_SPD       (20.0f)
// #define WATER_LEVEL_SPD     (10.0f)
#define NARROW_TUBE_SPD     (3.0f)
#define TUBE_SPD            (8.0f)
#define FREE_FALL_SPD       (15.0f)
#define WATER_LEVEL_SPD     (5.0f)
#define FREE_FALL_IND       (2)

// FIXME: clean up
bool drop_tube_small(unsigned int distance_cm) {
  static bool dropping_flag = false;
  static unsigned long start_time;
  static unsigned int drop_distance_cm;
  static unsigned int drop_time_ms;
  #define DROP_SPEED_CM_SEC (3.0f)
  
  if (!dropping_flag) {
      dropping_flag = true;
      tube_position = 0;                    
      start_time = millis();                
      setMotorSpeed(-DROP_SPEED_CM_SEC);    
      drop_distance_cm = distance_cm;       
      drop_time_ms = distance_cm / DROP_SPEED_CM_SEC * 1000;
  }

  unsigned long elasped_time = millis() - start_time;
  tube_position = elasped_time * DROP_SPEED_CM_SEC / 1000.0f + 1;

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
  static unsigned int drop_time_ms;
  static size_t phase_ind;
  
  static float speeds_cm_p_s[NUM_PHASES] = {NARROW_TUBE_SPD, TUBE_SPD, FREE_FALL_SPD, WATER_LEVEL_SPD};
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

// TODO: implement mag switch for homing;
#define RAISE_SPEED_CM_SEC      (3.0f)
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
      drop_time_ms = distance_cm * 1000 / RAISE_SPEED_CM_SEC;    // cnt to compare against
  }

  // elasped time would be coming from pwm timer?
  unsigned long elasped_time = millis() - start_time;
  tube_position = drop_distance_cm - (elasped_time * RAISE_SPEED_CM_SEC / 1000.0f + 1);
  // TODO: check distance to make sure no overshooting along with reading the mag switch
  if (magSensorRead() || elasped_time > drop_time_ms) {
    turnMotorOff();
    raise_flag = false;
    return true;
  }

  return false;
}




