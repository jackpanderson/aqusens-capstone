#define DUMP_WATER_TIME_S       (5UL)
#define DROP_TUBE_DIST_CM       (40.0f)

typedef enum FlushState {
  INIT,
  DUMP_1,
  DELAY_1,
  DROP,
  HOME_1,
  DUMP_2,
  DELAY_2,
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

  static FlushState state;

  static unsigned long start_time;
  unsigned long cur_time;

  switch(state) {
    case INIT:
      state = DUMP_1;
      break;

    case DUMP_1:
      setMotorSpeed(.5);
      
      if (!magSensorRead()) {
        turnMotorOff();
        start_time = millis();
        state = DELAY_1;
      }
      break;      

    case DELAY_1:
      cur_time = millis();

      if (cur_time - start_time > DUMP_WATER_TIME_MS) {
        state = DROP;
      }
      break;

    case DROP:
      if (drop_tube(DROP_TUBE_DIST_CM)) {
        // turn on flushing solenoids
        state = HOME_1; 
      }
      break;

    case HOME_1:
      setMotorSpeed(2);
      
      if (magSensorRead()) {
        turnMotorOff();
        // turn off solenoids
        state = DUMP_2;
      }
      break;

    case DUMP_2:
      setMotorSpeed(.5);
      
      if (!magSensorRead()) {
        turnMotorOff();
        start_time = millis();
        state = DELAY_2;
        // turn on solenoids
      }
      break;      

    case DELAY_2:
      cur_time = millis();

      if (cur_time - start_time > DUMP_WATER_TIME_MS) {
        // turn off solenoids
        state = HOME_2;
      }
      break;

    case HOME_2:
      setMotorSpeed(-2);
      
      if (magSensorRead()) {
        turnMotorOff();
        // turn off solenoids
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



