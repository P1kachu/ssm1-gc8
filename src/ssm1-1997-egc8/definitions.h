#pragma once

/*
 *  Brown cable: Pin 0 (Serial RX --> Subaru TX --> OBD-II 13)
 *  Red cable:   Pin 1 (Serial TX --> Subaru RX --> OBD-II 12)
 */

#define HWSerial Serial1
#define USBSerial Serial
#define LED_PIN 13
#define NUM_READ_TRIES 3
#define PRINT_DEBUG_MESSAGES_ON_USB 0
#define DATA_NOT_AVAILABLE_CONSTANT 123456789
#define TEST 0


/*
 *  This allows modes to be selected to prioritize
 *  certain values over others.
 *
 * DRIVING_MODE_NORMAL is for regular driving with most values being refreshed
 * DRIVING_MODE_SPORT is for GoPro visualization, with only speed, RPM, throttle and coolant
 * DRIVING_MODE_SPEED_ONLY is for speed limiter testing, with high precision speed updates
 */
enum DRIVING_MODES {
        DRIVING_MODE_NORMAL,
        DRIVING_MODE_SPORT,
        DRIVING_MODE_SPEED_ONLY,
        DRIVING_MODE_MAX,
};

/*
 * We don't need the coolant temperature everytime, so
 * let's just get it once in REFRESH_COUNTER loops
 */
#define LEVEL_1_REFRESH 1
#define LEVEL_2_REFRESH 2
#define LEVEL_3_REFRESH 6
#define LEVEL_4_REFRESH 12
#define LEVEL_5_REFRESH 24
#define LEVEL_6_REFRESH 100

// Used addresses
#define BATTERY_VOLTAGE_ADDR      0x0007
#define SPEED_ADDR                0x0008
#define RPM_ADDR                  0x0009
#define COOLANT_ADDR              0x000A
#define AIRFLOW_SENSOR_ADDR       0x000C
#define THROTTLE_ADDR             0x000F
#define MANIFOLD_PRESSURE_ADDR    0x0020
#define BOOST_SOLENOID_ADDR       0x0022

// Not yet used ones
#define IGNITION_ADVANCE_ADDR     0x000B
#define ENGINE_LOAD_ADDR          0x000D
#define INJECTOR_PULSE_WIDTH_ADDR 0x0010
#define ISU_DUTY_VALVE_ADDR       0x0011
#define O2_AVERAGE_ADDR           0x0012
#define O2_MINIMUM_ADDR           0x0013
#define O2_MAXIMUM_ADDR           0x0014
#define KNOCK_CORRECTION_ADDR     0x0015
#define AF_CORRECTION_ADDR        0x001C
#define ATMOSPHERIC_PRESSURE_ADDR 0x001F

// To avoid the --unused-variable flag
#define UNUSED_VARIABLE(x) x=x

extern int driving_mode;

