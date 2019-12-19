/*
 * mps.h
 *
 *  Created on: 18 Dec 2019
 *      Author: addod
 */

#ifndef LOCAL_INC_MPS_PROTOCOL_H_
#define LOCAL_INC_MPS_PROTOCOL_H_

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>


/*
 * Detailed information and understanding of the COMMANDS Please refer to
 * @link http://www.multiwii.com/wiki/index.php?title=Multiwii_Serial_Protocol
 *
 *
 *
 * The general format of an MSP message is:

           <preamble>,<direction>,<size>,<command>,,<crc>

        Where:

        preamble = the ASCII characters '$M'

        direction = the ASCII character '<' if to the MWC or '>' if from the MWC

        size = number of data bytes, binary. Can be zero as in the case of a data request to the MWC

        command = message_id as per the table below

        data = as per the table below. UINT16 values are LSB first.

        crc = XOR of <size>, <command> and each data byte into a zero'ed sum

 * */


// requests & replies
#define MSP_API_VERSION            1
#define MSP_FC_VARIANT             2
#define MSP_FC_VERSION             3
#define MSP_BOARD_INFO             4
#define MSP_BUILD_INFO             5
#define MSP_CALIBRATION_DATA      14
#define MSP_FEATURE               36
#define MSP_BOARD_ALIGNMENT       38
#define MSP_CURRENT_METER_CONFIG  40
#define MSP_RX_CONFIG             44
#define MSP_SONAR_ALTITUDE        58
#define MSP_ARMING_CONFIG         61
#define MSP_RX_MAP                64 // get channel map (also returns number of channels total)
#define MSP_LOOP_TIME             73 // FC cycle time i.e looptime parameter
#define MSP_STATUS               101
#define MSP_RAW_IMU              102
#define MSP_SERVO                103
#define MSP_MOTOR                104
#define MSP_RC                   105
#define MSP_RAW_GPS              106
#define MSP_COMP_GPS             107 // distance home, direction home
#define MSP_ATTITUDE             108
#define MSP_ALTITUDE             109
#define MSP_ANALOG               110
#define MSP_RC_TUNING            111 // rc rate, rc expo, rollpitch rate, yaw rate, dyn throttle PID
#define MSP_PID                  112 // P I D coeff
#define MSP_MISC                 114
#define MSP_SERVO_CONFIGURATIONS 120
#define MSP_NAV_STATUS           121 // navigation status
#define MSP_SENSOR_ALIGNMENT     126 // orientation of acc,gyro,mag
#define MSP_STATUS_EX            150
#define MSP_SENSOR_STATUS        151
#define MSP_BOXIDS               119
#define MSP_UID                  160 // Unique device ID
#define MSP_GPSSVINFO            164 // get Signal Strength (only U-Blox)
#define MSP_GPSSTATISTICS        166 // get GPS debugging data
#define MSP_SET_PID              202 // set P I D coeff


// commands
#define MSP_SET_HEAD             211 // define a new heading hold direction
#define MSP_SET_RAW_RC           200 // 8 rc chan
#define MSP_SET_RAW_GPS          201 // fix, numsat, lat, lon, alt, speed
#define MSP_SET_WP               209 // sets a given WP (WP#, lat, lon, alt, flags)


// bits of the Active Modes() return value
#define MSP_MODE_ARM          0
#define MSP_MODE_ANGLE        1
#define MSP_MODE_HORIZON      2
#define MSP_MODE_NAVALTHOLD   3
#define MSP_MODE_MAG          4
#define MSP_MODE_HEADFREE     5
#define MSP_MODE_HEADADJ      6
#define MSP_MODE_CAMSTAB      7
#define MSP_MODE_NAVRTH       8
#define MSP_MODE_NAVPOSHOLD   9
#define MSP_MODE_PASSTHRU    10
#define MSP_MODE_BEEPERON    11
#define MSP_MODE_LEDLOW      12
#define MSP_MODE_LLIGHTS     13
#define MSP_MODE_OSD         14
#define MSP_MODE_TELEMETRY   15
#define MSP_MODE_GTUNE       16
#define MSP_MODE_SONAR       17
#define MSP_MODE_BLACKBOX    18
#define MSP_MODE_FAILSAFE    19
#define MSP_MODE_NAVWP       20
#define MSP_MODE_AIRMODE     21
#define MSP_MODE_HOMERESET   22
#define MSP_MODE_GCSNAV      23
#define MSP_MODE_HEADINGLOCK 24
#define MSP_MODE_SURFACE     25
#define MSP_MODE_FLAPERON    26
#define MSP_MODE_TURNASSIST  27
#define MSP_MODE_NAVLAUNCH   28
#define MSP_MODE_AUTOTRIM    29


// MSP_API_VERSION reply
struct msp_api_version_t {
  uint8_t protocolVersion;
  uint8_t APIMajor;
  uint8_t APIMinor;
} __attribute__ ((packed));


// MSP_FC_VARIANT reply
struct msp_fc_variant_t {
  char flightControlIdentifier[4];
} __attribute__ ((packed));


// MSP_FC_VERSION reply
struct msp_fc_version_t {
  uint8_t versionMajor;
  uint8_t versionMinor;
  uint8_t versionPatchLevel;
} __attribute__ ((packed));


// MSP_BOARD_INFO reply
struct msp_board_info_t {
  char     boardIdentifier[4];
  uint16_t hardwareRevision;
} __attribute__ ((packed));


// MSP_BUILD_INFO reply
struct msp_build_info_t {
  char buildDate[11];
  char buildTime[8];
  char shortGitRevision[7];
} __attribute__ ((packed));


// MSP_RAW_IMU reply
struct msp_raw_imu_t {
  int16_t acc[3];  // x, y, z
  int16_t gyro[3]; // x, y, z
  int16_t mag[3];  // x, y, z
} __attribute__ ((packed));


// flags for msp status exit sensor and msp status sensor
#define MSP_STATUS_SENSOR_ACC    1
#define MSP_STATUS_SENSOR_BARO   2
#define MSP_STATUS_SENSOR_MAG    4
#define MSP_STATUS_SENSOR_GPS    8
#define MSP_STATUS_SENSOR_SONAR 16



// values for msp_nav_status_t.mode
#define MSP_NAV_STATUS_MODE_NONE   0
#define MSP_NAV_STATUS_MODE_HOLD   1
#define MSP_NAV_STATUS_MODE_RTH    2
#define MSP_NAV_STATUS_MODE_NAV    3
#define MSP_NAV_STATUS_MODE_EMERG 15

// values for msp_nav_status_t.state
#define MSP_NAV_STATUS_STATE_NONE                0  // None
#define MSP_NAV_STATUS_STATE_RTH_START           1  // RTH Start
#define MSP_NAV_STATUS_STATE_RTH_ENROUTE         2  // RTH Enroute
#define MSP_NAV_STATUS_STATE_HOLD_INFINIT        3  // PosHold infinit
#define MSP_NAV_STATUS_STATE_HOLD_TIMED          4  // PosHold timed
#define MSP_NAV_STATUS_STATE_WP_ENROUTE          5  // WP Enroute
#define MSP_NAV_STATUS_STATE_PROCESS_NEXT        6  // Process next
#define MSP_NAV_STATUS_STATE_DO_JUMP             7  // Jump
#define MSP_NAV_STATUS_STATE_LAND_START          8  // Start Land
#define MSP_NAV_STATUS_STATE_LAND_IN_PROGRESS    9  // Land in Progress
#define MSP_NAV_STATUS_STATE_LANDED             10  // Landed
#define MSP_NAV_STATUS_STATE_LAND_SETTLE        11  // Settling before land
#define MSP_NAV_STATUS_STATE_LAND_START_DESCENT 12  // Start descent

#define MSP_NAV_STATUS_WAYPOINT_ACTION_WAYPOINT 0x01
#define MSP_NAV_STATUS_WAYPOINT_ACTION_RTH      0x04

// values for msp_nav_status_t.error
#define MSP_NAV_STATUS_ERROR_NONE               0   // All systems clear
#define MSP_NAV_STATUS_ERROR_TOOFAR             1   // Next waypoint distance is more than safety distance
#define MSP_NAV_STATUS_ERROR_SPOILED_GPS        2   // GPS reception is compromised - Nav paused - copter is adrift !
#define MSP_NAV_STATUS_ERROR_WP_CRC             3   // CRC error reading WP data from EEPROM - Nav stopped
#define MSP_NAV_STATUS_ERROR_FINISH             4   // End flag detected, navigation finished
#define MSP_NAV_STATUS_ERROR_TIMEWAIT           5   // Waiting for poshold timer
#define MSP_NAV_STATUS_ERROR_INVALID_JUMP       6   // Invalid jump target detected, aborting
#define MSP_NAV_STATUS_ERROR_INVALID_DATA       7   // Invalid mission step action code, aborting, copter is adrift
#define MSP_NAV_STATUS_ERROR_WAIT_FOR_RTH_ALT   8   // Waiting to reach RTH Altitude
#define MSP_NAV_STATUS_ERROR_GPS_FIX_LOST       9   // Gps fix lost, aborting mission
#define MSP_NAV_STATUS_ERROR_DISARMED          10   // NAV engine disabled due disarm
#define MSP_NAV_STATUS_ERROR_LANDING           11   // Landing





//Range [1000;2000]
//ROLL/PITCH/YAW/THROTTLE/AUX1/AUX2/AUX3AUX4
#define MSP_MAX_SUPPORTED_CHANNELS 8

// MSP_SET_HEAD CMD
struct msp_set_head_t {
  int16_t magHoldHeading; // degrees
} __attribute__ ((packed));


// MSP_SET_RAW_RC CMD
struct msp_set_raw_rc_t {
  uint16_t channel[MSP_MAX_SUPPORTED_CHANNELS];
} __attribute__ ((packed));


// MSP_SET_PID command
//typedef msp_pid_t msp_set_pid_t;


// MSP_SET_RAW_GPS command
struct msp_set_raw_gps_t {
  uint8_t  fixType;       // MSP_GPS_NO_FIX, MSP_GPS_FIX_2D, MSP_GPS_FIX_3D
  uint8_t  numSat;
  int32_t  lat;           // 1 / 10000000 deg
  int32_t  lon;           // 1 / 10000000 deg
  int16_t  alt;           // meters
  int16_t  groundSpeed;   // cm/s
} __attribute__ ((packed));


// MSP_SET_WP command
// Special waypoints are 0 and 255. 0 is the RTH position, 255 is the POSHOLD position (lat, lon, alt).
struct msp_set_wp_t {
  uint8_t waypointNumber;
  uint8_t action;   // one of MSP_NAV_STATUS_WAYPOINT_ACTION_XXX
  int32_t lat;      // decimal degrees latitude * 10000000
  int32_t lon;      // decimal degrees longitude * 10000000
  int32_t alt;      // altitude (cm)
  int16_t p1;       // speed (cm/s) when action is MSP_NAV_STATUS_WAYPOINT_ACTION_WAYPOINT, or "land" (value 1) when action is MSP_NAV_STATUS_WAYPOINT_ACTION_RTH
  int16_t p2;       // not used
  int16_t p3;       // not used
  uint8_t flag;     // 0xa5 = last, otherwise set to 0
} __attribute__ ((packed));

typedef struct joycontrol {
  uint32_t speedUp;
  uint32_t speedDown;
  uint32_t azimuthDown;
} joycontrol __attribute__ ((packed));







#if 0
// See MSP_API_VERSION and MSP_MIXER_CONFIG
#define MSP_IDENT                100    //out message         mixerMode + multiwii version + protocol version + capability variable


#define MSP_STATUS               101    //out message         cycletime & errors_count & sensor present & box activation & current setting number
#define MSP_RAW_IMU              102    //out message         9 DOF
#define MSP_SERVO                103    //out message         servos
#define MSP_MOTOR                104    //out message         motors
#define MSP_RC                   105    //out message         rc channels and more
#define MSP_RAW_GPS              106    //out message         fix, numsat, lat, lon, alt, speed, ground course
#define MSP_COMP_GPS             107    //out message         distance home, direction home
#define MSP_ATTITUDE             108    //out message         2 angles 1 heading
#define MSP_ALTITUDE             109    //out message         altitude, variometer
#define MSP_ANALOG               110    //out message         vbat, powermetersum, rssi if available on RX
#define MSP_RC_TUNING            111    //out message         rc rate, rc expo, rollpitch rate, yaw rate, dyn throttle PID
#define MSP_PID                  112    //out message         P I D coeff (9 are used currently)

#define MSP_BOXNAMES             116    //out message         the aux switch names
#define MSP_PIDNAMES             117    //out message         the PID names
#define MSP_WP                   118    //out message         get a WP, WP# is in the payload, returns (WP#, lat, lon, alt, flags) WP#0-home, WP#16-poshold
#define MSP_BOXIDS               119    //out message         get the permanent IDs associated to BOXes
#define MSP_SERVO_CONFIGURATIONS 120    //out message         All servo configurations.
#define MSP_NAV_STATUS           121    //out message         Returns navigation status
#define MSP_NAV_CONFIG           122    //out message         Returns navigation parameters
#define MSP_MOTOR_3D_CONFIG      124    //out message         Settings needed for reversible ESCs
#define MSP_RC_DEADBAND          125    //out message         deadbands for yaw alt pitch roll
#define MSP_SENSOR_ALIGNMENT     126    //out message         orientation of acc,gyro,mag
#define MSP_LED_STRIP_MODECOLOR  127    //out message         Get LED strip mode_color settings
#define MSP_VOLTAGE_METERS       128    //out message         Voltage (per meter)
#define MSP_CURRENT_METERS       129    //out message         Amperage (per meter)
#define MSP_BATTERY_STATE        130    //out message         Connected/Disconnected, Voltage, Current Used
#define MSP_MOTOR_CONFIG         131    //out message         Motor configuration (min/max throttle, etc)
#define MSP_GPS_CONFIG           132    //out message         GPS configuration
#define MSP_COMPASS_CONFIG       133    //out message         Compass configuration

#define MSP_SET_RAW_RC           200    //in message          8 rc chan
#define MSP_SET_RAW_GPS          201    //in message          fix, numsat, lat, lon, alt, speed
#define MSP_SET_PID              202    //in message          P I D coeff (9 are used currently)
// Legacy multiiwii command that was never used.
//DEPRECATED - #define MSP_SET_BOX              203    //in message          BOX setup (number is dependant of your setup)
#define MSP_SET_RC_TUNING        204    //in message          rc rate, rc expo, rollpitch rate, yaw rate, dyn throttle PID, yaw expo
#define MSP_ACC_CALIBRATION      205    //in message          no param
#define MSP_MAG_CALIBRATION      206    //in message          no param
// Legacy command that was under constant change due to the naming vagueness, avoid at all costs - use more specific commands instead.

#define MSP_RESET_CONF           208    //in message          no param
#define MSP_SET_WP               209    //in message          sets a given WP (WP#,lat, lon, alt, flags)
#define MSP_SELECT_SETTING       210    //in message          Select Setting Number (0-2)
#define MSP_SET_HEADING          211    //in message          define a new heading hold direction
#define MSP_SET_SERVO_CONFIGURATION 212    //in message          Servo settings
#define MSP_SET_MOTOR            214    //in message          PropBalance function
#define MSP_SET_NAV_CONFIG       215    //in message          Sets nav config parameters - write to the eeprom
#define MSP_SET_MOTOR_3D_CONFIG  217    //in message          Settings needed for reversible ESCs
#define MSP_SET_RC_DEADBAND      218    //in message          deadbands for yaw alt pitch roll
#define MSP_SET_RESET_CURR_PID   219    //in message          resetting the current pid profile to defaults
#define MSP_SET_SENSOR_ALIGNMENT 220    //in message          set the orientation of the acc,gyro,mag
#define MSP_SET_LED_STRIP_MODECOLOR 221 //in  message         Set LED strip mode_color settings
#define MSP_SET_MOTOR_CONFIG     222    //out message         Motor configuration (min/max throttle, etc)
#define MSP_SET_GPS_CONFIG       223    //out message         GPS configuration
#define MSP_SET_COMPASS_CONFIG   224    //out message         Compass configuration


#define MSP_EEPROM_WRITE         250    //in message          no param
#define MSP_RESERVE_1            251    //reserved for system usage
#define MSP_RESERVE_2            252    //reserved for system usage
#define MSP_DEBUGMSG             253    //out message         debug string buffer
#define MSP_DEBUG                254    //out message         debug1,debug2,debug3,debug4
#define MSP_RESERVE_3            255    //reserved for system usage

#endif


#endif /* LOCAL_INC_MPS_PROTOCOL_H_ */
