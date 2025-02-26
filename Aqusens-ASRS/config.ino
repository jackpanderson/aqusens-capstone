// motor.ino
#define REEL_RAD_CM           (5.0f)
#define PULSE_PER_REV         (1600)
#define GEAR_RATIO            (5.0f)

// position.ino
#define NARROW_TUBE_CM          (15.0f) // TODO: replace with actual val
#define TUBE_CM                 (85.0f) // TODO: replace with actual val
#define WATER_LEVEL_CM          (15.0f) // TODO: replace with actual val
#define MIN_RAMP_DIST_CM        (NARROW_TUBE_CM + TUBE_CM + WATER_LEVEL_CM + 5.0f)
#define DROP_SPEED_CM_SEC       (15.0f)
#define RAISE_SPEED_CM_SEC      (5.0f)

typedef struct MotorConfig_t {
  float reel_radius_cm;
  float gear_ratio;
  unsigned int pulse_per_rev;
} MotorConfig_t;
MotorConfig_t DEF_MOTOR_CFG = {REEL_RAD_CM, GEAR_RATIO, PULSE_PER_REV};

typedef struct PositionConfig_t {
    float narrow_tube_cm;
    float tube_cm;
    float water_level_cm;
    float min_ramp_dist_cm;
    float drop_speed_cm_sec;
    float raise_speed_cm_sec;
} PositionConfig_t;
PositionConfig_t DEF_POSITION_CFG = {
    NARROW_TUBE_CM,
    TUBE_CM,
    WATER_LEVEL_CM,
    MIN_RAMP_DIST_CM,
    DROP_SPEED_CM_SEC,
    RAISE_SPEED_CM_SEC
};

typedef struct GlobalConfig_t {
  MotorConfig_t& motor_cfg;
  PositionConfig_t& position_cfg;
} GlobalConfig_t;

GlobalConfig_t gbl_cfg = {DEF_MOTOR_CFG, DEF_POSITION_CFG};

// func protos
GlobalConfig_t& getGlobalCfg();
void setMotorCfg(MotorConfig_t& cfg);
void setPositionCfg(PositionConfig_t& cfg);


GlobalConfig_t& getGlobalCfg() {
  return gbl_cfg;
}


void init_cfg() {
  char buf[300];
  snprintf(buf, sizeof(buf), "rr %f gr %f ppr %u",
             gbl_cfg.motor_cfg.reel_radius_cm,
             gbl_cfg.motor_cfg.gear_ratio,
             gbl_cfg.motor_cfg.pulse_per_rev);
  Serial.println(buf);

  // read JSON from sd

  // set up the config to devices

  setMotorCfg(gbl_cfg.motor_cfg);
  setPositionCfg(gbl_cfg.position_cfg);
}



