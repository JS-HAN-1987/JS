// ================================================
#ifdef MQ_P1
// ================================================

#define TFT_ROTATION 3

#define USE_OCLOGO
#define xUSE_HIDDENMENU
#define USE_REALTIME_ZOFFSET
#define USE_PROBE_SAFETY_CHECK
#define WORKAROUND_FOR_ILI9325

#define xUSE_DEBUG_OTM

#define USE_HEATER_INACTIVITY
#ifdef USE_HEATER_INACTIVITY
	#define DEFAULT_HEATER_DEACTIVE_TIME 60*5 // 5 minutes
#endif

#define DISABLE_NOZZLE_DETECTION

#define ZOFFSET_MIN 15.0
#define ZOFFSET_MAX 20.0

//#define OC_SUPPORT_DRYRUN
//#define OC_DEFAULT_DRYRUN_VALUE 0

#define IGNORE_MINTEMP
#define OC_TFT

#define OC_EN1 31
#define OC_EN2 33
#define OC_ENC 35

#define xOC_NPDM
	#ifdef OC_NPDM
		#define NPDM_PIN 19 // Z-Max pin
		const bool NPDM_ENDSTOP_INVERTING = false; // set to true to invert the logic of the endstop.
		#define NPDM_ADJUST_OFFSET 0 //0.1

		#define NPDM_START_X 0
		#define NPDM_START_Y 225
		#define NPDM_XYMOVING_Z 35

		#define NPDM_X (195.1+1.0)
		#define NPDM_Y_PROBE  (171.6+2.0)
		#define NPDM_X_PUSH_PROBE (NPDM_X-4.5)
		#define NPDM_X_NOZZLE (197.2)
		#define NPDM_Y_NOZZLE (206.1+2.0)
		#define NPDM_Z_PROBE_OFF (7+5)

	#endif

#define OC_MACHINENAME "Mannequin"
#define OC_FIRMWARE
#define OC_PAUSE
#define MANNEQUIN
//#define MQ_S4

#ifdef OC_TFT
#define SDSUPPORT
#define SDCARDDETECT 49
//#undef SDCARDDETECTINVERTED
#endif

#define COREXY
#define ENABLE_AUTO_BED_LEVELING

#define PROBE_UP_HEIGHT 0.5
#define PROBE_TEST_HEIGHT 20

// PID autotune results
#define  DEFAULT_Kp 23.23
#define  DEFAULT_Ki 1.97
#define  DEFAULT_Kd 68.37

#define TEMP_SENSOR_0 1
#define TEMP_SENSOR_1 0
#define TEMP_SENSOR_2 0
#define TEMP_SENSOR_3 0
#define TEMP_SENSOR_BED 0

const bool X_MIN_ENDSTOP_INVERTING = true; // set to true to invert the logic of the endstop.
const bool Y_MIN_ENDSTOP_INVERTING = false; // set to true to invert the logic of the endstop.
const bool Z_MIN_ENDSTOP_INVERTING = false; // set to true to invert the logic of the endstop.
const bool X_MAX_ENDSTOP_INVERTING = true; // set to true to invert the logic of the endstop.
const bool Y_MAX_ENDSTOP_INVERTING = false; // set to true to invert the logic of the endstop.
const bool Z_MAX_ENDSTOP_INVERTING = false; // set to true to invert the logic of the endstop.

#define INVERT_X_DIR true    // for Mendel set to false, for Orca set to true
#define INVERT_Y_DIR true    // for Mendel set to true, for Orca set to false
#define INVERT_Z_DIR true     // for Mendel set to false, for Orca set to true
#define INVERT_E0_DIR false   // for direct drive extruder v9 set to true, for geared extruder set to false
#define INVERT_E1_DIR false    // for direct drive extruder v9 set to true, for geared extruder set to false
#define INVERT_E2_DIR false   // for direct drive extruder v9 set to true, for geared extruder set to false

#define X_MAX_POS 200
#define X_MIN_POS 0
#define Y_MAX_POS 209
#define Y_PROBE_DOWN_POS Y_MAX_POS
#define G29_PROBE_UP_Y 0
//#define G29_PROBE_UP_Y 50 // for extra fan
#define Y_MIN_POS 0
#define Z_MAX_POS 200
#define Z_MIN_POS 0

#define Z_SAFE_HOMING_X_POINT (X_MAX_LENGTH/2)    // X point for Z homing when homing all axis (G28)
#define Z_SAFE_HOMING_Y_POINT (100)    // Y point for Z homing when homing all axis (G28)


#define MANUAL_X_HOME_POS 0
#define MANUAL_Y_HOME_POS 0
#define MANUAL_Z_HOME_POS 0

#define X_HOME_DIR -1
#define Y_HOME_DIR 1 
#define Z_HOME_DIR -1

#define LEFT_PROBE_BED_POSITION  20 //15
#define RIGHT_PROBE_BED_POSITION 180 //170
#define BACK_PROBE_BED_POSITION  150 //180
#define FRONT_PROBE_BED_POSITION 50 //20
#define AUTO_BED_LEVELING_GRID_POINTS 3

#define X_PROBE_OFFSET_FROM_EXTRUDER 0
#define Y_PROBE_OFFSET_FROM_EXTRUDER 31 // 31.1
#define Z_PROBE_OFFSET_FROM_EXTRUDER -18.0
#define Z_RAISE_BEFORE_HOMING 25 

#define HOMING_FEEDRATE               {8000, 8000, 480, 0}  // set the homing speeds (mm/min)
#define DEFAULT_MAX_FEEDRATE          {500, 500, 20, 25}    // (mm/sec)
#define DEFAULT_MAX_ACCELERATION      {3000, 3000, 100, 10000}    // X, Y, Z, E maximum start speed for accelerated moves. E default values are good for Skeinforge 40+, for older versions raise them a lot.
#define DEFAULT_AXIS_STEPS_PER_UNIT   {78.7402,78.7402,200.0*16/8,101.9}  // default steps per unit for Ultimaker

#define XY_TRAVEL_SPEED 10000         // X and Y axis travel speed between probes, in mm/min

#define EEPROM_SETTINGS
#define PLA_PREHEAT_FAN_SPEED 0   // Insert Value between 0 and 255

#define xREPRAP_DISCOUNT_SMART_CONTROLLER


#define GCODE_STR_HOME      "G28"
#define GCODE_STR_AUTOLEVEL "G29"
#define GCODE_STR_NPDM      "M800"
#define GCODE_STR_PREHEAT   "M104 S185"
#define GCODE_STR_COOLDOWN  "M104 S0"
#define GCODE_STR_MOTOROFF  "M84"

/*
#define GCODE_STR_LEFT10  "G0 X-10 F6000"
#define GCODE_STR_RIGHT10 "G0 X+10 F6000"
#define GCODE_STR_FRONT10 "G0 Y-10 F6000"
#define GCODE_STR_REAR10  "G0 Y+10 F6000"
#define GCODE_STR_UP5     "G0 Z+5 F300"
#define GCODE_STR_DOWN5   "G0 Z-5 F300"

#define GCODE_STR_EXTRACT20 "G0 E+20 F300"
#define GCODE_STR_RETRACT20 "G0 E-20 F300"
*/
// ================================================
#endif // MQ_S4
// ================================================

