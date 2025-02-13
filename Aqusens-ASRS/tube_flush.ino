#define DUMP_WATER_TIME_S       (5UL)
// #define AIR_GAP_TIME_S          (15)
// #define WATER_RINSE_TIME_S      (3 * 60)
#define AIR_GAP_TIME_S          (3)
#define LAST_AIR_GAP_TIME_S     (90)
#define WATER_RINSE_TIME_S      (10)
#define DROP_TUBE_DIST_CM       (40.0f)
#define LIFT_SPEED_CM_S         (0.5f)
#define HOME_TUBE_SPD_CM_S      (2.0f)
#define PULSE_TIME_MS           (3000UL)

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
bool flush_tube() {
  constexpr unsigned long DUMP_WATER_TIME_MS = DUMP_WATER_TIME_S * 1000;
  constexpr unsigned long AQUSENS_TIME_MS = (3 * AIR_GAP_TIME_S * 3 * WATER_RINSE_TIME_S) * 1000;

  static FlushState state;

  static unsigned long start_time;
  static bool solenoid_state = false;
  unsigned long cur_time, prev_time;

  switch(state) {
    case INIT:
      
      state = DUMP_TUBE_1;
      break;

    case DUMP_TUBE_1:
      setMotorSpeed(LIFT_SPEED_CM_S);
      
      if (!magSensorRead()) {
        turnMotorOff();
        start_time = millis();
        solenoidOneState = OPEN;
        updateSolenoid(solenoidOneState, SOLENOID_ONE);
        
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
      if (drop_tube(DROP_TUBE_DIST_CM)) {
        
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

      if (cur_time - start_time > DUMP_WATER_TIME_MS) {
        start_time = millis();
        
        state = RINSE_AQUSENS;
      }
      break;

    case RINSE_AQUSENS:
      cur_time = millis();

      if (flush_aqusens(cur_time)) {
        solenoidOneState = CLOSED;
        updateSolenoid(solenoidOneState, SOLENOID_ONE);
        
        solenoidTwoState = CLOSED;
        updateSolenoid(solenoidTwoState, SOLENOID_TWO);
        
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

typedef enum {
  RINSE_INIT,
  AIR_1,
  WATER_1,
  AIR_2,
  WATER_2,
  AIR_3,
  WATER_3,
  LAST_AIR,
} AquasensState;

// TODO: implement time from the struct    
//  aqusensFlushTime.Minute = 0;
//  aqusensFlushTime.Second = 15;
// TODO: danny convert this to the ms and then fill the gaps
bool flush_aqusens(unsigned long cur_time) {
  static AquasensState state = RINSE_INIT;
  static unsigned long prev_time;

  constexpr unsigned long AIR_GAP_TIME_MS = AIR_GAP_TIME_S * 1000;
  constexpr unsigned long WATER_RINSE_TIME_MS = WATER_RINSE_TIME_S * 1000;

  switch (state) {
    case RINSE_INIT:
      solenoidTwoState = CLOSED;
      updateSolenoid(solenoidTwoState, SOLENOID_TWO);
      
      prev_time = millis();

      state = AIR_1;
      break;

    case AIR_1:
      if (cur_time - prev_time >= AIR_GAP_TIME_MS) {
        prev_time = millis();

        solenoidTwoState = OPEN;
        updateSolenoid(solenoidTwoState, SOLENOID_TWO);

        state = WATER_1;
      }
      break;

    case WATER_1:
      if (cur_time - prev_time >= WATER_RINSE_TIME_MS) {
        prev_time = millis();

        solenoidTwoState = CLOSED;
        updateSolenoid(solenoidTwoState, SOLENOID_TWO);

        state = AIR_2;
      }
      break;
    
    case AIR_2:
      if (cur_time - prev_time >= AIR_GAP_TIME_MS) {
        prev_time = millis();

        solenoidTwoState = OPEN;
        updateSolenoid(solenoidTwoState, SOLENOID_TWO);

        state = WATER_2;
      }
      break;

    case WATER_2:
      if (cur_time - prev_time >= WATER_RINSE_TIME_MS) {
        prev_time = millis();

        solenoidTwoState = CLOSED;
        updateSolenoid(solenoidTwoState, SOLENOID_TWO);

        state = AIR_3;
      }
      break;
    
    case AIR_3:
      if (cur_time - prev_time >= AIR_GAP_TIME_MS) {
        prev_time = millis();

        solenoidTwoState = OPEN;
        updateSolenoid(solenoidTwoState, SOLENOID_TWO);

        state = WATER_3;
      }
      break;

    case WATER_3:
      if (cur_time - prev_time >= WATER_RINSE_TIME_MS) {
        prev_time = millis();

        solenoidTwoState = CLOSED;
        updateSolenoid(solenoidTwoState, SOLENOID_TWO);

        state = LAST_AIR;
        break;
      }

    case LAST_AIR:
      if (cur_time - prev_time >= LAST_AIR_GAP_TIME_S * 1000) {

        state = LAST_AIR;
        return true;
      }
  }

  return false;
}

