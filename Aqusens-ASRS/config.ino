// motor.ino
#define REEL_RAD_CM           (5.0f)
#define PULSE_PER_REV         (1600)
#define GEAR_RATIO            (5.0f)

// position.ino
#define NARROW_TUBE_CM          (15.0f) 
#define TUBE_CM                 (85.0f) 
#define WATER_LEVEL_CM          (15.0f) 
#define MIN_RAMP_DIST_CM        (NARROW_TUBE_CM + TUBE_CM + WATER_LEVEL_CM + 5.0f)
#define DROP_SPEED_CM_SEC       (15.0f)
#define RAISE_SPEED_CM_SEC      (5.0f)

// tube_flush.ino
// timings
#define LIFT_TUBE_TIME_S        (0.5f)
#define DUMP_WATER_TIME_S       (5UL)
#define ROPE_DROP_TIME_S        (40.0f / 15.0f)
#define RINSE_ROPE_TIME_S       (20.0f)
#define RINSE_TUBE_TIME_S       (5UL)

// aqusens timings 
#define AIR_GAP_TIME_S          (5)
#define WATER_RINSE_TIME_S      (15)
#define LAST_AIR_GAP_TIME_S     (10)

// ========================================================================
// Struct Definitions
// ========================================================================

typedef struct MotorConfig_t {
    float reel_radius_cm;
    float gear_ratio;
    unsigned int pulse_per_rev;
} MotorConfig_t;

typedef struct PositionConfig_t {
    float narrow_tube_cm;
    float tube_cm;
    float water_level_cm;
    float min_ramp_dist_cm;
    float drop_speed_cm_sec;
    float raise_speed_cm_sec;
    float drop_speeds[4];
    float raise_speeds[4];
} PositionConfig_t;

typedef struct FlushTimeConfig_t {
    float lift_tube_time_s;
    unsigned long dump_water_time_s;
    float rope_drop_time_s;
    float rinse_rope_time_s;
    unsigned long rinse_tube_time_s;
} FlushTimeConfig_t;

typedef struct AqusensTimeConfig_t {
    unsigned long air_gap_time_s;
    unsigned long water_rinse_time_s;
    unsigned long last_air_gap_time_s;
} AqusensTimeConfig_t;

typedef struct FlushConfig_t {
    FlushTimeConfig_t* flush_time_cfg;
    AqusensTimeConfig_t* aqusens_time_cfg;
} FlushConfig_t;

typedef struct GlobalConfig_t {
    MotorConfig_t& motor_cfg;
    PositionConfig_t& position_cfg;
    FlushConfig_t& flush_cfg;
} GlobalConfig_t;

// ========================================================================
// Default Values
// ========================================================================

MotorConfig_t DEF_MOTOR_CFG = {REEL_RAD_CM, GEAR_RATIO, PULSE_PER_REV};

PositionConfig_t DEF_POSITION_CFG = {
    NARROW_TUBE_CM,
    TUBE_CM,
    WATER_LEVEL_CM,
    MIN_RAMP_DIST_CM,
    DROP_SPEED_CM_SEC,
    RAISE_SPEED_CM_SEC,
    {15.0f, 30.0f, 75.0f, 30.0f},
    {25.0f, 50.0f, 10.0f, 1.5f}
};

FlushTimeConfig_t DEF_FLUSH_TIME_CFG = {
    LIFT_TUBE_TIME_S,
    DUMP_WATER_TIME_S,
    ROPE_DROP_TIME_S,
    RINSE_ROPE_TIME_S,
    RINSE_TUBE_TIME_S
};
AqusensTimeConfig_t DEF_AQS_TIME_CFG = {
    AIR_GAP_TIME_S,
    WATER_RINSE_TIME_S,
    LAST_AIR_GAP_TIME_S
};
FlushConfig_t DEF_FLUSH_CFG = {&DEF_FLUSH_TIME_CFG, &DEF_AQS_TIME_CFG};

GlobalConfig_t gbl_cfg = {DEF_MOTOR_CFG, DEF_POSITION_CFG, DEF_FLUSH_CFG};


// ========================================================================
// Function Prototypes
// ========================================================================

GlobalConfig_t& getGlobalCfg();
void setMotorCfg(MotorConfig_t& cfg);
void setPositionCfg(PositionConfig_t& cfg);
void setFlushCfg(FlushConfig_t& cfg);

// ========================================================================
// Functions
// ========================================================================

GlobalConfig_t& getGlobalCfg() {
    return gbl_cfg;
}

// needs to be call before everything
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
    setFlushCfg(gbl_cfg.flush_cfg);
}



