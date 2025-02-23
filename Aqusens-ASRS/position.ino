/**
 * @brief 
 * 
 * @file position.ino
 */

#define NUM_PHASES              (4UL)
#define NARROW_TUBE_CM          (15.0f) // TODO: replace with actual val
#define TUBE_CM                 (85.0f) // TODO: replace with actual val
#define WATER_LEVEL_CM          (15.0f) // TODO: replace with actual val
#define CONST_DIST_CM           (NARROW_TUBE_CM + TUBE_CM + WATER_LEVEL_CM)
#define FREE_FALL_IND           (2)
#define MIN_RAMP_DIST_CM        (CONST_DIST_CM)
#define DROP_SPEED_CM_SEC       (15.0f)
#define RAISE_SPEED_CM_SEC      (5.0f)

#define SAFE_RISE_SPEED_CM_SEC  (3.0f)
#define SAFE_DROP_DIST_CM       (10.0f)


/**
 * @brief returns tube to home position at constant speed
 * 
 * Re-homes the tube during initializing and calibrate state
 * 
 * TODO: refactor to check E-Stop
 * TODO: refactor to understand why turnMotorOff doesn't fully work
 */
void homeTube() {
  tube_position_f = 0;
  if (magSensorRead()) return;

  while (!dropTube(SAFE_DROP_DIST_CM));

  setMotorSpeed(SAFE_RISE_SPEED_CM_SEC);
  while (!magSensorRead());
  turnMotorOff(true);
  tube_position_f = 0;
}

/**
 * @brief drops tube with ramping function for given distance
 * 
 *      cm/s 
 *      ^
 *      |
 *      |         ________________
 *      |      __|                |__
 *      |   __|                      |__
 *      |__|                            |__
 *      |                                  |
 *      --------------------------------------> s
 *      |--|
 *       t is set by timer
 * 
 * @param distance_cm distance to drop in cm
 * @return true if the tube has finished dropping to given distance
 * @return false if the tube has not finished dropping
 */
bool dropTube(unsigned int distance_cm) {
  static bool dropping_flag = false;
  static bool small_drop = false;
  static unsigned long prev_time;
  static unsigned int drop_distance_cm;
  static size_t phase_ind;
  
  static float speeds_cm_p_s[NUM_PHASES] = {15.0f, 30.0f, 75.0f, 30.0f};
  static float dists_cm[NUM_PHASES] = {NARROW_TUBE_CM, TUBE_CM + NARROW_TUBE_CM, 0.0f, 0.0f};


  if (!dropping_flag) {
    dropping_flag = true;
    drop_distance_cm = distance_cm + tube_position_f;

    // small drop
    if (distance_cm <= MIN_RAMP_DIST_CM)  {
      small_drop = true;
      setMotorSpeed(-DROP_SPEED_CM_SEC);
    } 
    else {
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
    
    // ramp to next speed 
    if (tube_position_f >= dists_cm[phase_ind]) {
      phase_ind++;
      setMotorSpeed(-speeds_cm_p_s[phase_ind]);
    }
  }

  // reached drop distance
  if (tube_position_f >= drop_distance_cm) {
    turnMotorOff();
    dropping_flag = false;
    small_drop = false;
    return true;
  }

  return false;
}

/**
 * @brief retrieves tube with ramping function for given distance
 * 
 *      cm/s 
 *      ^
 *      |
 *      |         ________________
 *      |      __|                |__
 *      |   __|                      |__
 *      |__|                            |__
 *      |                                  |
 *      --------------------------------------> s
 *      |--|
 *       t is set by timer
 * 
 * @param distance_cm distance to retrieve in cm
 * @return true if the tube has finished retrieving to given distance
 * @return false if the tube has not finished retrieving
 * 
 * TODO: set up alarm state if magnetic sensor isn't read when distance is reached
 */
#define RAISE_DIST_PADDING_CM       (2.0f)
bool retrieveTube(float distance_cm) {
    static bool raise_flag = false;
    static bool small_raise = false;
    static unsigned long prev_time;
    static float raise_distance_cm;
    static unsigned int drop_time_ms;
    static size_t phase_ind;

    static float speeds_cm_p_s[NUM_PHASES] = {25.0f, 50.0f, 10.0f, 1.5f};
    static float dists_cm[NUM_PHASES] = {0.0f, TUBE_CM + NARROW_TUBE_CM, NARROW_TUBE_CM, 0.0f};


    if (!raise_flag) {
        raise_flag = true;
        raise_distance_cm = tube_position_f - distance_cm - RAISE_DIST_PADDING_CM;

        // small raise
        if (distance_cm <= MIN_RAMP_DIST_CM) {
            small_raise = true;
            setMotorSpeed(RAISE_SPEED_CM_SEC);
        }
        else {
            phase_ind = 0;
            dists_cm[0] = tube_position_f - WATER_LEVEL_CM;
            setMotorSpeed(speeds_cm_p_s[phase_ind]);
        }

        prev_time = millis();
    }

    // running integral
    unsigned long cur_time = millis();
    unsigned long delta_time = cur_time - prev_time;
    prev_time = cur_time;

    if (small_raise) {
        tube_position_f -= (delta_time * RAISE_SPEED_CM_SEC) / 1000.0f;
    } 
    else {
        tube_position_f -= (delta_time * speeds_cm_p_s[phase_ind]) / 1000.0f;

        // ramp to next speed
        if (tube_position_f <= dists_cm[phase_ind]) {
            phase_ind++;
            setMotorSpeed(speeds_cm_p_s[phase_ind]);
        }
    }

    // the mag sens GOOD :)
    if (magSensorRead()) {
        turnMotorOff();
        tube_position_f = 0;
        raise_flag = false;

        if (small_raise) small_raise = false;
        return true;
    } 
    // motor is still spinning and tube is not home :(
    else if (tube_position_f <= 0) {
        raise_flag = false;
        turnMotorOff();
        setAlarmFault(TUBE);

        return false;
    }

  return false;
}

/**
 * @brief Lift the tube to the leaking position
 * @note This function is BLOCKING!!!
 * @note Assumes that tube is in the home position. IDK what happens if it isnt
 */
static bool is_tube_up = false;
#define LIFT_SPEED_CM_S     (2.0f)
#define HOME_SPEED_CM_S     (-2.0f)
void liftup_tube() {
    if (is_tube_up) return; 

    int count = 0;
    setMotorSpeed(LIFT_SPEED_CM_S);
    while (1) {
        count += !magSensorRead(); 
        if (count > 100) {
            turnMotorOff();
            is_tube_up = true;
            return;
        }
    }
}


/**
 * @brief Returns the tube from leaking position to closed at home
 * @note This function is BLOCKING!!!
 * @note Assumes that tube is in the lifted up position. IDK what happens if it isnt
 */
void dropdown_tube() {
    if (!is_tube_up) return; 

    int count = 0;
    setMotorSpeed(HOME_SPEED_CM_S);
    while (1) {
        count += magSensorRead();
        if (count > 150) {
            turnMotorOff();
            is_tube_up = false;
            return;
        }
    }
}


