/**
 * Arduino Mega with RAMPS v1.3 pin assignments
 *
 * Applies to the following boards:
 *
 *  RAMPS_13_EFB (Extruder, Fan, Bed)
 *  RAMPS_13_EEB (Extruder, Extruder, Bed)
 *  RAMPS_13_EFF (Extruder, Fan, Fan)
 *  RAMPS_13_EEF (Extruder, Extruder, Fan)
 *
 *  Other pins_MYBOARD.h files may override these defaults
 */

#if !defined(__AVR_ATmega1280__) && !defined(__AVR_ATmega2560__)
  #error Oops!  Make sure you have 'Arduino Mega' selected from the 'Tools -> Boards' menu.
#endif

#define LARGE_FLASH true

#define X_STEP_PIN         54
#define X_DIR_PIN          55
#define X_ENABLE_PIN       38
#define X_MIN_PIN           3
#define X_MAX_PIN           2

#define Y_STEP_PIN         60
#define Y_DIR_PIN          61
#define Y_ENABLE_PIN       56
#if defined(MQ_S1) || defined(MQ_S4) || defined(MQ_P1)
#define Y_MIN_PIN          -1
#define Y_MAX_PIN          14
#else
#define Y_MIN_PIN          14
#define Y_MAX_PIN          -1
#endif

#define Z_STEP_PIN         46
#define Z_DIR_PIN          48
#define Z_ENABLE_PIN       62
#define Z_MIN_PIN          18
#define Z_MAX_PIN          -1 //19

#define Y2_STEP_PIN        36
#define Y2_DIR_PIN         34
#define Y2_ENABLE_PIN      30

#define Z2_STEP_PIN        36
#define Z2_DIR_PIN         34
#define Z2_ENABLE_PIN      30

#define E0_STEP_PIN        26
#define E0_DIR_PIN         28
#define E0_ENABLE_PIN      24

#define E1_STEP_PIN        36
#define E1_DIR_PIN         34
#define E1_ENABLE_PIN      30

#define SDPOWER            -1
#define SDSS               53
#define LED_PIN            13

#if MB(RAMPS_13_EEB) && defined(FILAMENT_SENSOR)  // FMM added for Filament Extruder
  // define analog pin for the filament width sensor input
  // Use the RAMPS 1.4 Analog input 5 on the AUX2 connector
  #define FILWIDTH_PIN        5
#endif

#if MB(RAMPS_13_EFB) || MB(RAMPS_13_EFF)
  #define FAN_PIN            9 // (Sprinter config)
  #if MB(RAMPS_13_EFF)
    #define CONTROLLERFAN_PIN  -1 // Pin used for the fan to cool controller
  #endif
#elif MB(RAMPS_13_EEF)
  #define FAN_PIN            8
#else
  #define FAN_PIN            4 // IO pin. Buffer needed
#endif

#define PS_ON_PIN          12

#if defined(REPRAP_DISCOUNT_SMART_CONTROLLER) || defined(G3D_PANEL)
  #define KILL_PIN           41
#else
  #define KILL_PIN           -1
#endif

#if MB(RAMPS_13_EFF)
  #define HEATER_0_PIN       8
#else
  #define HEATER_0_PIN       10   // EXTRUDER 1
#endif

#if MB(RAMPS_13_EFB)
  #define HEATER_1_PIN       -1
#else
  #define HEATER_1_PIN       9    // EXTRUDER 2 (FAN On Sprinter)
#endif

#define HEATER_2_PIN       -1

#define TEMP_0_PIN         13   // ANALOG NUMBERING
#define TEMP_1_PIN         15   // ANALOG NUMBERING
#define TEMP_2_PIN         -1   // ANALOG NUMBERING

#if MB(RAMPS_13_EFF) || MB(RAMPS_13_EEF)
  #define HEATER_BED_PIN     -1    // NO BED
#else
  #define HEATER_BED_PIN     8    // BED
#endif

#define TEMP_BED_PIN       14   // ANALOG NUMBERING

#ifdef NUM_SERVOS
  #define SERVO0_PIN         11
  #if NUM_SERVOS > 1
    #define SERVO1_PIN        6
    #if NUM_SERVOS > 2
      #define SERVO2_PIN      5
      #if NUM_SERVOS > 3
        #define SERVO3_PIN    4
      #endif
    #endif
  #endif
#endif

#ifdef ULTRA_LCD

  #ifdef NEWPANEL
    #define LCD_PINS_RS 16
    #define LCD_PINS_ENABLE 17
    #define LCD_PINS_D4 23
    #define LCD_PINS_D5 25
    #define LCD_PINS_D6 27
    #define LCD_PINS_D7 29
    #define BEEPER -1  // Beeper on AUX-4
	#define BTN_EN1 31
	#define BTN_EN2 33
	#define BTN_ENC 35
  #endif
#endif // ULTRA_LCD

// SPI for Max6675 Thermocouple
#ifndef SDSUPPORT
  #define MAX6675_SS       66 // Do not use pin 53 if there is even the remote possibility of using Display/SD card
#else
  #define MAX6675_SS       66 // Do not use pin 49 as this is tied to the switch inside the SD card socket to detect if there is an SD card present
#endif

#ifndef SDSUPPORT
  // these pins are defined in the SD library if building with SD support
  #define SCK_PIN          52
  #define MISO_PIN         50
  #define MOSI_PIN         51
#endif
