/*
 * c64.c
 *
 * Written by
 *  Ettore Perazzoli (ettore@comm2000.it)
 *  Teemu Rantanen (tvr@cs.hut.fi)
 *
 * This file is part of VICE, the Versatile Commodore Emulator.
 * See README for copyright notice.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
 *  02111-1307  USA.
 *
 */

#include "vice.h"

#ifdef STDC_HEADERS
#include <stdio.h>
#include <stdlib.h>
#endif

#include "attach.h"
#include "autostart.h"
#include "c64.h"
#include "c64cia.h"
#include "c64mem.h"
#include "c64tpi.h"
#include "c64ui.h"
#include "cartridge.h"
#include "ciatimer.h"
#include "clkguard.h"
#include "datasette.h"
#include "drive.h"
#include "interrupt.h"
#include "kbd.h"
#include "kbdbuf.h"
#include "log.h"
#include "machine.h"
#include "maincpu.h"
#include "mon.h"
#include "patchrom.h"
#include "psid.h"
#include "resources.h"
#include "reu.h"
#include "serial.h"
#include "sid.h"
#include "snapshot.h"
#include "tape.h"
#include "traps.h"
#include "utils.h"
#include "vicii.h"
#include "vsync.h"

#ifdef USE_VSID
#include "vsidui.h"
#endif

#ifdef HAVE_RS232
#include "c64acia.h"
#include "c64rsuser.h"
#include "rs232.h"
#include "rsuser.h"
#endif

#ifdef HAVE_PRINTER
#include "print.h"
#include "prdevice.h"
#include "pruser.h"
#endif

#ifdef HAVE_MOUSE
#include "mouse.h"
#endif


const char machine_name[] = "C64";

static void vsync_hook(void);

/* ------------------------------------------------------------------------- */

/* Serial traps.  */
static trap_t c64_serial_traps[] = {
    {
        "SerialListen",
        0xED24,
        0xEDAB,
        {0x20, 0x97, 0xEE},
        serialattention
    },
    {
        "SerialSaListen",
        0xED36,
        0xEDAB,
        {0x78, 0x20, 0x8E},
        serialattention
    },
    {
        "SerialSendByte",
        0xED40,
        0xEDAB,
        {0x78, 0x20, 0x97},
        serialsendbyte
    },
    {
        "SerialReceiveByte",
        0xEE13,
        0xEDAB,
        {0x78, 0xA9, 0x00},
        serialreceivebyte
    },
    {
        "SerialReady",
        0xEEA9,
        0xEDAB,
        {0xAD, 0x00, 0xDD},
        trap_serial_ready
    },

    {
        NULL,
        0,
        0,
        {0, 0, 0},
        NULL
    }
};

/* Tape traps.  */
static trap_t c64_tape_traps[] = {
    {
        "TapeFindHeader",
        0xF72F,
        0xF732,
        {0x20, 0x41, 0xF8},
        tape_find_header_trap
    },
    {
        "TapeReceive",
        0xF8A1,
        0xFC93,
        {0x20, 0xBD, 0xFC},
        tape_receive_trap
    },
    {
        NULL,
        0,
        0,
        {0, 0, 0},
        NULL
    }
};

static log_t c64_log = LOG_ERR;

/* ------------------------------------------------------------------------ */

/* C64-specific resource initialization.  This is called before initializing
   the machine itself with `machine_init()'.  */
int machine_init_resources(void)
{
    if (traps_init_resources() < 0
        || vsync_init_resources() < 0
        || video_init_resources() < 0
        || c64_mem_init_resources() < 0
        || vic_ii_init_resources() < 0
        || sound_init_resources() < 0
        || sid_init_resources() < 0
#ifdef HAVE_RS232
        || acia1_init_resources() < 0
        || rs232_init_resources() < 0
        || rsuser_init_resources() < 0
#endif
#ifdef HAVE_PRINTER
        || print_init_resources() < 0
        || prdevice_init_resources() < 0
        || pruser_init_resources() < 0
#endif
#ifdef HAVE_MOUSE
        || mouse_init_resources() < 0
#endif
        || kbd_init_resources() < 0
        || drive_init_resources() < 0
        || cartridge_init_resources() < 0
        )
        return -1;

    return 0;
}

/* C64-specific command-line option initialization.  */
int machine_init_cmdline_options(void)
{
    if (traps_init_cmdline_options() < 0
        || vsync_init_cmdline_options() < 0
        || video_init_cmdline_options() < 0
        || c64_mem_init_cmdline_options() < 0
        || vic_ii_init_cmdline_options() < 0
        || sound_init_cmdline_options() < 0
        || sid_init_cmdline_options() < 0
#ifdef HAVE_RS232
        || acia1_init_cmdline_options() < 0
        || rs232_init_cmdline_options() < 0
        || rsuser_init_cmdline_options() < 0
#endif
#ifdef HAVE_PRINTER
        || print_init_cmdline_options() < 0
        || prdevice_init_cmdline_options() < 0
        || pruser_init_cmdline_options() < 0
#endif
#ifdef HAVE_MOUSE
        || mouse_init_cmdline_options() < 0
#endif
        || kbd_init_cmdline_options() < 0
        || drive_init_cmdline_options() < 0
        || cartridge_init_cmdline_options() < 0
        || psid_init_cmdline_options() < 0
	)
        return -1;

    return 0;
}

/* C64-specific initialization.  */
int machine_init(void)
{
    if (c64_log != LOG_ERR)
        c64_log = log_open("C64");

    maincpu_init();

#ifdef USE_VSID
    if (psid_mode) {
	mem_powerup();

	psid_init_driver();

	/* Initialize the VIC-II emulation.  */
	/*
	if (vic_ii_init() == NULL)
	    return -1;
	*/
	vic_ii_init();
	vic_ii_enable_extended_keyboard_rows(0);
	cia1_enable_extended_keyboard_rows(0);

	ciat_init_table();
	cia1_init();
	cia2_init();

	/* Initialize the monitor.  */
	monitor_init(&maincpu_monitor_interface, &drive0_monitor_interface,
		     &drive1_monitor_interface);

	/* Initialize vsync and register our hook function.  */
	vsync_init(C64_PAL_RFSH_PER_SEC, C64_PAL_CYCLES_PER_SEC, vsync_hook);

	/* Initialize sound.  Notice that this does not really open the audio
	   device yet.  */
	sound_init(C64_PAL_CYCLES_PER_SEC, C64_PAL_CYCLES_PER_RFSH);

	/* Initialize keyboard buffer.  */
	kbd_buf_init(631, 198, 10, C64_PAL_CYCLES_PER_RFSH * C64_PAL_RFSH_PER_SEC);

	/* Initialize the C64-specific part of the UI.  */
	if (!console_mode) {
	    vsid_ui_init();
	}

	return 0;
    }
#endif

    if (mem_load() < 0)
        return -1;

    /* Setup trap handling.  */
    traps_init();

    /* Initialize serial traps.  */
    serial_init(c64_serial_traps);

    /* Initialize drives. */
    file_system_init();

#ifdef HAVE_RS232
    /* Initialize RS232 handler.  */
    rs232_init();
    c64_rsuser_init();
#endif

#ifdef HAVE_PRINTER
    /* Initialize print devices.  */
    print_init();
#endif

    /* Initialize the tape emulation.  */
    tape_init(0xb2, 0x90, 0x93, 0x29f, 0, 0xc1, 0xae, 0x277, 0xc6,
              c64_tape_traps);

    /* Initialize the datasette emulation.  */
    datasette_init();

    /* Fire up the hardware-level 1541 emulation.  */
    drive_init(C64_PAL_CYCLES_PER_SEC, C64_NTSC_CYCLES_PER_SEC);

    /* Initialize autostart.  */
    autostart_init(3 * C64_PAL_RFSH_PER_SEC * C64_PAL_CYCLES_PER_RFSH, 1,
                   0xcc, 0xd1, 0xd3, 0xd5);

    /* Initialize the VIC-II emulation.  */
    if (vic_ii_init() == NULL && !console_mode)
        return -1;
    vic_ii_enable_extended_keyboard_rows(0);

    cia1_enable_extended_keyboard_rows(0);

    ciat_init_table();
    cia1_init();
    cia2_init();

    tpi_init();

#ifdef HAVE_RS232
    acia1_init();
#endif
    /* Initialize the keyboard.  */
    if (c64_kbd_init() < 0)
        return -1;

    /* Initialize the monitor.  */
    monitor_init(&maincpu_monitor_interface, &drive0_monitor_interface,
                 &drive1_monitor_interface);

    /* Initialize vsync and register our hook function.  */
    vsync_init(C64_PAL_RFSH_PER_SEC, C64_PAL_CYCLES_PER_SEC, vsync_hook);

    /* Initialize sound.  Notice that this does not really open the audio
       device yet.  */
    sound_init(C64_PAL_CYCLES_PER_SEC, C64_PAL_CYCLES_PER_RFSH);

    /* Initialize keyboard buffer.  */
    kbd_buf_init(631, 198, 10, C64_PAL_CYCLES_PER_RFSH * C64_PAL_RFSH_PER_SEC);

    /* Initialize the C64-specific part of the UI.  */
    c64_ui_init();
    log_message(LOG_DEFAULT, "C64 UI initialized successfully.");

    /* Initialize the REU.  */
    reu_init();

#ifdef HAVE_MOUSE
    /* Initialize mouse support (if present).  */
    mouse_init();
#endif

    return 0;
}

/* C64-specific reset sequence.  */
void machine_reset(void)
{
    serial_reset();
    cia1_reset();
    cia2_reset();
    sid_reset();

    if (psid_mode) {
        vic_ii_reset();

        psid_init_driver();
	psid_init_tune();
	return;
    }

    reset_tpi();

#ifdef HAVE_RS232
    reset_acia1();
    rs232_reset();
    rsuser_reset();
#endif

#ifdef HAVE_PRINTER
    print_reset();
#endif

    /* FIXME */
    /* reset_reu(); */

    /* The VIC-II must be the *last* to be reset.  */
    vic_ii_reset();

    autostart_reset();
    drive_reset();
}

void machine_powerup(void)
{
    mem_powerup();
    maincpu_trigger_reset();
}

void machine_shutdown(void)
{
    /* Detach all disks.  */
    if (!psid_mode) {
        file_system_detach_disk(-1);
    }

#ifdef USE_VSID
    if (!console_mode && psid_mode) {
        vsid_ui_exit();
    }
#endif
}

void machine_handle_pending_alarms(int num_write_cycles)
{
     vic_ii_handle_pending_alarms(num_write_cycles);
}

/* ------------------------------------------------------------------------- */

/* This hook is called at the end of every frame.  */
static void vsync_hook(void)
{
    CLOCK sub;

    if (psid_mode) {
        clk_guard_prevent_overflow(&maincpu_clk_guard);
	return;
    }
      
    drive_vsync_hook();

    autostart_advance();

    sub = clk_guard_prevent_overflow(&maincpu_clk_guard);

    /* The drive has to deal both with our overflowing and its own one, so
       it is called even when there is no overflowing in the main CPU.  */
    /* FIXME: Do we have to check drive_enabled here?  */
    drive_prevent_clk_overflow(sub, 0);
    drive_prevent_clk_overflow(sub, 1);
}

int machine_set_restore_key(int v)
{
    maincpu_set_nmi(I_RESTORE, v ? 1 : 0);
    return 1;
}

/* ------------------------------------------------------------------------- */

long machine_get_cycles_per_second(void)
{
    return C64_PAL_CYCLES_PER_SEC;
}

/* ------------------------------------------------------------------------- */

#define SNAP_MAJOR 1
#define SNAP_MINOR 0

int machine_class = VICE_MACHINE_C64;

int machine_write_snapshot(const char *name, int save_roms, int save_disks)
{
    snapshot_t *s;

    s = snapshot_create(name, SNAP_MAJOR, SNAP_MINOR, machine_name);
    if (s == NULL)
        return -1;

    /* Execute drive CPUs to get in sync with the main CPU.  */
    drive_cpu_execute(clk);

    if (maincpu_write_snapshot_module(s) < 0
        || mem_write_snapshot_module(s, save_roms) < 0
        || cia1_write_snapshot_module(s) < 0
        || cia2_write_snapshot_module(s) < 0
        || sid_write_snapshot_module(s) < 0
        || drive_write_snapshot_module(s, save_disks, save_roms) < 0
        || vic_ii_write_snapshot_module(s) < 0) {
        snapshot_close(s);
        remove_file(name);
        return -1;
    }

    snapshot_close(s);
    return 0;
}

int machine_read_snapshot(const char *name)
{
    snapshot_t *s;
    BYTE minor, major;

    s = snapshot_open(name, &major, &minor, machine_name);
    if (s == NULL)
        return -1;

    if (major != SNAP_MAJOR || minor != SNAP_MINOR) {
        log_error(c64_log,
                  "Snapshot version (%d.%d) not valid: expecting %d.%d.",
                  major, minor, SNAP_MAJOR, SNAP_MINOR);
        goto fail;
    }

    vic_ii_prepare_for_snapshot();

    if (maincpu_read_snapshot_module(s) < 0
        || mem_read_snapshot_module(s) < 0
        || cia1_read_snapshot_module(s) < 0
        || cia2_read_snapshot_module(s) < 0
        || sid_read_snapshot_module(s) < 0
        || drive_read_snapshot_module(s) < 0
        || vic_ii_read_snapshot_module(s) < 0)
        goto fail;

    snapshot_close(s);

    return 0;

fail:
    if (s != NULL)
        snapshot_close(s);
    maincpu_trigger_reset();
    return -1;
}


/* ------------------------------------------------------------------------- */
int machine_autodetect_psid(const char *name)
{
  if (name == NULL) {
      return -1;
  }

  return psid_load_file(name);
}
