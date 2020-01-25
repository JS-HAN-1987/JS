/*
  main.c - main grbl simulator program

  Part of Grbl Simulator

  Copyright (c) 2012 Jens Geisler
  Copyright (c) 2014-2015 Adam Shelly

  Grbl is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Grbl is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with Grbl.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>

#include "simulator.h"
#include "grbl_interface.h"
#include "../grbl.h"


arg_vars_t args;
const char* progname;

int usage(const char* badarg){
  if (badarg){
   printf("Unrecognized option %s\n",badarg);
  }
  printf("Usage: \n"
      "%s [options] [time_step] [block_file]\n"
      "  Options:\n"
      "    -r <report time>   : minimum time step for printing stepper values. Default=0=no print.\n"
      "    -t <time factor>   : multiplier to realtime clock. Default=1. (needs work)\n"
      "    -g <response file> : file to report responses from grbl.  default = stdout\n"
      "    -b <block file>    : file to report each block executed.  default = stdout\n"
      "    -s <step file>     : file to report each step executed.  default = stderr\n"
      "    -c<comment_char>   : character to print before each line from grbl.  default = '#'\n"
      "    -n                 : no comments before grbl response lines.\n"
      "    -h                 : this help.\n"
      "\n  <time_step> and <block_file> can be specifed with option flags or positional parameters\n"
      "\n  ^-F to shutdown cleanly\n\n",
      progname);
  return -1;
}

//prototype for renamed original main function
int main_org(void);
//wrapper for thread interface
PLAT_THREAD_FUNC(avr_main_thread,exit){
	main_org();
  return NULL;
}

int main(int argc, char *argv[]) {
  float tick_rate=1.0;
  int positional_args=0;

  //defaults
  args.step_out_file = stderr;
  args.block_out_file = stdout;
  args.serial_out_file = stdout;
  args.comment_char = '#';

  args.step_time = 0.0;
  // Get the minimum time step for printing stepper values.
  // If not given or the command line cannot be parsed to a float than
  // step_time= 0.0; This means to not print stepper values at all

  progname = argv[0];
#ifdef WIN32
  winserial_init(argv[1]);
#endif

  while (argc>1) {
    argv++;argc--;
    if (argv[0][0] == '-'){
     switch(argv[0][1]){
     case 'c':  //set Comment char 
      args.comment_char = argv[0][2];
      break;
     case 'n': //No comment char on grbl responses
      args.comment_char = 0;
      break;
     case 't': //Tick rate
      argv++;argc--;
      tick_rate = atof(*argv);
      break;
     case 'b': //Block file
      argv++;argc--;
      args.block_out_file = fopen(*argv,"w");
      if (!args.block_out_file) {
        perror("fopen");
        printf("Error opening : %s\n",*argv);
        return(usage(0));
      }
      break;
     case 's': //Step out file.
      argv++;argc--;
      args.step_out_file = fopen(*argv,"w");
      if (!args.step_out_file) {
        perror("fopen");
        printf("Error opening : %s\n",*argv);
        return(usage(0));
      }
      break;
     case 'g': //Grbl output
      argv++;argc--;
      args.serial_out_file = fopen(*argv,"w");
      if (!args.serial_out_file) {
        perror("fopen");
        printf("Error opening : %s\n",*argv);
        return(usage(0));
      }
      break;
     case 'r':  //step_time for Reporting
      argv++;argc--;
      args.step_time= atof(*argv);
      break;
     case 'h':
      return usage(NULL);
     default:
      return usage(*argv);
     }
    }
    else { //handle old positional argument interface
     positional_args++;
     switch(positional_args){
     case 1:
      args.step_time= atof(*argv);
      break;
     case 2:  //block out and grbl out to same file, like before.
      args.block_out_file = fopen(*argv,"w");
      if (!args.block_out_file) {
        perror("fopen");
        printf("Error opening : %s\n",*argv);
        return(usage(0));
      }
      args.serial_out_file = args.block_out_file;
      break;
     default:
      return usage(*argv);
     }
    }
  }

  // Make sure the output streams are flushed immediately.
  // This is important when using the simulator inside another application in parallel
  // to the real grbl.
  // Theoretically flushing could be limited to complete lines. Unfortunately Windows
  // does not know line buffered streams. So for now we stick to flushing every character.
  //setvbuf(stdout, NULL, _IONBF, 1);
  //setvbuf(stderr, NULL, _IONBF, 1);
  //( Files are now closed cleanly when sim gets EOF or CTRL-F.)
  platform_init(); 

  sim_add_hooks(grbl_app_init, grbl_per_tick, grbl_per_byte, grbl_app_exit);

  init_simulator(tick_rate);

  //launch a thread with the original grbl code.
  plat_thread_t*th = platform_start_thread(avr_main_thread); 
  if (!th){
   printf("Fatal: Unable to start hardware thread.\n");
   exit(-5);
  }

  //All the stream io and interrupt happen in this thread.
  sim_loop();

  // Graceful exit
  shutdown_simulator();

  platform_kill_thread(th); //need force kill since original main has no return.

  //close the files we opened
  fclose(args.block_out_file);
  fclose(args.step_out_file);
  fclose(args.serial_out_file);

  platform_terminate();


  exit(EXIT_SUCCESS);
}

// Declare system global variable structure
system_t sys;

int32_t sys_position[N_AXIS];      // Real-time machine (aka home) position vector in steps.
int32_t sys_probe_position[N_AXIS]; // Last probe position in machine coordinates and steps.
volatile uint8_t sys_probe_state;   // Probing state value.  Used to coordinate the probing cycle with stepper ISR.
volatile uint8_t sys_rt_exec_state;   // Global realtime executor bitflag variable for state management. See EXEC bitmasks.
volatile uint8_t sys_rt_exec_alarm;   // Global realtime executor bitflag variable for setting various alarms.
volatile uint8_t sys_rt_exec_motion_override; // Global realtime executor bitflag variable for motion-based overrides.
volatile uint8_t sys_rt_exec_accessory_override; // Global realtime executor bitflag variable for spindle/coolant overrides.
#ifdef DEBUG
volatile uint8_t sys_rt_exec_debug;
#endif


int main_org(void)
{
	// Initialize system upon power-up.
	serial_init();   // Setup serial baud rate and interrupts
	settings_init(); // Load Grbl settings from EEPROM
	stepper_init();  // Configure stepper pins and interrupt timers
	system_init();   // Configure pinout pins and pin-change interrupt

	memset(sys_position, 0, sizeof(sys_position)); // Clear machine position.
	sei(); // Enable interrupts

	// Initialize system state.
#ifdef FORCE_INITIALIZATION_ALARM
  // Force Grbl into an ALARM state upon a power-cycle or hard reset.
	sys.state = STATE_ALARM;
#else
	sys.state = STATE_IDLE;
#endif

	// Check for power-up and set system alarm if homing is enabled to force homing cycle
	// by setting Grbl's alarm state. Alarm locks out all g-code commands, including the
	// startup scripts, but allows access to settings and internal commands. Only a homing
	// cycle '$H' or kill alarm locks '$X' will disable the alarm.
	// NOTE: The startup script will run after successful completion of the homing cycle, but
	// not after disabling the alarm locks. Prevents motion startup blocks from crashing into
	// things uncontrollably. Very bad.
#ifdef HOMING_INIT_LOCK
	if (bit_istrue(settings.flags, BITFLAG_HOMING_ENABLE)) { sys.state = STATE_ALARM; }
#endif

	// Grbl initialization loop upon power-up or a system abort. For the latter, all processes
	// will return to this loop to be cleanly re-initialized.
	for (;;) {

		// Reset system variables.
		uint8_t prior_state = sys.state;
		memset(&sys, 0, sizeof(system_t)); // Clear system struct variable.
		sys.state = prior_state;
		sys.f_override = DEFAULT_FEED_OVERRIDE;  // Set to 100%
		sys.r_override = DEFAULT_RAPID_OVERRIDE; // Set to 100%
		sys.spindle_speed_ovr = DEFAULT_SPINDLE_SPEED_OVERRIDE; // Set to 100%
		memset(sys_probe_position, 0, sizeof(sys_probe_position)); // Clear probe position.
		sys_probe_state = 0;
		sys_rt_exec_state = 0;
		sys_rt_exec_alarm = 0;
		sys_rt_exec_motion_override = 0;
		sys_rt_exec_accessory_override = 0;

		// Reset Grbl primary systems.
		serial_reset_read_buffer(); // Clear serial read buffer
		gc_init(); // Set g-code parser to default state
		spindle_init();
		coolant_init();
		limits_init();
		probe_init();
		plan_reset(); // Clear block buffer and planner variables
		st_reset(); // Clear stepper subsystem variables.

		// Sync cleared gcode and planner positions to current system position.
		plan_sync_position();
		gc_sync_position();

		// Print welcome message. Indicates an initialization has occured at power-up or with a reset.
		report_init_message();

		// Start Grbl main loop. Processes program inputs and executes them.
		protocol_main_loop();

	}
	return 0;   /* Never reached */
}
