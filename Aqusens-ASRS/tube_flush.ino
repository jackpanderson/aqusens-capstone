// For some fun
// #define FUN
#ifdef FUN
#define TWEAK_TIME_MS           (500)
#endif

// flushing consts
#define DROP_TUBE_DIST_CM       (40.0f)
#define LIFT_SPEED_CM_S         (0.5f)
#define HOME_TUBE_SPD_CM_S      (2.0f)


// timings
#define LIFT_TUBE_TIME_S        (0.5f)
#define DUMP_WATER_TIME_S       (5UL)
#define ROPE_DROP_TIME_S        (DROP_TUBE_DIST_CM / 15) // FIXME: slow drop speed instead of 15
#define RINSE_ROPE_TIME_S       (DROP_TUBE_DIST_CM / HOME_TUBE_SPD_CM_S)
#define RINSE_TUBE_TIME_S       (5UL)


// aqusens timings TODO: config
// #define AIR_GAP_TIME_S          (15)
// #define LAST_AIR_GAP_TIME_S     (90)
// #define WATER_RINSE_TIME_S      (180)
#define AIR_GAP_TIME_S          (5)
#define WATER_RINSE_TIME_S      (15)
#define LAST_AIR_GAP_TIME_S     (10)

constexpr unsigned long FLUSH_TIME_S = (3 * LIFT_TUBE_TIME_S + DUMP_WATER_TIME_S + 
  ROPE_DROP_TIME_S + RINSE_ROPE_TIME_S + RINSE_TUBE_TIME_S);
constexpr unsigned long AQUSENS_TIME_S = 3 * AIR_GAP_TIME_S + 3 * WATER_RINSE_TIME_S + LAST_AIR_GAP_TIME_S;
constexpr unsigned long TOT_FLUSH_TIME_S = FLUSH_TIME_S + AQUSENS_TIME_S - 1;

typedef enum FlushState {
  INIT,
  DUMP_TUBE_1,
  DUMP_DELAY,
  ROPE_DROP,
  RINSE_ROPE_HOME,
  DUMP_TUBE_2,
  RINSE_TUBE,
  RINSE_AQUSENS,
  HOME_2,
} FlushState;

/**
 * @brief FSM to control flushing process (8 states)
 * 
 * @return true once done with the flushing cycle
 * @return false if error occurs during flushing cycle
 */
bool flushTube() {
  constexpr unsigned long DUMP_WATER_TIME_MS = DUMP_WATER_TIME_S * 1000;

  static FlushState state;
  static unsigned long start_time;
  unsigned long cur_time;

  switch(state) {
    case INIT:
      state = DUMP_TUBE_1;
      break;

    case DUMP_TUBE_1:
      setMotorSpeed(LIFT_SPEED_CM_S);
      
      if (!magSensorRead()) {
        turnMotorOff();
        start_time = millis();

        updateSolenoid(OPEN, SOLENOID_ONE);
        
        state = DUMP_DELAY;
      }
      break;      

    case DUMP_DELAY:
      cur_time = millis();

      if (cur_time - start_time > DUMP_WATER_TIME_MS) {
        state = ROPE_DROP;
      }
      break;

    case ROPE_DROP:
      if (dropTube(DROP_TUBE_DIST_CM)) {
        
        state = RINSE_ROPE_HOME; 
      }
      break;

    case RINSE_ROPE_HOME:
      setMotorSpeed(HOME_TUBE_SPD_CM_S);
      
      if (magSensorRead()) {
        turnMotorOff();
        
        state = DUMP_TUBE_2;
      }
      break;

    case DUMP_TUBE_2:
      setMotorSpeed(LIFT_SPEED_CM_S);
      
      if (!magSensorRead()) {
        turnMotorOff();
        start_time = millis();
        state = RINSE_TUBE;
      }
      break;      

    case RINSE_TUBE:
      cur_time = millis();

      if (cur_time - start_time > RINSE_TUBE_TIME_S) {
        start_time = millis();
        
        state = RINSE_AQUSENS;
      }
      break;

    case RINSE_AQUSENS:
      cur_time = millis();

      if (flushAqusens(cur_time)) {
        updateSolenoid(CLOSED, SOLENOID_ONE);
        updateSolenoid(CLOSED, SOLENOID_TWO);
        
        state = HOME_2;
      }
      break;

    case HOME_2:
      setMotorSpeed(-LIFT_SPEED_CM_S);
      
      if (magSensorRead()) {
        turnMotorOff();
        state = INIT;
        return true;
      }
      break;

    default:
      Serial.println("[FLUSH_TUBE] bad state. Shouldn't be here");      
      while(1);
  }

  return false;
}

typedef enum AqusensState {
  RINSE_INIT,
  AIR_1,
  WATER_1,
  AIR_2,
  WATER_2,
  AIR_3,
  WATER_3,
  LAST_AIR,
} AqusensState;

// TODO: implement time from the struct    
//  aqusens_flush_time.Minute = 0;
//  aqusens_flush_time.Second = 15;
// TODO: danny convert this to the ms and then fill the gaps
/**
 * @brief TODO: Danny - update this function comment
 * 
 * @param cur_time 
 * @return true 
 * @return false 
 */
bool flushAqusens(unsigned long cur_time) {
  static AqusensState state = RINSE_INIT;
  static unsigned long prev_time;
  
  #ifdef FUN
  static unsigned long prev_time_tweak;
  static bool sole_state = true;
  #endif

  constexpr unsigned long AIR_GAP_TIME_MS = AIR_GAP_TIME_S * 1000;
  constexpr unsigned long LAST_AIR_GAP_TIME_MS = LAST_AIR_GAP_TIME_S * 1000;
  constexpr unsigned long WATER_RINSE_TIME_MS = WATER_RINSE_TIME_S * 1000;

  switch (state) {
    case RINSE_INIT:
      updateSolenoid(CLOSED, SOLENOID_TWO);
      
      prev_time = millis();

      #ifdef FUN
      prev_time_tweak = prev_time;
      #endif

      Serial.println("going to air 1");
      state = AIR_1;
      break;

    case AIR_1:

      #ifdef FUN
      if (cur_time - prev_time_tweak >= TWEAK_TIME_MS) {
        prev_time_tweak = millis();

        sole_state = !sole_state;
        updateSolenoid(sole_state, SOLENOID_TWO);
      }

      #endif

      if (cur_time - prev_time >= AIR_GAP_TIME_MS) {
        prev_time = millis();

        updateSolenoid(OPEN, SOLENOID_TWO);

        Serial.println("going to water 1");
        state = WATER_1;
      }
      break;

    case WATER_1:
      if (cur_time - prev_time >= WATER_RINSE_TIME_MS) {
        prev_time = millis();

        updateSolenoid(CLOSED, SOLENOID_TWO);

        state = AIR_2;
        Serial.println("going to air 2");

      }
      break;
    
    case AIR_2:
      if (cur_time - prev_time >= AIR_GAP_TIME_MS) {
        prev_time = millis();

        updateSolenoid(OPEN, SOLENOID_TWO);

        state = WATER_2;
        Serial.println("going to water 2");

      }
      break;

    case WATER_2:
      if (cur_time - prev_time >= WATER_RINSE_TIME_MS) {
        prev_time = millis();

        updateSolenoid(CLOSED, SOLENOID_TWO);

        state = AIR_3;
        Serial.println("going to air 3");

      }
      break;
    
    case AIR_3:
      if (cur_time - prev_time >= AIR_GAP_TIME_MS) {
        prev_time = millis();

        updateSolenoid(OPEN, SOLENOID_TWO);

        state = WATER_3;
        Serial.println("going to water 3");

      }
      break;

    case WATER_3:
      if (cur_time - prev_time >= WATER_RINSE_TIME_MS) {
        prev_time = millis();

        updateSolenoid(CLOSED, SOLENOID_TWO);

        Serial.println("going to last air ");

        state = LAST_AIR;
        break;
      }

    case LAST_AIR:
      if (cur_time - prev_time >= LAST_AIR_GAP_TIME_MS) {

        state = RINSE_INIT;
        Serial.println("Finished aqusens ");

        return true;
      }
  }

  return false;
}

