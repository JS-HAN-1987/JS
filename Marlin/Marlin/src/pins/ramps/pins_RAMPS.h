/**
 * Marlin 3D Printer Firmware
 * Copyright (c) 2019 MarlinFirmware [https://github.com/MarlinFirmware/Marlin]
 *
 * Based on Sprinter and grbl.
 * Copyright (c) 2011 Camiel Gubbels / Erik van der Zalm
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#pragma once

/**
 * Arduino Mega with RAMPS v1.4 (or v1.3) pin assignments
 *
 * Applies to the following boards:
 *
 *  RAMPS_14_EFB (Hotend, Fan, Bed)
 *  RAMPS_14_EEB (Hotend0, Hotend1, Bed)
 *  RAMPS_14_EFF (Hotend, Fan0, Fan1)
 *  RAMPS_14_EEF (Hotend0, Hotend1, Fan)
 *  RAMPS_14_SF  (Spindle, Controller Fan)
 *
 *  RAMPS_13_EFB (Hotend, Fan, Bed)
 *  RAMPS_13_EEB (Hotend0, Hotend1, Bed)
 *  RAMPS_13_EFF (Hotend, Fan0, Fan1)
 *  RAMPS_13_EEF (Hotend0, Hotend1, Fan)
 *  RAMPS_13_SF  (Spindle, Controller Fan)
 *
 *  Other pins_MYBOARD.h files may override these defaults
 *
 *  Differences between
 *  RAMPS_13 | RAMPS_14
 *         7 | 11
 */

#ifdef TARGET_LPC1768
  #error "Oops! Set MOTHERBOARD to an LPC1768-based board when building for LPC1768."
#elif defined(__STM32F1__)
  #error "Oops! Set MOTHERBOARD to an STM32F1-based board when building for STM32F1."
#endif

#if NONE(IS_RAMPS_SMART, IS_RAMPS_DUO, IS_RAMPS4DUE, TARGET_LPC1768)
  #if !defined(__AVR_ATmega1280__) && !defined(__AVR_ATmega2560__)
    #error "Oops! Select 'Arduino/Genuino Mega or Mega 2560' in 'Tools > Board.'"
  #endif
#endif

#ifndef BOARD_INFO_NAME
  #define BOARD_INFO_NAME "RAMPS 1.4"
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
//////////////////////////
// LCDs and Controllers //
//////////////////////////

#if HAS_SPI_LCD

  //
  // LCD Display output pins
  //
  #if ENABLED(MANNEQUIN)
	#define LCD_RESET 47 // A4 // 47
	#define LCD_CS 32 // A3 // 32
	#define LCD_CD 59 // A2 // 59(A5)
	#define LCD_WR 58 // A1 // 58(A4)
	#define LCD_RD 40 // A0 // 40

	#define LCD_D0 63
	#define LCD_D1 64
	#define LCD_D2 65
	#define LCD_D3 66
	#define LCD_D4 45
	#define LCD_D5 44
	#define LCD_D6 43
	#define LCD_D7 42

  #elif ENABLED(REPRAPWORLD_GRAPHICAL_LCD)

    #define LCD_PINS_RS         49   // CS chip select /SS chip slave select
    #define LCD_PINS_ENABLE     51   // SID (MOSI)
    #define LCD_PINS_D4         52   // SCK (CLK) clock

  #elif BOTH(NEWPANEL, PANEL_ONE)

    #define LCD_PINS_RS         40
    #define LCD_PINS_ENABLE     42
    #define LCD_PINS_D4         65
    #define LCD_PINS_D5         66
    #define LCD_PINS_D6         44
    #define LCD_PINS_D7         64

  #else

    #if ENABLED(CR10_STOCKDISPLAY)

      #define LCD_PINS_RS       27
      #define LCD_PINS_ENABLE   29
      #define LCD_PINS_D4       25

      #if DISABLED(NEWPANEL)
        #define BEEPER_PIN      37
      #endif

    #elif ENABLED(ZONESTAR_LCD)

      #define LCD_PINS_RS       64
      #define LCD_PINS_ENABLE   44
      #define LCD_PINS_D4       63
      #define LCD_PINS_D5       40
      #define LCD_PINS_D6       42
      #define LCD_PINS_D7       65

    #else

      #if EITHER(MKS_12864OLED, MKS_12864OLED_SSD1306)
        #define LCD_PINS_DC     25   // Set as output on init
        #define LCD_PINS_RS     27   // Pull low for 1s to init
        // DOGM SPI LCD Support
        #define DOGLCD_CS       16
        #define DOGLCD_MOSI     17
        #define DOGLCD_SCK      23
        #define DOGLCD_A0       LCD_PINS_DC
      #else
        #define LCD_PINS_RS     16
        #define LCD_PINS_ENABLE 17
        #define LCD_PINS_D4     23
        #define LCD_PINS_D5     25
        #define LCD_PINS_D6     27
      #endif

      #define LCD_PINS_D7       29

      #if DISABLED(NEWPANEL)
        #define BEEPER_PIN      33
      #endif

    #endif

    #if DISABLED(NEWPANEL)
      // Buttons attached to a shift register
      // Not wired yet
      //#define SHIFT_CLK       38
      //#define SHIFT_LD        42
      //#define SHIFT_OUT       40
      //#define SHIFT_EN        17
    #endif

  #endif

  //
  // LCD Display input pins
  //
  #if ENABLED(NEWPANEL)

    #if ENABLED(REPRAP_DISCOUNT_SMART_CONTROLLER)

      #define BEEPER_PIN        37

      #if ENABLED(CR10_STOCKDISPLAY)
        #define BTN_EN1         17
        #define BTN_EN2         23
      #else
        #define BTN_EN1         31
        #define BTN_EN2         33
      #endif

      #define BTN_ENC           35
      #ifndef SD_DETECT_PIN
        #define SD_DETECT_PIN   49
      #endif
      #define KILL_PIN          41

      #if ENABLED(BQ_LCD_SMART_CONTROLLER)
        #define LCD_BACKLIGHT_PIN 39
      #endif

    #elif ENABLED(REPRAPWORLD_GRAPHICAL_LCD)

      #define BTN_EN1           64
      #define BTN_EN2           59
      #define BTN_ENC           63
      #define SD_DETECT_PIN     42

    #elif ENABLED(LCD_I2C_PANELOLU2)

      #define BTN_EN1           47
      #define BTN_EN2           43
      #define BTN_ENC           32
      #define LCD_SDSS          SDSS
      #define KILL_PIN          41

    #elif ENABLED(LCD_I2C_VIKI)

      #define BTN_EN1           40   // http://files.panucatt.com/datasheets/viki_wiring_diagram.pdf explains 40/42.
      #define BTN_EN2           42
      #define BTN_ENC           -1

      #define LCD_SDSS          SDSS
      #define SD_DETECT_PIN     49

    #elif ANY(VIKI2, miniVIKI)

      #define DOGLCD_CS         45
      #define DOGLCD_A0         44
      #define LCD_SCREEN_ROT_180

      #define BEEPER_PIN        33
      #define STAT_LED_RED_PIN  32
      #define STAT_LED_BLUE_PIN 35

      #define BTN_EN1           22
      #define BTN_EN2            7
      #define BTN_ENC           39

      #define SD_DETECT_PIN     -1   // Pin 49 for display SD interface, 72 for easy adapter board
      #define KILL_PIN          31

    #elif ENABLED(ELB_FULL_GRAPHIC_CONTROLLER)

      #define DOGLCD_CS         29
      #define DOGLCD_A0         27

      #define BEEPER_PIN        23
      #define LCD_BACKLIGHT_PIN 33

      #define BTN_EN1           35
      #define BTN_EN2           37
      #define BTN_ENC           31

      #define LCD_SDSS          SDSS
      #define SD_DETECT_PIN     49
      #define KILL_PIN          41

    #elif EITHER(MKS_MINI_12864, FYSETC_MINI_12864)

      #define BEEPER_PIN        37
      #define BTN_ENC           35
      #define SD_DETECT_PIN     49

      #ifndef KILL_PIN
        #define KILL_PIN        41
      #endif

      #if ENABLED(MKS_MINI_12864)   // Added in Marlin 1.1.6

        #define DOGLCD_A0       27
        #define DOGLCD_CS       25

        // GLCD features
        // Uncomment screen orientation
        //#define LCD_SCREEN_ROT_90
        //#define LCD_SCREEN_ROT_180
        //#define LCD_SCREEN_ROT_270

        // not connected to a pin
        #define LCD_BACKLIGHT_PIN 65   // backlight LED on A11/D65

        #define BTN_EN1         31
        #define BTN_EN2         33

      #elif ENABLED(FYSETC_MINI_12864)

        // From https://wiki.fysetc.com/Mini12864_Panel/?fbclid=IwAR1FyjuNdVOOy9_xzky3qqo_WeM5h-4gpRnnWhQr_O1Ef3h0AFnFXmCehK8

        #define DOGLCD_A0       16
        #define DOGLCD_CS       17

        #define BTN_EN1         33
        #define BTN_EN2         31

        //#define FORCE_SOFT_SPI    // Use this if default of hardware SPI causes display problems
                                    //   results in LCD soft SPI mode 3, SD soft SPI mode 0

        #define LCD_RESET_PIN   23   // Must be high or open for LCD to operate normally.

        #if EITHER(FYSETC_MINI_12864_1_2, FYSETC_MINI_12864_2_0)
          #ifndef RGB_LED_R_PIN
            #define RGB_LED_R_PIN 25
          #endif
          #ifndef RGB_LED_G_PIN
            #define RGB_LED_G_PIN 27
          #endif
          #ifndef RGB_LED_B_PIN
            #define RGB_LED_B_PIN 29
          #endif
        #elif ENABLED(FYSETC_MINI_12864_2_1)
          #define NEOPIXEL_PIN    25
        #endif

    #endif

    #elif ENABLED(MINIPANEL)

      #define BEEPER_PIN        42
      // not connected to a pin
      #define LCD_BACKLIGHT_PIN 65   // backlight LED on A11/D65

      #define DOGLCD_A0         44
      #define DOGLCD_CS         66

      // GLCD features
      // Uncomment screen orientation
      //#define LCD_SCREEN_ROT_90
      //#define LCD_SCREEN_ROT_180
      //#define LCD_SCREEN_ROT_270

      #define BTN_EN1           40
      #define BTN_EN2           63
      #define BTN_ENC           59

      #define SD_DETECT_PIN     49
      #define KILL_PIN          64

    #elif ENABLED(ZONESTAR_LCD)

      #define ADC_KEYPAD_PIN    12

    #elif ENABLED(AZSMZ_12864)

      // Pins only defined for RAMPS_SMART currently
	#elif ENABLED(MANNEQUIN)
	  #define BTN_EN1 31
	  #define BTN_EN2 33
	  #define BTN_ENC 35
	  #define SD_DETECT_PIN 49
    #else

      // Beeper on AUX-4
      #define BEEPER_PIN        33

      // Buttons are directly attached to AUX-2
      #if ENABLED(REPRAPWORLD_KEYPAD)
        #define SHIFT_OUT       40
        #define SHIFT_CLK       44
        #define SHIFT_LD        42
        #define BTN_EN1         64
        #define BTN_EN2         59
        #define BTN_ENC         63
      #elif ENABLED(PANEL_ONE)
        #define BTN_EN1         59   // AUX2 PIN 3
        #define BTN_EN2         63   // AUX2 PIN 4
        #define BTN_ENC         49   // AUX3 PIN 7
      #else
        #define BTN_EN1         37
        #define BTN_EN2         35
        #define BTN_ENC         31
      #endif

      #if ENABLED(G3D_PANEL)
        #define SD_DETECT_PIN   49
        #define KILL_PIN        41
      #endif

    #endif
  #endif // NEWPANEL

#endif // HAS_SPI_LCD


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
