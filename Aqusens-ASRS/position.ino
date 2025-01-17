#define DROP_DISTANCE_IN    (6)     //! comment out if want to drop feet

#ifdef DROP_DISTANCE_IN
#define DROP_DISTANCE_FT    (DROP_DISTANCE_IN / 12.0f)
#else
#define DROP_DISTANCE_FT    (20)
#endif

#define DROP_DISTANCE_M     (DROP_DISTANCE_FT * 3.28084f)
#define DROP_DISTANCE_CM    (DROP_DISTANCE_M * 100)

#define REEL_RAD_CM         (2.7f)
#define PULSE_PER_REV       (8000)
// #define FREQ_TIMER          (1000) // 1kHz
#define PUL_TOLERANCE       (50)

#define MOTOR_STEPS         (3)
#define NUM_VEL_BINS        (2 * MOTOR_STEPS + 1)

typedef enum {
    RAMPING_UP,
    CONSTANT,
    SLOWING_DOWN
} SpeedState;

inline float vel_reel(unsigned int motor_freq);
void init_pos_tracking();
void home_tube();
void init_timer(SpeedState state);
bool drop_motor(unsigned int& cur_position);
void TC4_Handler();

static volatile bool sample_flag = false;
static volatile bool dropping_flag = false;

inline float vel_reel(unsigned int motor_freq) {
    return 2 * PI * motor_freq / PULSE_PER_REV;
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
}

// TODO:
void init_timer(SpeedState state) {
    // clear timer

    // set time
    switch (state) {
        case RAMPING_UP:
          return;
        case CONSTANT:
          return;
        case SLOWING_DOWN:
          return;
    }

    // start the counter
}

// ? maybe pass in pos_var so user knows exactly how much fallen?
// ? what happens if estop is pressed in the middle
bool drop_motor(unsigned int& cur_position) {
    static int bin_ind; 
    static unsigned int start_cnt;
    static unsigned int prev_cnt;

    // start the drop
    if (!dropping_flag) {
        dropping_flag = true;
        cur_position = 0;
        start_cnt = 0; // FIXME: either this or get count from the timer
        bin_ind = 0;
        // set motor speed
        init_timer(RAMPING_UP);
    }

    // not done getting into position
    if (!sample_flag) return false; // early leave
    sample_flag = false;    // clear flag

    unsigned int cur_cnt = 0; // get count from timer
    unsigned int motor_freq = 0; // FIXME: get_motor_freq
    unsigned int pulses = cur_cnt - prev_cnt; // FIXME: get_timer_diff
    // running integral
    cur_position += vel_reel(motor_freq) * (pulses + PUL_TOLERANCE);
    // set motor speed

    // ? change to cur > drop_dis
    // if here vel vs time graph should look like
    /**
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
     */

    if (bin_ind == NUM_VEL_BINS - 1) {
        dropping_flag = false;
        return true;
    }

    return false;
}

void TC4_Handler() {
    sample_flag = true;
}




